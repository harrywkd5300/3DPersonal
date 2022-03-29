#include "stdafx.h"
#include "ToolScene.h"

#include "Graphic_Device.h"
#include "Layer.h"
#include "ToolCamera.h"
#include "ToolTerrain.h"

#include "StaticMesh.h"
#include "DynamicObject.h"
#include "Navi_Mesh.h"
#include "PickObj.h"


CToolScene::CToolScene(LPDIRECT3DDEVICE9 pGraphicDev)
	:CScene(pGraphicDev)
	, m_fFPS(0.f)
{
}

HRESULT CToolScene::Ready_Scene(void)
{
	if (FAILED(Ready_D3dFont()))
		return E_FAIL;

	if (FAILED(Ready_LightInfo()))
		return E_FAIL;

	if (FAILED(Ready_LogoComponent()))
		return E_FAIL;

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(L"Layer_Camera", &_vec3(0.f, 10.f, -10.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Navigation(L"Layer_Navigation")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Mouse(L"Layer_PickObj")))
		return E_FAIL;

	return NOERROR;
}

_int CToolScene::Update_Scene(const _float & fTimeDelta)
{
	m_fTimer += fTimeDelta;
	++m_fFPS;

	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CToolScene::Render_Scene(void)
{
	//for (size_t i = 0; i < 3; ++i)
	//{
	//	if (_vec3(-1.f, -1.f, -1.f) != m_vNaviPos[i])
	//	{
	//		char					m_szBullet[32] = "";
	//		sprintf_s(m_szBullet, "%d", i);
	//		m_pD3dFont->DrawTextW(BACKCX / 2 + 30 * i , BACKCY - 50, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), m_szBullet);
	//	}
	//}

	//if (m_fTimer > 1.f)
	//{
	//	sprintf_s(m_szFPS, "FPS : %d", _int(m_fFPS));

	//	m_fTimer = 0.f;
	//	m_fFPS = 0.f;
	//}

	//m_pD3dFont->DrawTextW(10, 10, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), m_szFPS);

	Engine::CScene::Render_Scene();
}

HRESULT CToolScene::Ready_D3dFont()
{
	m_pD3dFont = CGraphic_Device::GetInstance()->Get_Font();

	if (nullptr == m_pD3dFont)
	{
		MSG_BOX("Ready D3dFont Failed");
		return E_FAIL;
	}
	m_pD3dFont->AddRef();

	return NOERROR;
}

HRESULT CToolScene::Ready_LogoComponent(void)
{
	// For.Logo_Scene Component
	Engine::CComponent*			pComponent = nullptr;

	/*create Picking*/
	pComponent = m_pPickingCom = CPicking::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, L"Component_Picking", pComponent)))
		return E_FAIL;

	// For.Texture_Terrain
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Brush/Particle_8.png");
	//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Aura0.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Texture_Brush", pComponent)))
		return E_FAIL;

	// For.Texture_Filter
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Filter.bmp");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Texture_Filter", pComponent)))
		return E_FAIL;

	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Buffer_Collider", pComponent)))
		return E_FAIL;

	pComponent = CNavigation::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Navigation", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Terrain
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Terrain.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Shader_Terrain", pComponent)))
		return E_FAIL;

	// for.Component_Frustum
	pComponent = CFrustum::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Frustum", pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CToolScene::Ready_Texture(void)
{
	// For.Logo_Scene Component
	Engine::CComponent*			pComponent = nullptr;

	/* texture */
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Snow-D.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Texture_BackLogo", pComponent)))
		return E_FAIL;

	/* texture */
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowBase.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Texture_SnowBase", pComponent)))
		return E_FAIL;

	/* texture */
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowMix02.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Texture_Rocks", pComponent)))
		return E_FAIL;

	/* texture */
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowMud.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Texture_Multi0", pComponent)))
		return E_FAIL;


	/* texture */
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Floor1_d.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Texture_Floor1_d", pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CToolScene::Ready_LightInfo()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	D3DLIGHT9		LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightInfo.Ambient = LightInfo.Diffuse;
	LightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	m_pGraphicDev->SetLight(0, &LightInfo);
	m_pGraphicDev->LightEnable(0, TRUE);

	//// Á¡±¤¿ø1
	//LightInfo.Type = D3DLIGHT_POINT;
	//LightInfo.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.f);
	//LightInfo.Ambient = LightInfo.Diffuse;
	//LightInfo.Position = _vec3(20.0f, 7.0f, 20.f);
	//LightInfo.Range = 10.f;
	//LightInfo.Attenuation0 = 2.f;

	//m_pGraphicDev->SetLight(1, &LightInfo);
	//m_pGraphicDev->LightEnable(1, TRUE);

	return NOERROR;
}


HRESULT CToolScene::Ready_Layer_Camera(const _tchar * pLayerTag, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Camera
	pGameObject = CToolCamera::Create(m_pGraphicDev, *pEye, *pAt, *pUp);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(TOOL_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CToolScene::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Camera
	pGameObject = CToolTerrain::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(TOOL_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CToolScene::Ready_Layer_Navigation(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Camera
	pGameObject = CNavi_Mesh::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(TOOL_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;

	for (size_t i = 0; i < 3; ++i)
		m_vNaviPos[i] = _vec3(-1.f, -1.f, -1.f);

	return NOERROR;
}

HRESULT CToolScene::Ready_Layer_Mouse(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.PickObj
	pGameObject = CPickObj::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(TOOL_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CToolScene::Add_MsehComponent(const _tchar * pFilePath, const _tchar * pFileName, const _tchar * pComName, bool isStatic)
{
	Engine::CComponent*			pComponent = nullptr;

	if (nullptr != m_pComponent_Manager->Find_Component(TOOL_STATIC, pComName))
		return NOERROR;

	if (isStatic)
	{
		// For.Add_Component
		pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, pFilePath, pFileName);
		if (nullptr == pComponent)
			return E_FAIL;
		if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, pComName, pComponent)))
			return E_FAIL;
	}
	else
	{
		// For.Add_Component
		pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, pFilePath, pFileName, _vec3(0.f, 0.f, 0.f));
		if (nullptr == pComponent)
			return E_FAIL;
		if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, pComName, pComponent)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CToolScene::Add_MeshObject(const _tchar * pFilePath, const _tchar * pFileName, const _tchar * pComName, const _vec3& vMakeScale, const _vec3& vMakePos , const _vec3& vMakeRot, const _bool& vStatic)
{
	CGameObject*	pGameObject = nullptr;

	if (vStatic)
	{
		All_PickingObject(false);
		// For.StaticMesh
		pGameObject = m_pSelObject =  CStaticMesh::Create(m_pGraphicDev, pFilePath, pFileName, pComName, vMakeScale, vMakePos, vMakeRot);
		if (nullptr == pGameObject)
			return E_FAIL;
		if (FAILED(m_pObject_Manager->Ready_GameObject(TOOL_STATIC, L"Layer_Object", pGameObject)))
			return E_FAIL;

		//dynamic_cast<CStaticMesh*>(m_pSelObject)->Set_PickObjectCollider(true);
	}
	else
	{
		// For.DynamicMesh
		//pGameObject = m_pSelObject = CDynamicObject::Create(m_pGraphicDev, pComName, vMakeScale);
		//if (nullptr == pGameObject)
		//	return E_FAIL;
		//if (FAILED(m_pObject_Manager->Ready_GameObject(TOOL_STATIC, L"Layer_Object", pGameObject)))
		//	return E_FAIL;

		//dynamic_cast<CDynamicObject*>(m_pSelObject)->Set_PickObjectCollider(false);
	}

	return NOERROR;
}

HRESULT CToolScene::Add_Terrain(const _uint& iVtxX, const _uint& iVtxZ, const _float& iInterval)
{
	if(nullptr != Engine::CComponent_Manager::GetInstance()->Find_Component(TOOL_STATIC, L"Component_Buffer_Terrain"))
		Engine::CComponent_Manager::GetInstance()->Release_TargetComponent(TOOL_STATIC, L"Component_Buffer_Terrain");

	if (nullptr == Engine::CComponent_Manager::GetInstance()->Find_Component(TOOL_STATIC, L"Component_Buffer_Terrain"))
	{
		CComponent*		pComponent = nullptr;
		// For.Buffer_Terrain
		pComponent = Engine::CTerrain_Buffer::Create(m_pGraphicDev, iVtxX, iVtxZ, iInterval);
		if (nullptr == pComponent)
		{
			MSG_BOX("Don't Create Terrain Buffer!");
			return E_FAIL;
		}
		if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(TOOL_STATIC, L"Component_Buffer_Terrain", pComponent)))
		{
			MSG_BOX("Don't Create Terrain Buffer Ready!!");
			return E_FAIL;
		}
	}

	if (nullptr == m_pObject_Manager->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
	{
		CGameObject*	pGameObject = nullptr;

		// For.Terrain
		pGameObject = CToolTerrain::Create(m_pGraphicDev);
		if (nullptr == pGameObject)
			return E_FAIL;
		if (FAILED(m_pObject_Manager->Ready_GameObject(TOOL_STATIC, L"Layer_Terrain", pGameObject)))
			return E_FAIL;

		MSG_BOX("Created Terrain!!");
		return NOERROR;
	}
	else
	{
		CLayer*	pGameObject = m_pObject_Manager->Find_Layer(TOOL_STATIC, L"Layer_Terrain");
		auto& iter = pGameObject->GetObjList()->begin();
		dynamic_cast<CToolTerrain*>((*iter))->Change_TerrianBuffer();
		return NOERROR;
	}

	return E_FAIL;
}

HRESULT CToolScene::Change_Texture_view(_bool ViewWireFrame)
{
	CLayer*	pGameObject = m_pObject_Manager->Find_Layer(TOOL_STATIC, L"Layer_Terrain");
	auto& iter = pGameObject->GetObjList()->begin();
	dynamic_cast<CToolTerrain*>((*iter))->Set_WireFrame(ViewWireFrame);

	return NOERROR;
}

void CToolScene::Terrain_Picking(_vec3 * vPickPos, _ulong uX, _ulong uY)
{
	CWnd* pWnd = AfxGetMainWnd();
	HWND hWnd = pWnd->m_hWnd;

	CComponent* pComponent = CObject_Manager::GetInstance()->Get_Component(TOOL_STATIC, L"Layer_Terrain", 0, L"Com_Buffer");
	if (nullptr == pComponent)
		return;
	
	*vPickPos = m_pPickingCom->Picking_ToNaviBuffer(hWnd, BACKCX, BACKCY, (CVIBuffer*)pComponent, uX, uY, nullptr);
}

_bool CToolScene::Make_Navigation(_vec3 * vPickPos)
{
	_vec3 vMakcPos[3];
	for (size_t i = 0; i < 3; ++i)
	{
		if (_vec3(-1.f, -1.f, -1.f) == m_vNaviPos[i])
		{
			m_vNaviPos[i] = *vPickPos;
			if (2 == i)
			{
				for (size_t i = 0; i < 3; ++i)
				{
					vMakcPos[i] = m_vNaviPos[i];
					m_vNaviPos[i] = _vec3(-1.f, -1.f, -1.f);
				}
				break;
			}
			return false;
		}
	}

	auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Navigation")->GetObjList()->begin();
	dynamic_cast<CNavi_Mesh*>(*iter)->Make_NaviMesh(&vMakcPos[0], &vMakcPos[1], &vMakcPos[2]);

	return true;
}


void CToolScene::Reserve_Navigation(const _ulong& Size)
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Navigation")->GetObjList()->begin();

	dynamic_cast<CNavi_Mesh*>(*iter)->Reserve_Navigation(Size);
}

vector<CCell*>* CToolScene::Get_VecCell()
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Navigation")->GetObjList()->begin();

	return dynamic_cast<CNavi_Mesh*>(*iter)->Get_VecCell();
}

void CToolScene::All_ClearNavigation()
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Navigation")->GetObjList()->begin();
	dynamic_cast<CNavi_Mesh*>(*iter)->All_Clear();
}

void CToolScene::Sel_ClearNavigation(const _int & iCellIdx)
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Navigation")->GetObjList()->begin();
	dynamic_cast<CNavi_Mesh*>(*iter)->Select_Delete(iCellIdx);
}

void CToolScene::Set_PickObj(const _vec3 & vPos)
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->begin();
	dynamic_cast<CPickObj*>(*iter)->Set_Position(vPos);
}

void CToolScene::Set_POINT(const _int & iIdx, const _vec3 & vPickPos, const _vec3& vChangePos)
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Navigation")->GetObjList()->begin();
	dynamic_cast<CNavi_Mesh*>(*iter)->Set_PoInt(iIdx, vPickPos, vChangePos);
}

void CToolScene::Set_ObjectCollBox(_bool View)
{
	auto iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->begin();
	auto iter_end = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->end();

	for (; iter != iter_end; ++iter)
	{
		dynamic_cast<CStaticMesh*>(*iter)->Set_ColliderBox(View);
	}
}

_uint CToolScene::Pick_Navigation(const _vec3* vPickPos)
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Navigation")->GetObjList()->begin();
	return dynamic_cast<CNavi_Mesh*>(*iter)->Picking_Navigation(vPickPos);
}

_bool CToolScene::Pick_Dist_Short(const _vec3 * vPickPos, _vec3 & vGetPos)
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Navigation")->GetObjList()->begin();
	return dynamic_cast<CNavi_Mesh*>(*iter)->Picking_Dist_Short(vPickPos, vGetPos);
}

void CToolScene::Obj_WireFrame(_bool View)
{
	if (nullptr == CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object"))
		return;

	auto iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->begin();
	auto iter_end = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->end();

	for (; iter != iter_end; ++iter)
	{
		dynamic_cast<CStaticMesh*>(*iter)->Set_Wire(View);
	}
}

CGameObject* CToolScene::Pick_Mesh(const _vec3 * vPickPos, _ulong uX, _ulong uY)
{
	if (nullptr == CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object"))
		return nullptr;

	auto iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->begin();
	auto iter_end = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->end();

	for (; iter != iter_end; ++iter)
	{
		_matrix matWorld;
		matWorld.Identity();
		dynamic_cast<CStaticMesh*>(*iter)->Get_WorldMatrix(&matWorld);
		if (m_pPickingCom->Picking_ToMeshBuffer(g_hWnd, BACKCX, BACKCY, (LPD3DXMESH*)dynamic_cast<CStaticMesh*>(*iter)->Get_Mesh() , uX, uY, &matWorld))
		{
			return (*iter);
		}
	}
	return nullptr;
}

CGameObject * CToolScene::Get_SelObject()
{
	if (nullptr == m_pSelObject)
		return nullptr;

	if (dynamic_cast<CStaticMesh*>(m_pSelObject)->Get_RedCollBox())
		return m_pSelObject;

	return nullptr;
}

void CToolScene::Picking_Object(CGameObject * pObject)
{
	if (nullptr == pObject)
		return;
	All_PickingObject(false);

	if (m_pSelObject == pObject)
	{
		m_pSelObject = nullptr;
		return;
	}
	m_pSelObject = pObject;
	dynamic_cast<CStaticMesh*>(m_pSelObject)->Set_PickObjectCollider(true);
}

void CToolScene::All_PickingObject(_bool Pick)
{
	if (nullptr == CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object"))
		return;

	auto iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->begin();
	auto iter_end = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->end();

	for (; iter != iter_end; ++iter)
	{
		dynamic_cast<CStaticMesh*>(*iter)->Set_PickObjectCollider(Pick);
	}
}

void CToolScene::Set_SelObjectTrans(_vec3 * vChangePos, const _uint& idx)
{
	if (0 == idx)
	{
		CComponent* pComponent = dynamic_cast<CStaticMesh*>(m_pSelObject)->Get_Component(L"Com_Transform");
		dynamic_cast<CTransform*>(pComponent)->Set_Position(*vChangePos);
	}
	else if (1 == idx)
	{
		CComponent* pComponent = dynamic_cast<CStaticMesh*>(m_pSelObject)->Get_Component(L"Com_Transform");
		dynamic_cast<CTransform*>(pComponent)->Scaling(*vChangePos);
	}
	else if (2 == idx)
	{
		CComponent* pComponent = dynamic_cast<CStaticMesh*>(m_pSelObject)->Get_Component(L"Com_Transform");
		dynamic_cast<CTransform*>(pComponent)->Set_Degree(*vChangePos);
	}

	return;
}

void CToolScene::Set_DeleteSelObject()
{
	if (nullptr == CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object"))
	{
		AfxMessageBox(L"Object is Zero!!!");
		return;
	}

	if (nullptr == m_pSelObject)
	{
		AfxMessageBox(L"Don't Select Object!!");
		return;
	}

	auto iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->begin();
	auto iter_end = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter) == m_pSelObject)
		{
			(*iter)->Set_IsDead();
			m_pSelObject = nullptr;
			return;
		}
	}
}

CToolScene * CToolScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CToolScene* pInstance = new CToolScene(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("CToolScene Created Failed!!");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CToolScene::Free(void)
{
	Safe_Release(m_pD3dFont);
	Engine::CScene::Free();

	return 0;
}
