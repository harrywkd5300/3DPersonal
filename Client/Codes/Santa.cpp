#include "stdafx.h"
#include "..\Headers\Santa.h"

#include "Component_Manager.h"
#include "object_Manager.h"
#include "Layer.h"

#include "Sound_Manager.h"

#include "cell.h"
#include "Item.h"
#include <time.h>

CSanta::CSanta(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pRendererCom(nullptr), m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr), m_pPlayerTrans(nullptr), m_pShaderCom(nullptr), m_pMaterialCom(nullptr)
	, m_fTimeDelta(0.f), m_fAlpha(1.f), m_iPass(1)
{

}

HRESULT CSanta::Ready_GameObject()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	_vec3 vPosA = *m_pNavigation->Get_Cell(0)->Get_Point(CCell::POINT_A);
	_vec3 vPosB = *m_pNavigation->Get_Cell(0)->Get_Point(CCell::POINT_B);
	_vec3 vPosC = *m_pNavigation->Get_Cell(0)->Get_Point(CCell::POINT_C);

	_vec3 vPos = _vec3((vPosA.x + vPosB.x + vPosC.x) / 3.f,
		(vPosA.y + vPosB.y + vPosC.y) / 3.f,
		(vPosA.z + vPosB.z + vPosC.z) / 3.f);

	// Stage
	m_pTransformCom->Set_Position(vPos);

	m_pTransformCom->Scaling(_vec3(0.005f, 0.005f, 0.005f));

	m_pBufferCom->Set_AnimationSet(0);

	m_eNewState = SANTA_IDLE;

	return NOERROR;
}

_int CSanta::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	if (!m_bBattleTime)
		return 0;

	srand(time(NULL));

	m_fTimeDelta = fTimeDelta;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	Collision_ToObject();

	Move_Partern(fTimeDelta);

	CollTime(fTimeDelta);

	State_Update();

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CSanta::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void CSanta::Render_GameObject(void)
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

	pEffect->BeginPass(m_iPass);

	m_pBufferCom->Render_Mesh(pEffect, "g_BaseTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CSanta::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Material
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Material");
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
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Mesh_Santa");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Navigation
	pComponent = m_pNavigation = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Navigation_Santa");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigation", pComponent));
	m_pNavigation->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Shader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

HRESULT CSanta::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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

	// For.Alpha
	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	Safe_Release(pEffect);

	return NOERROR;
}

void CSanta::State_Update()
{
	if (SANTA_FORWARD != m_eCurrState && SANTA_TURNLEFT != m_eCurrState && SANTA_IDLE != m_eCurrState)
	{
		if (!m_pBufferCom->End_Animation(m_fDelayTime))
			return;
		else
		{
			m_eNewState = SANTA_IDLE;

			if (true == m_bMotion)
			{
				m_bMotion = false;
				m_fDelayTime = 0.f;
			}

			if (SANTA_DROPITEM == m_eCurrState)
			{
				m_bDropItem = false;
				m_bEndItem = true;
			}
			
			if (SANTA_WINWAVE == m_eCurrState)
			{
				m_bPutItem = true;
				m_eNewState = SANTA_DROPITEM;
				CSound_Manager::GetInstance()->Play_Sound(L"voice-prompts-male-voice-male-ho-ho-ho-merry-christmas-santa-occasions-speaking-human-voice.mp3", CSound_Manager::Channel_Santa, 2.f);
			}

			/*if (SANTA_SPAWN == m_eCurrState)
				m_pTelepoter = false;*/
		}
	}

	if (m_eNewState != m_eCurrState)
	{
		// Spawn 0
		switch (m_eNewState)
		{
		case SANTA_IDLE:
			m_pBufferCom->Set_AnimationSet(4);
			//m_fDelayTime = 2.f;
			break;
		case SANTA_FORWARD:
			m_pBufferCom->Set_AnimationSet(2);
			break;
		case SANTA_DEATH:
			m_pBufferCom->Set_AnimationSet(6);
			//m_fDelayTime = 0.2f;
			m_bMotion = true;
			break;
		case SANTA_HURT:
			m_pBufferCom->Set_AnimationSet(5);
			m_bMotion = true;
			break;
		case SANTA_TURNLEFT:
			m_pBufferCom->Set_AnimationSet(8);
			break;
		case SANTA_DROPITEM:
			m_pBufferCom->Set_AnimationSet(3);
			m_bMotion = true;
			break;
		case SANTA_WINWAVE:
			m_pBufferCom->Set_AnimationSet(1);
			m_bMotion = true;
			break;
		case SANTA_SPAWN:
			m_pBufferCom->Set_AnimationSet(0);
			m_bMotion = true;
			break;
		case SANTA_CALLOUT:
			m_pBufferCom->Set_AnimationSet(7);
			m_bMotion = true;
			break;
		}
		m_eCurrState = m_eNewState;
	}
}

void CSanta::Move_Partern(const _float & fTimeDelta)
{
	//if (m_fSuper > 5.f)
	//{
	//	if (m_bPutItem)
	//	{
	//		m_fPutTime += fTimeDelta * 1.f;
	//		if (m_fPutTime > 0.4f)
	//		{
	//			m_fPutTime = 0.f;
	//			m_eNewState = SANTA_CALLOUT;
	//			if (nullptr == CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Item"))
	//			{
	//				_vec3 vPos = m_pTransformCom->Get_Position();
	//				_vec3 vLook = m_pTransformCom->Get_Look();
	//				CGameObject* pObject = CItem::Create(m_pGraphicDev, &vPos, &vLook);
	//				if (nullptr == pObject)
	//					return;
	//				CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_Item", pObject);
	//				m_bPutItem = false;
	//				m_bEndItem = false;
	//				return;
	//			}
	//			else
	//			{
	//				_uint iSize = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Item")->GetObjList()->size();
	//				_uint iRd = rand() % iSize - 1;
	//				if (iSize > 15)
	//				{
	//					_vec3 vPosA = *m_pNavigation->Get_Cell(iRd)->Get_Point(CCell::POINT_A);
	//					_vec3 vPosB = *m_pNavigation->Get_Cell(iRd)->Get_Point(CCell::POINT_B);
	//					_vec3 vPosC = *m_pNavigation->Get_Cell(iRd)->Get_Point(CCell::POINT_C);
	//					_vec3 m_vTelPos = _vec3((vPosA.x + vPosB.x + vPosC.x) / 3.f,
	//						(vPosA.y + vPosB.y + vPosC.y) / 3.f,
	//						(vPosA.z + vPosB.z + vPosC.z) / 3.f);
	//					//m_pTransformCom->Set_Position(m_vTelPos);
	//					_vec3 vPos = m_pTransformCom->Get_Position();
	//					_vec3 vLook = m_pTransformCom->Get_Look();
	//					CGameObject* pObject = CItem::Create(m_pGraphicDev, &m_vTelPos, &vLook);
	//					if (nullptr == pObject)
	//						return;
	//					CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_Item", pObject);
	//					m_bPutItem = false;
	//					return;
	//				}
	//			}
	//		}
	//}

	if (m_pTelepoter && !m_bDropItem)
	{
		_uint iMax = m_pNavigation->Get_VecCell()->size();

		m_pTelepoter = false;

		_uint iIdx = rand() % iMax;
		//_uint iIdx = 0;

		m_pNavigation->Set_Currrent_Index(iIdx);

		_vec3 vPosA = *m_pNavigation->Get_Cell(iIdx)->Get_Point(CCell::POINT_A);
		_vec3 vPosB = *m_pNavigation->Get_Cell(iIdx)->Get_Point(CCell::POINT_B);
		_vec3 vPosC = *m_pNavigation->Get_Cell(iIdx)->Get_Point(CCell::POINT_C);

		_vec3 m_vTelPos = _vec3((vPosA.x + vPosB.x + vPosC.x) / 3.f,
			(vPosA.y + vPosB.y + vPosC.y) / 3.f,
			(vPosA.z + vPosB.z + vPosC.z) / 3.f);

		m_pTransformCom->Set_Position(m_vTelPos);

		m_eNewState = SANTA_SPAWN;

		m_bMoveBool = true;

		m_bDropItem = false;

		//_vec3(48.f, 0.f, 64.f)
		return;
	}
	else if (m_bDropItem && !m_pTelepoter)
	{
		m_eNewState = SANTA_WINWAVE;

		if (m_bPutItem)
		{
			m_fPutTime += fTimeDelta * 1.f;
			if (m_fPutTime > 0.4f)
			{
				m_fPutTime = 0.f;
				if (nullptr == CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Item"))
				{
					_vec3 vPos = m_pTransformCom->Get_Position();
					_vec3 vLook = m_pTransformCom->Get_Look();

					CGameObject* pObject = CItem::Create(m_pGraphicDev, &vPos, &vLook);
					if (nullptr == pObject)
						return;
					CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_Item", pObject);
					m_bPutItem = false;
					m_bEndItem = false;
					return;
				}
				else
				{
					_uint iSize = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Item")->GetObjList()->size();
					if (iSize < 5)
					{
						_vec3 vPos = m_pTransformCom->Get_Position();
						_vec3 vLook = m_pTransformCom->Get_Look();

						CGameObject* pObject = CItem::Create(m_pGraphicDev, &vPos, &vLook);
						if (nullptr == pObject)
							return;
						CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_Item", pObject);
						m_bPutItem = false;
						m_bEndItem = false;
					}
					return;
				}
			}
		}
		else
		{
			return;
		}
	}

	if (m_bMoveBool && m_bEndItem)
	{
		m_bMoveBool = false;

		_uint iSize = m_pNavigation->Get_VecCell()->size();

		_vec3 vIdx = _vec3(0.f, 0.f, 0.f);

		m_pNavigation->Get_NeghbirIndex(&vIdx);

		_float iDX = 0;

		if (vIdx.x < 0)
		{
			if (vIdx.y < 0)
				iDX = vIdx.z;
			else if(vIdx.z < 0)
				iDX = vIdx.y;
			else
			{
				_uint iRand = rand() % 10;
				if(0 == iRand || 4 == iRand || 7 == iRand || 9 == iRand || 2 == iRand)
					iDX = vIdx.z;
				else
					iDX = vIdx.y;
			}
		}
		else if (vIdx.y < 0)
		{
			if (vIdx.x < 0)
				iDX = vIdx.z;
			else if (vIdx.z < 0)
				iDX = vIdx.x;
			else
			{
				_uint iRand = rand() % 10;
				if (0 == iRand || 4 == iRand || 7 == iRand || 9 == iRand || 2 == iRand)
					iDX = vIdx.z;
				else
					iDX = vIdx.x;
			}
		}
		else if (vIdx.z < 0)
		{
			if (vIdx.x < 0)
				iDX = vIdx.y;
			else if (vIdx.y < 0)
				iDX = vIdx.x;
			else
			{
				_uint iRand = rand() % 10;
				if (0 == iRand || 4 == iRand || 7 == iRand || 9 == iRand || 2 == iRand)
					iDX = vIdx.y;
				else
					iDX = vIdx.x;
			}
		}
		else
		{
			_uint iRand = rand() % 20;
			if (0 == iRand || 1 == iRand || 7 == iRand || 11 == iRand || 19 == iRand)
				iDX = vIdx.y;
			else if (5 == iRand || 9 == iRand || 14 == iRand || 18 == iRand || 10 == iRand)
				iDX = vIdx.x;
			else
				iDX = vIdx.z;
		}

		if (iDX < 0)
			return;

		_vec3 vPosA = *m_pNavigation->Get_Cell(iDX)->Get_Point(CCell::POINT_A);
		_vec3 vPosB = *m_pNavigation->Get_Cell(iDX)->Get_Point(CCell::POINT_B);
		_vec3 vPosC = *m_pNavigation->Get_Cell(iDX)->Get_Point(CCell::POINT_C);

		m_vEndPos = _vec3((vPosA.x + vPosB.x + vPosC.x) / 3.f,
						  (vPosA.y + vPosB.y + vPosC.y) / 3.f,
			              (vPosA.z + vPosB.z + vPosC.z) / 3.f);

		m_vDir = m_vEndPos - m_pTransformCom->Get_Position();
	}
	else
	{
		_vec3 vMyLook = m_pTransformCom->Get_Look();

		_float fDgree = 0.f;

		_float fDist = m_pTransformCom->Compute_Distance(&m_vEndPos);

		m_pTransformCom->Get_LookDgreeY(fDgree, &m_vDir, &vMyLook);

		if (abs(fDgree) > 5.f)
		{
			m_pTransformCom->Rotation_Y(fDgree * fTimeDelta * 2.f);
			m_eNewState = SANTA_TURNLEFT;
		}
		else
		{
			if (m_pColliderCom->Get_IsColl())
				m_bMoveBool = true;

			if (fDist > 0.5f)
			{
				_vec3 vNext, vLook;
				vLook = m_pTransformCom->Get_Look();
				m_pTransformCom->Get_NextMove(&vNext, &vLook);
				m_pNavigation->Check_Navigation(&vNext);
				m_pTransformCom->Move_ToDirection(m_vDir, 3.f, fTimeDelta);
				m_eNewState = SANTA_FORWARD;
			}
			else
				m_bMoveBool = true;
		}
	}

}

void CSanta::CollTime(const _float & fTimeDelta)
{
	if (!m_bDropItem)
	{
		m_fDropItem += fTimeDelta * 1.f;
		if (m_fDropItem > 8.f)
		{
			m_fDropItem = 0.f;
			m_bDropItem = true;
		}
	}

	if (!m_pTelepoter)
	{
		m_fTelepoter += fTimeDelta * 1.f;
		if (m_fTelepoter > 10.f)
		{
			m_fTelepoter = 0.f;
			m_pTelepoter = true;
		}
	}
}

void CSanta::Collision_ToObject()
{
	if (nullptr == CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Item"))
		return;

	_uint iSize = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Item")->GetObjList()->size();

	for (size_t i = 0; i < iSize; ++i)
	{
		CComponent*		pCollider_Item = CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_Item", i, L"Com_Collider");
		if (nullptr == pCollider_Item)
			return;

		if (true == m_pColliderCom->Collision_AABB((CCollider*)pCollider_Item))
			return;
	}
}

CSanta * CSanta::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSanta*	pInstance = new CSanta(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CSanta Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CSanta::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pPlayerTrans);
	Engine::Safe_Release(m_pNavigation);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
