#include "stdafx.h"
#include "..\Headers\Player.h"

#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Layer.h"
#include "cell.h"

#include "camera_Manager.h"
#include "Input_Device.h"
#include "CrossBow.h"
#include "Arrow.h"
#include "Terrain.h"
#include "BaseObj.h"
#include "BossTerrain.h"

#include "LightningTower.h"
#include "DragonKiller.h"
#include "MisileTower.h"
#include "Shield.h"

#include "Door.h"

#include "Boss.h"
#include "SnowmanBoss.h"
#include "Goblin.h"
#include "OrcBruiser.h"
#include "DarkElfArcher.h"
#include "Kobold.h"

#include "Fire_Explosion.h"
#include "Subject_Manager.h"

#include "management.h"

#include "Tree.h"

#include "Camera_Static.h"
#include "Camera_Dynamic.h"
#include "Sound_Manager.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pBufferCom(nullptr), m_pRendererCom(nullptr), m_pTargetBufferCom(nullptr), m_pTransformCom(nullptr), m_pPickingCom(nullptr), m_pColliderCom(nullptr)
	, m_pRHandMatrix(nullptr), m_fTimeDelta(0.f), m_fDelayTime(0.f), m_bJump(false), m_bMotion(false), m_bTurnLeft(false), m_bTurnRight(false)
	, m_bJumpFall(false), m_bSummon(false), m_pNavigationCom(nullptr), m_bJumpMove(false), m_bMoveJump(false), m_bStaticCamera(true), m_bAttack(true), m_bNotWeapon(false)
	, m_bReload(false), m_pBow(nullptr), m_bBuild(true), m_fJumpPower(7.f), m_fAniTime(1.f), m_bFailBuild(true)
	, m_vJumpDir(_vec3(0.f, 0.f, 0.f)) , m_vJumpNextPos(_vec3(0.f, 0.f, 0.f)), m_fAttackTime(0.f), m_fAttackCount(10.f), m_fAttackMaxCount(10.f)
	, m_fMouseMoveSpeed(7.f), m_pShaderCom(nullptr), m_bHit(false), m_bHitCol(false), m_fHitColTime(0.f)
{
	m_pInput_Device->AddRef();

}

HRESULT CPlayer::Ready_GameObject(void)
{
	e_CurType = SCENE_STAGE;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pBow = CCrossBow::Create(m_pGraphicDev);
	if (nullptr == m_pBow)
		return E_FAIL;

	m_pRHandMatrix = m_pBufferCom->Get_BoneMatrix("b_WristR");

	// Stage
	m_pTransformCom->Set_Position(_vec3(58.f, 0.f, 64.f));

	// Boss
	//m_pTransformCom->Set_Position(_vec3(62.f, 0.f, 30.f));

	m_pTransformCom->Scaling(_vec3(0.05f, 0.05f, 0.05f));

	m_pTransformCom->Rotation_Y(-90.f);

	m_pBufferCom->Set_AnimationSet(22);

	return NOERROR;
}

_int CPlayer::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bNotUpdate)
		return 0;

	m_fTimeDelta = fTimeDelta;
	
	if(e_CurType == SCENE_STAGE)
		Collision_ToObject();

	Key_Input(fTimeDelta);

	if (!m_bJump)
	{
		Move_OnTerrain();
	}
	else
		Jump(fTimeDelta);

	if (m_bHit)
	{
		Hit(fTimeDelta);
		CollTime(fTimeDelta);
	}

	Attack(fTimeDelta);

	State_Update();

	m_pTransformCom->Make_WorldMatrix();

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pBow->Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);
	
	return 0;
}

_int CPlayer::LastUpdate_GameObject(const _float & fTimeDelta)
{
	CComponent* pTransform_Sword = m_pBow->Get_Component(L"Com_Transform");
	if (nullptr == pTransform_Sword)
		return -1;
	_matrix matPostion;
	matPostion.Identity();
	m_pTransformCom->Get_WorldMatrix(&matPostion, nullptr);
	((CTransform*)pTransform_Sword)->Parent((*m_pRHandMatrix * matPostion));


	m_pBow->LastUpdate_GameObject(fTimeDelta);

	Engine::CGameObject::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void CPlayer::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom || nullptr == m_pMaterialCom)
		return;

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
		return;

	m_pMaterialCom->SetUp_OnGraphicDev();

#ifdef _DEBUG
	m_pColliderCom->Render_Buffer();
#endif 

	m_pBufferCom->Play_Animation(m_fTimeDelta);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();


	SetUp_OnConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(1);

	m_pBufferCom->Render_Mesh(pEffect, "g_BaseTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CPlayer::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Material
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(e_CurType, L"Component_Material");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Material", pComponent));
	m_pMaterialCom->AddRef();

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Mesh_Player");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(e_CurType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom/*&_vec3(0.f, 0.f, 0.f)*/);
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Picking
	pComponent = m_pPickingCom = (Engine::CPicking*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Picking");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Picking", pComponent));
	m_pPickingCom->AddRef();

	//For.Navigation
//	pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(e_CurType, L"Component_Navigation_Santa");
	pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(e_CurType, L"Component_Navigation");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigtion", pComponent));
	m_pNavigationCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(e_CurType, L"Component_Shader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

void CPlayer::Release_Component()
{
	Engine::Safe_Release(m_pMaterialCom);
	Delete_Component(L"Com_Material");
	Engine::Safe_Release(m_pTransformCom);
	Delete_Component(L"Com_Transform");
	Engine::Safe_Release(m_pPickingCom);
	Delete_Component(L"Com_Picking");
	Engine::Safe_Release(m_pBufferCom);
	Delete_Component(L"Com_Buffer");
	Engine::Safe_Release(m_pShaderCom);
	Delete_Component(L"Com_Shader");
	Engine::Safe_Release(m_pColliderCom);
	Delete_Component(L"Com_Collider");
	Engine::Safe_Release(m_pRendererCom);
	Delete_Component(L"Com_Renderer");
	Engine::Safe_Release(m_pNavigationCom);
	Delete_Component(L"Com_Navigtion");

	Ready_Component();
}


void CPlayer::Move_OnTerrain()
{
	if (nullptr == m_pTransformCom)
		return;

	if (nullptr == m_pNavigationCom)
		return;

	if (nullptr == m_pTargetBufferCom)
	{
		m_pTargetBufferCom = (CVIBuffer*)Engine::CObject_Manager::GetInstance()->Get_Component(e_CurType, L"Layer_BackGround", 0, L"Com_Buffer");
		m_pTargetBufferCom->AddRef();
	}
	if (e_CurType == SCENE_STAGE)
		m_pTransformCom->Move_OnBuffer(m_pTargetBufferCom);
	else
		m_pTransformCom->Move_OnNavigation(m_pNavigationCom);
}

void CPlayer::Key_Input(const _float & fTimeDelta)
{
	if(m_bTurnLeft)
		m_pTransformCom->Rotation_Y(1.0f);
	else if(m_bTurnRight)
		m_pTransformCom->Rotation_Y(-1.0f);

	if (true == m_bMotion)
		return;

	if (!m_bMoveJump && m_bBuild)
	{
		if (m_pInput_Device->Get_DIKeyState(DIK_W) & 0x80 && m_bBuild)
		{
			if (!m_bJump)
				m_eNewState = STATE_FORWARD;
			if (m_pInput_Device->Get_DIKeyState(DIK_C) & 0x80)
			{
				_vec3 vNext, vLook;
				vLook = m_pTransformCom->Get_Look();
				m_pTransformCom->Get_NextMove(&vNext, &(vLook * 6.f));
				m_eNewState = STATE_JUMP;
				m_bJump = true;
				m_bMoveJump = true;
				m_bJumpMove = true;
				m_fPosY = m_pTransformCom->Get_PositionY();
				m_vJumpNextPos = vNext;
				m_vJumpDir = vNext - m_pTransformCom->Get_Position();
			}
			else if (m_pInput_Device->Get_DIKeyState(DIK_LSHIFT) & 0x80)
			{
				if (!m_pColliderCom->Get_IsColl())
				{
					if (m_bFootWalk)
					{
						m_bFootWalk = false;
						CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 2.f);
					}
					_vec3 vNext, vLook;
					vLook = m_pTransformCom->Get_Look();
					m_pTransformCom->Get_NextMove(&vNext, &vLook);
					if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&vNext))
						m_pTransformCom->Go_Straight(8.0f, fTimeDelta);
					else if (CNavigation::MOVE_OUT == m_pNavigationCom->Check_Navigation(&vNext))
					{
						_vec3 vSlide;
						m_pTransformCom->Get_SlidingVec(&vSlide, 3.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
						if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&vSlide))
							m_pTransformCom->Sliding(3.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
					}
				}
				else
				{
					_vec3 vNormal, vLook;
					if (m_bFootWalk)
					{
						m_bFootWalk = false;
						CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 1.f);
					}
					D3DXVec3Normalize(&vNormal, m_pColliderCom->Get_CollNormal());
					D3DXVec3Normalize(&vLook, &(-m_pTransformCom->Get_Look()));

					if (D3DXVec3Dot(&vNormal, &(vLook)) > 0)
						m_pTransformCom->Sliding(2.0f, fTimeDelta, *m_pColliderCom->Get_CollNormal());
					else
						m_pTransformCom->Go_Straight(5.0f, fTimeDelta);
				}
			}
			else
			{
				if (!m_pColliderCom->Get_IsColl())
				{
					if (m_bFootWalk)
					{
						m_bFootWalk = false;
						CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 2.f);
					}
					_vec3 vNext, vLook;
					vLook = m_pTransformCom->Get_Look();
					m_pTransformCom->Get_NextMove(&vNext, &(vLook));
					if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&vNext))
						m_pTransformCom->Go_Straight(5.0f, fTimeDelta);
					else if (CNavigation::MOVE_OUT == m_pNavigationCom->Check_Navigation(&vNext))
					{
						_vec3 vSlide;
						m_pTransformCom->Get_SlidingVec(&vSlide, 3.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
						if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_SlideNavigation(&vSlide))
							m_pTransformCom->Sliding(3.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
					}
				}
				else
				{
					if (m_bFootWalk)
					{
						m_bFootWalk = false;
						CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 1.f);
					}
					_vec3 vNormal, vLook;
					D3DXVec3Normalize(&vNormal, m_pColliderCom->Get_CollNormal());
					D3DXVec3Normalize(&vLook, &(-m_pTransformCom->Get_Look()));

					if (D3DXVec3Dot(&vNormal, &(vLook)) > 0)
						m_pTransformCom->Sliding(2.0f, fTimeDelta, *m_pColliderCom->Get_CollNormal());
					else
						m_pTransformCom->Go_Straight(5.0f, fTimeDelta);
				}
			}
		}
		else if (m_pInput_Device->Get_DIKeyState(DIK_S) & 0x80 && m_bBuild)
		{
			if (m_bFootWalk)
			{
				m_bFootWalk = false;
				CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 2.f);
			}
			if (!m_bJump)
				m_eNewState = STATE_BACKWORD;
			if (!m_pColliderCom->Get_IsColl())
			{
				_vec3 vNext, vLook;
				vLook = m_pTransformCom->Get_Look();
				m_pTransformCom->Get_NextMove(&vNext, &(-vLook));
				if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&vNext))
					m_pTransformCom->Go_Straight(-5.0f, fTimeDelta);
				else if (CNavigation::MOVE_OUT == m_pNavigationCom->Check_Navigation(&vNext))
				{
					_vec3 vSlide;
					m_pTransformCom->Get_SlidingVec(&vSlide, -2.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
					if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&vSlide))
						m_pTransformCom->Sliding(-2.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
				}
			}
			else
			{
				if (m_bFootWalk)
				{
					m_bFootWalk = false;
					CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 1.f);
				}
				_vec3 vNormal, vLook;
				D3DXVec3Normalize(&vNormal, m_pColliderCom->Get_CollNormal());
				D3DXVec3Normalize(&vLook, &(m_pTransformCom->Get_Look()));

				if (D3DXVec3Dot(&vNormal, &(vLook)) > 0)
					m_pTransformCom->Sliding(-2.0f, fTimeDelta, *m_pColliderCom->Get_CollNormal());
				else
					m_pTransformCom->Go_Straight(-5.0f, fTimeDelta);

			}
		}
		else if (m_pInput_Device->Get_DIKeyState(DIK_A) & 0x80 && m_bBuild)
		{
			if (m_bFootWalk)
			{
				m_bFootWalk = false;
				CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 2.f);
			}
			if (!m_bJump)
				m_eNewState = STATE_LEFT;
			if (!m_pColliderCom->Get_IsColl())
			{
				_vec3 vPlayerGo = m_pTransformCom->Get_Position();
				_vec3	vDirection;
				vPlayerGo += *D3DXVec3Normalize(&vDirection, &(-m_pTransformCom->Get_Right())) * 0.5f/* * -2.f * fTimeDelta*/;
				if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&vPlayerGo))
					m_pTransformCom->Go_Right(-5.0f, fTimeDelta);
				else if (CNavigation::MOVE_OUT == m_pNavigationCom->Check_Navigation(&vPlayerGo))
				{
					_vec3 vSlide;
					m_pTransformCom->Get_SlidingVec(&vSlide, -2.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
					if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&vSlide))
						m_pTransformCom->Sliding(-2.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
				}
			}
			else
			{
				if (m_bFootWalk)
				{
					m_bFootWalk = false;
					CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 1.f);
				}
				_vec3 vNormal, vLook;
				D3DXVec3Normalize(&vNormal, m_pColliderCom->Get_CollNormal());
				D3DXVec3Normalize(&vLook, &(m_pTransformCom->Get_Right()));

				if (D3DXVec3Dot(&vNormal, &(vLook)) > 0)
					m_pTransformCom->Sliding(-2.0f, fTimeDelta, *m_pColliderCom->Get_CollNormal());
				else
					m_pTransformCom->Go_Right(-5.0f, fTimeDelta);
			}
		}
		else if (m_pInput_Device->Get_DIKeyState(DIK_D) & 0x80 && m_bBuild)
		{
			if (m_bFootWalk)
			{
				m_bFootWalk = false;
				CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 2.f);
			}
			if (!m_bJump)
				m_eNewState = STATE_RIGHT;
			if (!m_pColliderCom->Get_IsColl())
			{
				_vec3 vPlayerGo = m_pTransformCom->Get_Position();
				_vec3	vDirection;
				vPlayerGo += *D3DXVec3Normalize(&vDirection, &m_pTransformCom->Get_Right()) * 0.5f/* * 2.f * fTimeDelta*/;
				if (CNavigation::MOVE_IN == m_pNavigationCom->Check_Navigation(&vPlayerGo))
					m_pTransformCom->Go_Right(5.0f, fTimeDelta);
				else if (CNavigation::MOVE_OUT == m_pNavigationCom->Check_Navigation(&vPlayerGo))
				{
					_vec3 vSlide;
					m_pTransformCom->Get_SlidingVec(&vSlide, 2.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
					if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&vSlide))
						m_pTransformCom->Sliding(2.0f, fTimeDelta, *m_pNavigationCom->Get_CollNormal());
				}
			}
			else
			{
				if (m_bFootWalk)
				{
					m_bFootWalk = false;
					CSound_Manager::GetInstance()->Play_Sound(L"footsteps-footstep-snow-boot-2.mp3", CSound_Manager::Channel_FootWalk, 1.f);
				}
				_vec3 vNormal, vLook;
				D3DXVec3Normalize(&vNormal, m_pColliderCom->Get_CollNormal());
				D3DXVec3Normalize(&vLook, &(-m_pTransformCom->Get_Right()));

				if (D3DXVec3Dot(&vNormal, &(vLook)) > 0)
					m_pTransformCom->Sliding(2.0f, fTimeDelta, *m_pColliderCom->Get_CollNormal());
				else
					m_pTransformCom->Go_Right(5.0f, fTimeDelta);
			}
		}
		else if (m_pInput_Device->Get_DIKeyState(DIK_6) & 0x80)
		{
			CLayer* pLayer1 = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_LeftDoor");
			auto& iter1 = pLayer1->GetObjList()->front();
			dynamic_cast<CDoor*>(iter1)->Set_OpenDoor();
			dynamic_cast<CDoor*>(iter1)->Set_OpenDegree(&_vec3(0.f,10.f,0.f));

			CLayer* pLayer2 = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_RightDoor");
			auto& iter2 = pLayer2->GetObjList()->front();
			dynamic_cast<CDoor*>(iter2)->Set_OpenDoor();
			dynamic_cast<CDoor*>(iter2)->Set_OpenDegree(&_vec3(0.f, -10.f, 0.f));
		}
		else if (m_pInput_Device->Get_DIKeyState(DIK_5) & 0x80)
		{
			m_eNewState = STATE_RELOAD;
			//cout << "Reloaing!!" << endl;
			m_fAttackCount = 6;
		}
		//else if (m_pInput_Device->Get_DIKeyState(DIK_7) & 0x80)
		//{
		//	CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Tree");
		//	auto& iter = pLayer->GetObjList()->begin();
		//	auto& iter_end = pLayer->GetObjList()->end();
		//	for (; iter != iter_end; ++iter)
		//	{
		//		dynamic_cast<CTree*>((*iter))->Set_Draw(false);
		//	}
		//}
		//else if (m_pInput_Device->Get_DIKeyState(DIK_8) & 0x80)
		//{
		//	CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Tree");
		//	auto& iter = pLayer->GetObjList()->begin();
		//	auto& iter_end = pLayer->GetObjList()->end();
		//	for (; iter != iter_end; ++iter)
		//	{
		//		dynamic_cast<CTree*>((*iter))->Set_Draw(true);
		//	}
		//}
		else if (m_pInput_Device->Get_DIKeyState(DIK_0) & 0x80)
		{
			if (!m_bStaticCamera)
			{
				CCamera_Manager::GetInstance()->Change_Camera(L"Dynamic_Camera");
				m_bStaticCamera = true;
			}
			else
			{
				CCamera_Manager::GetInstance()->Change_Camera(L"Static_Camera");
				m_bStaticCamera = false;
			}
		}
		else if (m_pInput_Device->Get_DIKeyState(DIK_C) & 0x80 && m_bBuild)
		{
			m_eNewState = STATE_JUMP;
			m_bJump = true;
			m_bMoveJump = true;
			m_fPosY = m_pTransformCom->Get_PositionY();
		}
		else
		{
			if (!m_bLowHp)
				m_eNewState = STATE_STAND;
			else
				m_eNewState = STATE_LOWIDLE;
		}

		if (m_bAttack && !m_bNotWeapon)
		{
			if (m_pInput_Device->Get_DIMouseState(m_pInput_Device->DIM_LBUTTON) & 0x80)
			{
				--m_fAttackCount;
				m_eNewState = STATE_ATTACK;
				m_bAttack = false;
				//cout << "Bullet : " << m_fAttackCount << endl;

				if (0 <= m_fAttackCount)
				{
					if (nullptr != m_pBow)
					{
						CSound_Manager::GetInstance()->Play_Sound(L"DarkElf_Arrow1.ogg", CSound_Manager::Channel_Player, 2.f);
						_vec3 vPos = *dynamic_cast<CCrossBow*>(m_pBow)->Get_Position();
						_vec3 vLook = m_pTransformCom->Get_Look();
						_vec3 vRot = m_pTransformCom->Get_Rotation();
						_int iIdx = _int(9 - m_fAttackCount);
						CGameObject* pObject = CArrow::Create(m_pGraphicDev, &vPos, &vLook, &vRot, e_CurType);
						if (nullptr == pObject)
							return;
						CObject_Manager::GetInstance()->Ready_GameObject(e_CurType, L"Layer_Arrow", pObject);
					}
				}
			}
		}

	}

	if (!m_bMoveJump)
	{
		if (m_pInput_Device->Get_DIKeyState(DIK_1) & 0x80)
		{
			if (KeyCheck6)
			{
				KeyCheck6 = false;
				if (m_bBuildLighting)
				{
					m_eNewState = STATE_SUMMON;
					CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
					CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
					dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_BULID);
					m_bBuild = false;
					m_bBuildLighting = false;
					m_fMouseMoveSpeed = 11.f;
					m_iCheckIdx = 0;
					_vec3 vPos = m_pTransformCom->Get_Position();
					_vec3 vLook = m_pTransformCom->Get_Look();
					vPos += *D3DXVec3Normalize(&vLook, &vLook);
					CGameObject* pObject = CLightningTower::Create(m_pGraphicDev, &vPos, e_CurType);
					if (nullptr == pObject)
						return;
					CObject_Manager::GetInstance()->Ready_GameObject(e_CurType, L"Layer_LightingAgoTower", pObject);
				}
				else
				{
					CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
					dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_REBULID);
					CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
					m_bBuild = true;
					m_bBuildLighting = true;
					m_fMouseMoveSpeed = 7.f;
					if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_LightingAgoTower"))
					{
						auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_LightingAgoTower")->GetObjList()->back();
						if (dynamic_cast<CLightningTower*>(iter)->Get_NotBuild())
							dynamic_cast<CLightningTower*>(iter)->Set_IsDead();
					}
				}

				CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_BackGround");
				auto& iter = pLayer->GetObjList()->front();
				if (SCENE_STAGE == e_CurType)
					dynamic_cast<CTerrain*>(iter)->Set_TowerRadius();
				else
					dynamic_cast<CBossTerrain*>(iter)->Set_TowerRadius();


			}
		}
		else if (m_pInput_Device->Get_DIKeyState(DIK_2) & 0x80)
		{
			if (KeyCheck6)
			{
				KeyCheck6 = false;
				if (m_bBuildStrike)
				{
					m_eNewState = STATE_SUMMON;
					CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
					CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
					dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_BULID);
					m_bBuild = false;
					m_bBuildStrike = false;
					m_fMouseMoveSpeed = 11.f;
					m_iCheckIdx = 1;
					_vec3 vPos = m_pTransformCom->Get_Position();
					_vec3 vLook = m_pTransformCom->Get_Look();
					vPos += *D3DXVec3Normalize(&vLook, &vLook);
					CGameObject* pObject = CDragonKiller::Create(m_pGraphicDev, &vPos, e_CurType);
					if (nullptr == pObject)
						return;
					CObject_Manager::GetInstance()->Ready_GameObject(e_CurType, L"Layer_StrikeAgoTower", pObject);
				}
				else
				{
					CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
					dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_REBULID);
					CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
					m_bBuild = true;
					m_bBuildStrike = true;
					m_fMouseMoveSpeed = 7.f;
					if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_StrikeAgoTower"))
					{
						auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_StrikeAgoTower")->GetObjList()->back();
						if (dynamic_cast<CDragonKiller*>(iter)->Get_NotBuild())
							dynamic_cast<CDragonKiller*>(iter)->Set_IsDead();
					}
				}

				CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_BackGround");
				auto& iter = pLayer->GetObjList()->front();
				if(SCENE_STAGE == e_CurType)
					dynamic_cast<CTerrain*>(iter)->Set_TowerRadius();
				else
					dynamic_cast<CBossTerrain*>(iter)->Set_TowerRadius();
			}
		}
		else if (m_pInput_Device->Get_DIKeyState(DIK_3) & 0x80)
		{
			if (KeyCheck6)
			{
				KeyCheck6 = false;
				if (m_bBuildMisile)
				{
					m_eNewState = STATE_SUMMON;
					m_iCheckIdx = 2;
					CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
					CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
					dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_BULID);
					m_bBuild = false;
					m_bBuildMisile = false;
					m_fMouseMoveSpeed = 11.f;

					_vec3 vPos = m_pTransformCom->Get_Position();
					_vec3 vLook = m_pTransformCom->Get_Look();
					vPos += *D3DXVec3Normalize(&vLook, &vLook);
					CGameObject* pObject = CMisileTower::Create(m_pGraphicDev, &vPos, e_CurType);
					if (nullptr == pObject)
						return;
					CObject_Manager::GetInstance()->Ready_GameObject(e_CurType, L"Layer_MisileAgoTower", pObject);
				}
				else
				{
					CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
					dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_REBULID);
					CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
					m_bBuild = true;
					m_bBuildMisile = true;
					m_fMouseMoveSpeed = 7.f;
					CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_MisileAgoTower");
					if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_MisileAgoTower"))
					{
						auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_MisileAgoTower")->GetObjList()->back();
						if (dynamic_cast<CMisileTower*>(iter)->Get_NotBuild())
							dynamic_cast<CMisileTower*>(iter)->Set_IsDead();

					}
				}

				CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_BackGround");
				auto& iter = pLayer->GetObjList()->front();
				if (SCENE_STAGE == e_CurType)
					dynamic_cast<CTerrain*>(iter)->Set_TowerRadius();
				else
					dynamic_cast<CBossTerrain*>(iter)->Set_TowerRadius();

			}
		}
		else if (m_pInput_Device->Get_DIKeyState(DIK_4) & 0x80)
		{
			if (KeyCheck6)
			{
				KeyCheck6 = false;
				if (m_bBuildShilder)
				{
					m_eNewState = STATE_SUMMON;
					m_iCheckIdx = 3;
					CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
					CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
					dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_BULID);
					m_bBuild = false;
					m_bBuildShilder = false;
					m_fMouseMoveSpeed = 11.f;

					_vec3 vPos = m_pTransformCom->Get_Position();
					_vec3 vLook = m_pTransformCom->Get_Look();
					vPos += *D3DXVec3Normalize(&vLook, &vLook);
					CGameObject* pObject = CShield::Create(m_pGraphicDev, &vPos, e_CurType);
					if (nullptr == pObject)
						return;
					CObject_Manager::GetInstance()->Ready_GameObject(e_CurType, L"Layer_ShieldAgoTower", pObject);
				}
				else
				{
					CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
					CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
					dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_REBULID);
					m_bBuild = true;
					m_bBuildShilder = true;
					m_fMouseMoveSpeed = 7.f;
					if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_ShieldAgoTower"))
					{
						auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_ShieldAgoTower")->GetObjList()->back();
						if (dynamic_cast<CShield*>(iter)->Get_NotBuild())
							dynamic_cast<CShield*>(iter)->Set_IsDead();
					}
				}

				CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_BackGround");
				auto& iter = pLayer->GetObjList()->front();
				if (SCENE_STAGE == e_CurType)
					dynamic_cast<CTerrain*>(iter)->Set_TowerRadius();
				else
					dynamic_cast<CBossTerrain*>(iter)->Set_TowerRadius();

			}
		}

		if (m_pInput_Device->Get_DIKeyState(DIK_SPACE) & 0x80)
		{
			if (!m_bBuildLighting)
			{
				CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
				dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_REBULID);
				CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
				m_bBuild = true;
				m_bBuildLighting = true;
				m_fMouseMoveSpeed = 7.f;
				if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_LightingAgoTower"))
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_LightingAgoTower")->GetObjList()->back();
					if (dynamic_cast<CLightningTower*>(iter)->Get_NotBuild())
						dynamic_cast<CLightningTower*>(iter)->Set_IsDead();
				}

				_vec3 vPos = m_pTransformCom->Get_Position();
				_vec3 vLook = m_pTransformCom->Get_Look();
				vPos += *D3DXVec3Normalize(&vLook, &vLook);
				CGameObject* pObject = CLightningTower::Create(m_pGraphicDev, &vPos, e_CurType);
				if (nullptr == pObject)
					return;
				CObject_Manager::GetInstance()->Ready_GameObject(e_CurType, L"Layer_LightingTower", pObject);
				if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_LightingTower"))
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_LightingTower")->GetObjList()->back();
					dynamic_cast<CLightningTower*>(iter)->Set_CompleteBuild(false);
				}

				CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_BackGround");
				auto& iter = pLayer->GetObjList()->front();
				if (SCENE_STAGE == e_CurType)
					dynamic_cast<CTerrain*>(iter)->Set_TowerRadius();
				else
					dynamic_cast<CBossTerrain*>(iter)->Set_TowerRadius();

			}
			else if (!m_bBuildStrike)
			{
				CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
				dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_REBULID);
				CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
				m_bBuild = true;
				m_bBuildStrike = true;
				m_fMouseMoveSpeed = 7.f;
				if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_StrikeAgoTower"))
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_StrikeAgoTower")->GetObjList()->back();
					if (dynamic_cast<CDragonKiller*>(iter)->Get_NotBuild())
						dynamic_cast<CDragonKiller*>(iter)->Set_IsDead();
				}

				_vec3 vPos = m_pTransformCom->Get_Position();
				_vec3 vLook = m_pTransformCom->Get_Look();
				vPos += *D3DXVec3Normalize(&vLook, &vLook);
				CGameObject* pObject = CDragonKiller::Create(m_pGraphicDev, &vPos, e_CurType);
				if (nullptr == pObject)
					return;
				CObject_Manager::GetInstance()->Ready_GameObject(e_CurType, L"Layer_StrikeTower", pObject);
				if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_StrikeTower"))
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_StrikeTower")->GetObjList()->back();
					dynamic_cast<CDragonKiller*>(iter)->Set_CompleteBuild(false);
					dynamic_cast<CDragonKiller*>(iter)->Set_Dgree(&m_pTransformCom->Get_Rotation());
				}

				CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_BackGround");
				auto& iter = pLayer->GetObjList()->front();
				if (SCENE_STAGE == e_CurType)
					dynamic_cast<CTerrain*>(iter)->Set_TowerRadius();
				else
					dynamic_cast<CBossTerrain*>(iter)->Set_TowerRadius();

			}
			else if (!m_bBuildMisile)
			{
				CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
				dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_REBULID);
				CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
				m_bBuild = true;
				m_bBuildMisile = true;
				m_fMouseMoveSpeed = 7.f;
				if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_MisileAgoTower"))
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_MisileAgoTower")->GetObjList()->back();
					if (dynamic_cast<CMisileTower*>(iter)->Get_NotBuild())
						dynamic_cast<CMisileTower*>(iter)->Set_IsDead();
				}

				_vec3 vPos = m_pTransformCom->Get_Position();
				_vec3 vLook = m_pTransformCom->Get_Look();
				vPos += *D3DXVec3Normalize(&vLook, &vLook);
				CGameObject* pObject = CMisileTower::Create(m_pGraphicDev, &vPos, e_CurType);
				if (nullptr == pObject)
					return;
				CObject_Manager::GetInstance()->Ready_GameObject(e_CurType, L"Layer_MisileTower", pObject);
				if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_MisileTower"))
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_MisileTower")->GetObjList()->back();
					dynamic_cast<CMisileTower*>(iter)->Set_CompleteBuild(false);
				}

				CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_BackGround");
				auto& iter = pLayer->GetObjList()->front();
				if (SCENE_STAGE == e_CurType)
					dynamic_cast<CTerrain*>(iter)->Set_TowerRadius();
				else
					dynamic_cast<CBossTerrain*>(iter)->Set_TowerRadius();

			}
			else if (!m_bBuildShilder)
			{
				CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
				dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_REBULID);
				CSound_Manager::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", CSound_Manager::Channel_Summon, 2.f);
				m_bBuild = true;
				m_bBuildShilder = true;
				m_fMouseMoveSpeed = 7.f;
				if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_ShieldAgoTower"))
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_ShieldAgoTower")->GetObjList()->back();
					if (dynamic_cast<CShield*>(iter)->Get_NotBuild())
						dynamic_cast<CShield*>(iter)->Set_IsDead();
				}

				_vec3 vPos = m_pTransformCom->Get_Position();
				_vec3 vLook = m_pTransformCom->Get_Look();
				vPos += *D3DXVec3Normalize(&vLook, &vLook);
				CGameObject* pObject = CShield::Create(m_pGraphicDev, &vPos, e_CurType);
				if (nullptr == pObject)
					return;
				CObject_Manager::GetInstance()->Ready_GameObject(e_CurType, L"Layer_Shield", pObject);
				if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_Shield"))
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_Shield")->GetObjList()->back();
					dynamic_cast<CShield*>(iter)->Set_CompleteBuild(false);
					dynamic_cast<CShield*>(iter)->Set_Dgree(&m_pTransformCom->Get_Rotation());
				}

				CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_BackGround");
				auto& iter = pLayer->GetObjList()->front();
				if (SCENE_STAGE == e_CurType)
					dynamic_cast<CTerrain*>(iter)->Set_TowerRadius();
				else
					dynamic_cast<CBossTerrain*>(iter)->Set_TowerRadius();

			}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
		}
		if (!KeyCheck6)
		{
			m_fCheck6 += fTimeDelta * 1.f;
			if (m_fCheck6 > 1.5f)
			{
				KeyCheck6 = true;
				m_fCheck6 = 0.f;
			}
		}
	}


	// Monster
	if (m_pInput_Device->Get_DIKeyState(DIK_I) & 0x80)
	{
		if (m_bKeyTest)
		{
			m_bKeyTest = false;
			auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Boss")->GetObjList()->front();
			dynamic_cast<CBoss*>(iter)->Set_HpMinus();
		}
	}
	else if (m_pInput_Device->Get_DIKeyState(DIK_L) & 0x80)
	{
		if (m_bKeyTest)
		{
			m_bKeyTest = false;
			auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Boss")->GetObjList()->front();
			if(dynamic_cast<CBoss*>(iter)->Get_BuildPage())
				dynamic_cast<CBoss*>(iter)->Set_BuildPage(false);
			else
				dynamic_cast<CBoss*>(iter)->Set_BuildPage(true);
		}
	}
	//else if (m_pInput_Device->Get_DIKeyState(DIK_Y) & 0x80)
	//{
	//	if (m_pInput_Device->Get_DIKeyState(DIK_M) & 0x80)
	//	{
	//		auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Snowman")->GetObjList()->front();
	//		dynamic_cast<SnowmanBoss*>(iter)->Set_MinusHp();
	//	}
	//	else
	//	{
	//		if (m_bKeyTest)
	//		{
	//			m_bKeyTest = false;
	//			auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Snowman")->GetObjList()->front();
	//			if (dynamic_cast<SnowmanBoss*>(iter)->Get_View())
	//				dynamic_cast<SnowmanBoss*>(iter)->Set_View(false);
	//			else
	//				dynamic_cast<SnowmanBoss*>(iter)->Set_View(true);
	//		}
	//	}
	//}
	////else if (m_pInput_Device->Get_DIKeyState(DIK_T) & 0x80)
	////{
	//	//if (m_pInput_Device->Get_DIKeyState(DIK_M) & 0x80)
	//	//{
	//	//	auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Goblin")->GetObjList()->front();
	//	//	//dynamic_cast<CGoblin*>(iter)->Set_MinusHp();
	//	//}
	//	//else
	//	//{
	//	//	if (m_bKeyTest)
	//	//	{
	//	//		m_bKeyTest = false;
	//	//		auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Goblin")->GetObjList()->front();
	//	//		if (dynamic_cast<CGoblin*>(iter)->Get_View())
	//	//			dynamic_cast<CGoblin*>(iter)->Set_View(false);
	//	//		else
	//	//			dynamic_cast<CGoblin*>(iter)->Set_View(true);
	//	//	}
	//	//}	//else if (m_pInput_Device->Get_DIKeyState(DIK_K) & 0x80)
	////{
	////	if (m_pInput_Device->Get_DIKeyState(DIK_M) & 0x80)
	////	{
	////		auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Archer")->GetObjList()->front();
	////		//dynamic_cast<CDarkElfArcher*>(iter)->Set_MinusHp();
	////	}
	////	else
	////	{
	////		if (m_bKeyTest)
	////		{
	////			m_bKeyTest = false;
	////			auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Archer")->GetObjList()->front();
	////			if (dynamic_cast<CDarkElfArcher*>(iter)->Get_View())
	////				dynamic_cast<CDarkElfArcher*>(iter)->Set_View(false);
	////			else
	////				dynamic_cast<CDarkElfArcher*>(iter)->Set_View(true);
	////		}
	////	}
	////}
	//else if (m_pInput_Device->Get_DIKeyState(DIK_L) & 0x80)
	//{
	//	if (m_pInput_Device->Get_DIKeyState(DIK_M) & 0x80)
	//	{
	//		auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Kobold")->GetObjList()->front();
	//		dynamic_cast<CKobold*>(iter)->Set_MinusHp();
	//	}
	//	else
	//	{
	//		if (m_bKeyTest)
	//		{
	//			m_bKeyTest = false;
	//			auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Kobold")->GetObjList()->front();
	//			if (dynamic_cast<CKobold*>(iter)->Get_View())
	//				dynamic_cast<CKobold*>(iter)->Set_View(false);
	//			else
	//				dynamic_cast<CKobold*>(iter)->Set_View(true);
	//		}
	//	}
	//}
	//else if (m_pInput_Device->Get_DIKeyState(DIK_H) & 0x80)
	//{
	//	if (m_pInput_Device->Get_DIKeyState(DIK_M) & 0x80)
	//	{
	//		auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Orge")->GetObjList()->front();
	//		dynamic_cast<COrcBruiser*>(iter)->Set_MinusHp();
	//	}
	//	else
	//	{
	//		if (m_bKeyTest)
	//		{
	//			m_bKeyTest = false;
	//			auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Orge")->GetObjList()->front();
	//			if (dynamic_cast<COrcBruiser*>(iter)->Get_View())
	//				dynamic_cast<COrcBruiser*>(iter)->Set_View(false);
	//			else
	//				dynamic_cast<COrcBruiser*>(iter)->Set_View(true);
	//		}
	//	}
	//}

	if (!m_bKeyTest)
	{
		m_fKeyTest += fTimeDelta * 1.f;
		if (m_fKeyTest > 1.f)
		{
			m_bKeyTest = true;
			m_fKeyTest = 0.f;
		}
	}

	if (!m_bFootWalk)
	{
		m_fFootWalk += fTimeDelta;
		if (m_fFootWalk > 0.46f)
		{
			m_fFootWalk = 0.f;
			m_bFootWalk = true;
		}
	}

	CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();

	if (!m_bStaticCamera)
	{
		if (true == dynamic_cast<CCamera_Static*>(pCamera)->Get_Build())
		{
			const _long& lMoveX = m_pInput_Device->Get_DIMouseMove(CInput_Device::DIMM_X);
			const _long& lMoveY = m_pInput_Device->Get_DIMouseMove(CInput_Device::DIMM_Y);

			if (lMoveX)
			{
				_float f = lMoveX * m_fMouseMoveSpeed * fTimeDelta;
				m_pTransformCom->Rotation_Y(f);
			}
			if (lMoveY)
			{
				_float f = lMoveY * m_fMouseMoveSpeed * fTimeDelta;
			}
		}
	}
	else
	{
		const _long& lMoveX = m_pInput_Device->Get_DIMouseMove(CInput_Device::DIMM_X);
		const _long& lMoveY = m_pInput_Device->Get_DIMouseMove(CInput_Device::DIMM_Y);

		if (lMoveX)
		{
			_float f = lMoveX * m_fMouseMoveSpeed * fTimeDelta;
			m_pTransformCom->Rotation_Y(f);
		}
	}
}

//if (true == m_bTurnLeft)
//m_bTurnLeft = false;
//
//if (true == m_bTurnRight)
//m_bTurnRight = false;
//
//if (STATE_JUMP == m_eCurrState)
//{
//	m_fAniTime = 1.f;
//	m_eNewState = STATE_JUMPFALL;
//}


void CPlayer::State_Update()
{
	if (STATE_STAND != m_eCurrState && STATE_FORWARD != m_eCurrState && STATE_BACKWORD != m_eCurrState && 
		STATE_LEFT != m_eCurrState && STATE_RIGHT != m_eCurrState && STATE_LOWIDLE != m_eCurrState && 
		STATE_TURN != m_eCurrState && STATE_JUMPFALL != m_eCurrState && STATE_SUMMON != m_eCurrState)
	{
		if (!m_pBufferCom->End_Animation(m_fDelayTime))
			return;
		else
		{
			if (!m_bLowHp)
				m_eNewState = STATE_STAND;
			else
				m_eNewState = STATE_LOWIDLE;

			if (STATE_RELOAD == m_eCurrState)
			{
				m_fAttackCount = m_fAttackMaxCount;
				m_bAttack = false;
				m_bNotWeapon = false;
			}

			if (STATE_HURT == m_eCurrState)
			{
				m_bFailBuild = true;
				m_bHit = false;
			}

			if (true == m_bMotion)
				m_bMotion = false;
		}
	}

	if (m_eNewState != m_eCurrState)
	{
		switch (m_eNewState)
		{
		case STATE_STAND:
			m_pBufferCom->Set_AnimationSet(22);
			break;
		case STATE_FORWARD:
			m_pBufferCom->Set_AnimationSet(12);
			break;
		case STATE_BACKWORD:
			m_pBufferCom->Set_AnimationSet(13);
			break;
		case STATE_LEFT:
			m_pBufferCom->Set_AnimationSet(11);
			break;
		case STATE_RIGHT:
			m_pBufferCom->Set_AnimationSet(10);
			break;
		case STATE_SPAWN:
			m_fDelayTime = 0.8f;
			m_bMotion = true;
			m_pBufferCom->Set_AnimationSet(9);
			break;
		case STATE_ATTACK:
			m_pBufferCom->Set_AnimationSet(25);
			m_bMotion = true;
			m_fDelayTime = 0.3f;
			break;
		case STATE_JUMP:
			m_pBufferCom->Set_AnimationSet(20);
			m_fDelayTime = 0.4f;
			m_fAniTime = 0.01f;
			m_bJump = true;
			break;
		case STATE_JUMPFALL:
			m_pBufferCom->Set_AnimationSet(19);
			m_fDelayTime = 0.4f;
			break;
		case STATE_WIN:
			m_pBufferCom->Set_AnimationSet(1);
			m_bMotion = true;
			m_fDelayTime = 0.3f;
			break;
		case STATE_TURN:
			m_pBufferCom->Set_AnimationSet(4);
			m_fDelayTime = 0.2;
			break;
		case STATE_DEATH:
			m_pBufferCom->Set_AnimationSet(27);
			m_fDelayTime = 0.22;
			m_bMotion = true;
			break;
		case STATE_HURT:
			m_pBufferCom->Set_AnimationSet(23);
			m_fDelayTime = 0.25;
			m_bMotion = true;
			break;
		case STATE_RELOAD:
			m_pBufferCom->Set_AnimationSet(15);
			m_fDelayTime = 0.24;
			m_bMotion = true;
			break;
		case STATE_SUMMON:
			m_pBufferCom->Set_AnimationSet(8);
			break;
		case STATE_REPAIR:
			m_pBufferCom->Set_AnimationSet(14);
			m_bMotion = true;
			break;
		case STATE_LOWIDLE:
			m_pBufferCom->Set_AnimationSet(21);
			break;
		}

		m_eCurrState = m_eNewState;
	}
}

void CPlayer::Jump(const _float& fTimeDelta)
{
	if (!m_bJumpMove)   // 제자리 점프
	{
		float height = (m_fJumpTime * m_fJumpTime * (-9.8f) / 2) + (m_fJumpTime * m_fJumpPower);
		//m_bMotion = false;

		m_fJumpFall = height;

		m_pTransformCom->Set_PositionY(m_fPosY + height);
		//점프시간을 증가시킨다.
		m_fJumpTime += fTimeDelta;
		//처음의 높이 보다 더 내려 갔을때 => 점프전 상태로 복귀한다.
		if (m_fPosY + height < m_fPosY)
		{
			m_bJump = false;
			m_bMoveJump = false;
			m_fJumpTime = 0.0f;
			m_fJumpFall = 0.f;
			m_pTransformCom->Set_PositionY(m_fPosY);
			m_eNewState = STATE_STAND;
		}
	}
	else    // 움직이면서 점프
	{
		float height = (m_fJumpTime * m_fJumpTime * (-9.8f) / 2) + (m_fJumpTime * (m_fJumpPower - 1.f));
		m_pTransformCom->Set_PositionY(m_fPosY + height);
		//점프시간을 증가시킨다.
		m_fJumpTime += fTimeDelta;
		//처음의 높이 보다 더 내려 갔을때 => 점프전 상태로 복귀한다.
		if (!m_pColliderCom->Get_IsColl())
		{
			if(CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&m_pTransformCom->Get_Position()))
				m_pTransformCom->Move_ToDirection(m_vJumpDir, 4.f, fTimeDelta);
		}

		_float fCurY = 0.f;

		m_pTransformCom->Get_CurPositionY(m_pNavigationCom, &fCurY);

		if (m_fPosY + height < -fCurY)
		{
			m_bJump = false;
			m_bMoveJump = false;
			m_bJumpMove = false;
			m_fJumpTime = 0.0f;
			m_pTransformCom->Set_PositionY(-fCurY);
			m_eNewState = STATE_STAND;
			m_fJumpFall = 0.f;
			m_vJumpDir = _vec3(0.f, 0.f, 0.f);
		}
	}
}

void CPlayer::Attack(const _float & fTimeDelta)
{
	if (m_fAttackCount == 0)
	{
		m_bAttack = true;
		m_bNotWeapon = true;
	}

	if (!m_bAttack)
	{
		m_fAttackTime += fTimeDelta * 1.f;
		if (m_fAttackTime > 1.3f)
		{
			m_bAttack = true;
			m_fAttackTime = 0.f;
		}
	}
}

void CPlayer::Hit(const _float & fTimeDelta)
{
	m_eNewState = STATE_HURT;

	if(m_eCurrState != STATE_HURT)
		m_fPosY = m_pTransformCom->Get_PositionY();

	_float fPower;

	if (SMALL_HIT == m_eCurHit)
	{
		m_pTransformCom->Go_Straight(-2.f, fTimeDelta);
		m_fPosY = m_pTransformCom->Get_PositionY();
		fPower = 1.f;
	}
	else if (SMASH_HIT == m_eCurHit)
	{
		m_pTransformCom->Go_Straight(-4.f, fTimeDelta);
		m_fPosY = m_pTransformCom->Get_PositionY();
		fPower = 2.f;
	}

	//float height = (m_fJumpTime * m_fJumpTime * (-9.8f) / 2) + (m_fJumpTime * (fPower));
	//m_pTransformCom->Set_PositionY(m_fPosY + height);
	//점프시간을 증가시킨다.
	//m_fJumpTime += fTimeDelta;
	//처음의 높이 보다 더 내려 갔을때 => 점프전 상태로 복귀한다.
	if (!m_pColliderCom->Get_IsColl())
	{
		if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&m_pTransformCom->Get_Position()))
			m_pTransformCom->Move_ToDirection(m_vJumpDir, 4.f, fTimeDelta);
	}

	if (SCENE_STAGE == e_CurType)
		m_pTransformCom->Move_OnBuffer(m_pTargetBufferCom);
	else
		m_pTransformCom->Move_OnNavigation(m_pNavigationCom);

	if (m_bFailBuild)
	{
		m_bFailBuild = false;
		if (!m_bBuild)
		{
			m_bBuild = true;
			CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();
			dynamic_cast<CCamera_Static*>(pCamera)->Set_CameraCurState(dynamic_cast<CCamera_Static*>(pCamera)->STATE_REBULID);
			m_fMouseMoveSpeed = 7.f;
			if (nullptr != CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_AgoTower"))
			{
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_AgoTower")->GetObjList()->back();
				if (dynamic_cast<CLightningTower*>(iter)->Get_NotBuild())
					dynamic_cast<CLightningTower*>(iter)->Set_IsDead();
			}
			CLayer* pLayer = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_BackGround");
			auto& iter = pLayer->GetObjList()->front();
			dynamic_cast<CTerrain*>(iter)->Set_TowerRadius();
		}
	}
}

void CPlayer::CollTime(const _float & fTimeDelta)
{
	if (m_bHitCol)
	{
		m_fHitColTime += 1.f * fTimeDelta;
		if (m_fHitColTime > 5.f)
		{
			m_bHitCol = false;
			m_fHitColTime = 0.f;
		}
	}

}

HRESULT CPlayer::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	// For.LightInfo
	D3DLIGHT9			LightInfo;
	m_pGraphicDev->GetLight(0, &LightInfo);

	pEffect->SetVector("g_vLight_Direction", &_vec4(LightInfo.Direction, 0.f));
	pEffect->SetVector("g_vLight_Diffuse", (_vec4*)&LightInfo.Diffuse);
	pEffect->SetVector("g_vLight_Ambient", (_vec4*)&LightInfo.Ambient);
	pEffect->SetVector("g_vLight_Specular", (_vec4*)&LightInfo.Specular);

	// For.MaterialInfo

	pEffect->SetVector("g_vMaterial_Diffuse", &_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->SetVector("g_vMaterial_Ambient", &_vec4(0.3f, 0.3f, 0.3f, 1.f));
	pEffect->SetVector("g_vMaterial_Specular", &_vec4(1.f, 1.f, 1.f, 1.f));

	// For.Camera	
	D3DXMatrixInverse(&matView, nullptr, &matView);

	pEffect->SetVector("g_vCamera_Position", (_vec4*)&matView.m[3][0]);
	pEffect->SetFloat("g_fPower", 20.0f);

	Safe_Release(pEffect);

	return NOERROR;
}

void CPlayer::Collision_ToObject()
{
	if (SCENE_STAGE == e_CurType)
	{
		auto& iter = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_Object")->GetObjList()->begin();
		auto& iter_end = CObject_Manager::GetInstance()->Find_Layer(e_CurType, L"Layer_Object")->GetObjList()->end();
		for (; iter != iter_end; ++iter)
		{
			if (true == m_pColliderCom->Collision_AABB(dynamic_cast<CBaseObj*>(*iter)->Get_CollCom()))
			{
				m_pColliderCom->Set_Coll(true);
				break;
			}
			m_pColliderCom->Set_Coll(false);
		}
	}
}

void CPlayer::Set_IsColl(_bool Coll)
{
	m_pColliderCom->Set_Coll(Coll);
}

void CPlayer::Set_Position(_vec3 * vPos)
{
	m_pTransformCom->Set_Position(*vPos);
}

void CPlayer::Set_Scale(_vec3 * vPos)
{
	m_pTransformCom->Scaling(*vPos);
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer *	pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CPlayer Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CPlayer::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pPickingCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTargetBufferCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::Safe_Release(m_pNavigationCom);

	Engine::Safe_Release(m_pInput_Device);//Jang

	Engine::Safe_Release(m_pBow);


	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
