
// Tool76View.cpp : CTool76View 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool76.h"
#endif

#include "Tool76Doc.h"
#include "Tool76View.h"
#include "MyForm.h"

#include "MainFrm.h"
#include "Input_Device.h"
#include "timer_Manager.h"
#include "frame_manager.h"
#include "Graphic_Device.h"

#include "layer.h"
#include "object_Manager.h"

#include "ToolScene.h"
#include "ToolTerrain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTool76View

IMPLEMENT_DYNCREATE(CTool76View, CView)

BEGIN_MESSAGE_MAP(CTool76View, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CTool76View 생성/소멸

CTool76View::CTool76View()
	: m_pGraphicDev(nullptr)
	, m_pRenderer(nullptr)
	, m_pDevice(CGraphic_Device::GetInstance())
	, m_pManagement(CManagement::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, pTimer_Manager(CTimer_Manager::GetInstance())
	, pFrame_Manager(CFrame_Manager::GetInstance())
	, m_iSelectCellIdx(-1)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_pDevice->AddRef();
	m_pManagement->AddRef();
	m_pComponent_Manager->AddRef();
	m_pInput_Device->AddRef();
	pTimer_Manager->AddRef();
	pFrame_Manager->AddRef();
		
}

CTool76View::~CTool76View()
{
	Free();
}

BOOL CTool76View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CTool76View 그리기

void CTool76View::OnDraw(CDC* /*pDC*/)
{
	CTool76Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CTool76View 인쇄

BOOL CTool76View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CTool76View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CTool76View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CTool76View 진단

#ifdef _DEBUG
void CTool76View::AssertValid() const
{
	CView::AssertValid();
}

void CTool76View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

_ulong CTool76View::Free()
{
	_ulong dwRefCnt = 0;
	/*Decrease RefCount*/
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDevice);
	Safe_Release(m_pManagement);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pInput_Device);
	Safe_Release(pTimer_Manager);
	Safe_Release(pFrame_Manager);


	/*Destroy Instance*/
	if (dwRefCnt = CManagement::GetInstance()->DestroyInstance())
		AfxMessageBox(L"Management not destroyed");
	if (dwRefCnt = CObject_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CObject_Manager not destroyed");
	if (dwRefCnt = CComponent_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"Component_Manager not destroyed");
	if (dwRefCnt = CGraphic_Device::GetInstance()->DestroyInstance())
		AfxMessageBox(L"Device not destroyed");
	if (dwRefCnt = CInput_Device::GetInstance()->DestroyInstance())
		AfxMessageBox(L"Input_Device not destroyed");
	if (dwRefCnt = CTimer_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"Timer_Manager not destroyed");
	if (dwRefCnt = CFrame_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"Frame_Manager not destroyed");
	if (dwRefCnt = CCamera_Manager::GetInstance()->DestroyInstance())
		AfxMessageBox(L"CCamera_Manager Released Failed");

	return dwRefCnt;
}

CTool76Doc* CTool76View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTool76Doc)));
	return (CTool76Doc*)m_pDocument;
}
#endif //_DEBUG


// CTool76View 메시지 처리기

HRESULT CTool76View::Ready_DeviceSetting()
{
	if (nullptr == m_pDevice)
		goto exception;

	if (FAILED(m_pDevice->Ready_GraphicDev(&m_pGraphicDev, g_hWnd, Engine::CGraphic_Device::MODE_WIN, BACKCX, BACKCY)))
		goto exception;

	return NOERROR;

exception:
	AfxMessageBox(L"Ready_DeviceSetting failed err:04934");
	return E_FAIL;
}

HRESULT CTool76View::Ready_StaticComponent(void)
{
	if (nullptr == m_pComponent_Manager)
	{
		AfxMessageBox(L"Component_Manager isn't allocated");
		return E_FAIL;
	}
	/*Create Triangle_Color*/
	CComponent* pComponent = nullptr;

	pComponent = CTriangle_Color::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		goto exception;
	if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, L"Component_Buffer_TriCol", pComponent)))
		goto exception;

	/*create Rect_Color*/
	pComponent = CRect_Color::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		goto exception;
	if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, L"Component_Buffer_RectCol", pComponent)))
		goto exception;

	/*Create Rect_Texture*/
	pComponent = CRect_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		goto exception;
	if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, L"Component_Buffer_RectTex", pComponent)))
		goto exception;

	/*Create Cube_Texture*/
	pComponent = CCube_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		goto exception;
	if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, L"Component_Buffer_Cube", pComponent)))
		goto exception;

	/*create Renderer & reference*/
	pComponent = m_pRenderer = CRenderer::Create(m_pGraphicDev);
	m_pRenderer->AddRef();
	if (nullptr == pComponent)
		goto exception;
	if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, L"Component_Renderer", pComponent)))
		goto exception;
	m_pManagement->SetUp_Renderer(m_pRenderer);

	/*create transform*/
	pComponent = CTransform::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		goto exception;
	if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, L"Component_Transform", pComponent)))
		goto exception;

	// For.Material
	pComponent = Engine::CMaterial::Create(m_pGraphicDev, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f), 10);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Ready_Component(TOOL_STATIC, L"Component_Material", pComponent)))
		return E_FAIL;


	return NOERROR;
exception:
	AfxMessageBox(L"Ready_StaticComponent Failed!!");
	return E_FAIL;
}

HRESULT CTool76View::Ready_StartScene(void)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	CScene* pScene = CToolScene::Create(m_pGraphicDev);

	if (nullptr == pScene)
		return E_FAIL;

	m_pScene = pScene;

	if (FAILED(m_pManagement->SetUp_CurrentScene(pScene)))
		return E_FAIL;

	return NOERROR;
}

void CTool76View::TimeUpdate()
{
	if (nullptr == pTimer_Manager || nullptr == pFrame_Manager || nullptr == m_pInput_Device)
		AfxMessageBox(L"Manager Setting Failed!!");
	pTimer_Manager->Compute_Timer(L"Timer_Default");
	_float fTimeDelta_default = pTimer_Manager->Get_TimeDelta(L"Timer_Default");
	if (pFrame_Manager->Permit_Call(L"Frame_60", fTimeDelta_default))
	{
		m_pInput_Device->Check_InputState();
		pTimer_Manager->Compute_Timer(L"Timer_60");
		fTimeDelta = pTimer_Manager->Get_TimeDelta(L"Timer_60");

		Management_Update(fTimeDelta);
		Management_Render();
	}
}

void CTool76View::Management_Update(_float & fTimeDelta)
{
	m_pManagement->Update_Management(fTimeDelta);
}

void CTool76View::Management_Render()
{
	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(255, 0, 0, 255), 1.f, 0);
	m_pGraphicDev->BeginScene();

	m_pManagement->Render_Management();

	m_pGraphicDev->EndScene();
	m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);
}


void CTool76View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	RECT rcWindow;

	pMainFrame->GetWindowRect(&rcWindow);
	//AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, FALSE);

	SetRect(&rcWindow, 0, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top);

	RECT rcMainView;
	GetClientRect(&rcMainView);

	//가로 프레임의 길이를 구하자.
	float fX = float(rcWindow.right - rcMainView.right);

	//세로 프레임의 길이를 구하자.
	float fY = float(rcWindow.bottom - rcMainView.bottom);

	pMainFrame->SetWindowPos(
		NULL,		//프레임의 크기를 계산한 상태로 출력할 위치를 결정하는 인자.
		0,
		0,
		int(BACKCX + fX),
		int(BACKCY + fY),
		SWP_NOZORDER
	);

	g_hWnd = m_hWnd;

	if (FAILED(Ready_DeviceSetting()))
	{
		AfxMessageBox(L"Device Created Failed!!");
	}
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	//m_pGraphicDev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	if (FAILED(m_pManagement->Ready_Management(TOOL_END)))
		AfxMessageBox(L"Ready_Setting Failed!!");
	if (FAILED(Ready_StaticComponent()))
		AfxMessageBox(L"Ready_Setting Failed!!");
	if (FAILED(Ready_StartScene()))
		AfxMessageBox(L"Ready_Setting Failed!!");
	//
	/* Add Timer */
	//
	CTimer*	pTimer = nullptr;
	pTimer = CTimer::Create();

	if (nullptr == pTimer)
		AfxMessageBox(L"Timer Create Failed!!");

	if (FAILED(pTimer_Manager->Add_Timer(L"Timer_Default", pTimer)))
		AfxMessageBox(L"Timer_Default Setting Failed!!");

	pTimer = CTimer::Create();

	if (nullptr == pTimer)
		AfxMessageBox(L"Timer Create Failed!!");

	if (FAILED(pTimer_Manager->Add_Timer(L"Timer_60", pTimer)))
		AfxMessageBox(L"Timer_60 Setting Failed!!");
	//
	/*Add Frame*/
	//
	if (FAILED(pFrame_Manager->Add_Frame(L"Frame_60", 60.f)))
		AfxMessageBox(L"Frame_60 Setting Failed!!");

	CWnd* pWnd = AfxGetMainWnd();
	HWND hWnd = pWnd->m_hWnd;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	m_pInput_Device->Ready_Input_Device(hInstance, hWnd);
}


void CTool76View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnLButtonDown(nFlags, point);

	CMyForm* pMyForm = ((CMainFrame*)AfxGetMainWnd())->GetMapForm();
	_uint TabNumber = pMyForm->GetTabCtrl()->GetCurSel();

	if (0 == TabNumber)
	{
		if (nullptr == CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
			return;
		

		if (pMyForm->GetTab1()->m_bHeightUsing)
		{
			if (pMyForm->GetTab1()->CheckRadio(0))
			{
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();

				if (m_pInput_Device->Get_DIMouseState(m_pInput_Device->DIM_LBUTTON) & 0x80)
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
					dynamic_cast<CToolTerrain*>(iter)->Plus_Height(1.f, pMyForm->GetTab1()->m_fMaxHeight);
				}
			}
			else if (pMyForm->GetTab1()->CheckRadio(1))
			{
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();

				if (m_pInput_Device->Get_DIMouseState(m_pInput_Device->DIM_LBUTTON) & 0x80)
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
					dynamic_cast<CToolTerrain*>(iter)->Plus_Height(-1.f, pMyForm->GetTab1()->m_fMaxHeight);
				}
			}
			else if (pMyForm->GetTab1()->CheckRadio(2))
			{
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();

				if (m_pInput_Device->Get_DIMouseState(m_pInput_Device->DIM_LBUTTON) & 0x80)
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
					dynamic_cast<CToolTerrain*>(iter)->Plus_Height(0.f, pMyForm->GetTab1()->m_fMaxHeight);
				}
			}
			else if (pMyForm->GetTab1()->CheckRadio(3))
			{
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();

				if (m_pInput_Device->Get_DIMouseState(m_pInput_Device->DIM_LBUTTON) & 0x80)
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
					dynamic_cast<CToolTerrain*>(iter)->Plus_Height(2.f, pMyForm->GetTab1()->m_fMaxHeight);
				}
			}
			else if (pMyForm->GetTab1()->CheckRadio(4))
			{
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();

				if (m_pInput_Device->Get_DIMouseState(m_pInput_Device->DIM_LBUTTON) & 0x80)
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
					dynamic_cast<CToolTerrain*>(iter)->Plus_Height(3.f, pMyForm->GetTab1()->m_fMaxHeight);
				}
			}
			return;
		}

		_vec3 LastPickingPos = _vec3(0.f, 0.f, 0.f);
		dynamic_cast<CToolScene*>(m_pScene)->Terrain_Picking(&LastPickingPos, point.x, point.y);
		// PickingPos == Terrain Buffer의 정점
		if (_vec3(-1.f, -1.f, -1.f) == LastPickingPos)
		{
			AfxMessageBox(L"Retry Clicked Terrain Pos!!");
			return;
		}
		
		LastPickingPos.y = pMyForm->GetTab1()->Get_PointY();
	
		PickingPos = LastPickingPos;
	
		_int iCell =  dynamic_cast<CToolScene*>(m_pScene)->Pick_Navigation(&LastPickingPos);
		if (-1 != iCell)
		{
			m_iSelectCellIdx = iCell;
			return;
		}
		else
		{
			_vec3 vGetPos = _vec3(0.f, 0.f, 0.f);
			if(dynamic_cast<CToolScene*>(m_pScene)->Pick_Dist_Short(&LastPickingPos, vGetPos))
				LastPickingPos = vGetPos;
		}

		pMyForm->GetTab1()->Set_PointXYZ(LastPickingPos.x, LastPickingPos.y, LastPickingPos.z);

		pMyForm->GetTab1()->Set_MakeCount();
		if (dynamic_cast<CToolScene*>(m_pScene)->Make_Navigation(&LastPickingPos))
		{
			m_iSelectCellIdx = dynamic_cast<CToolScene*>(m_pScene)->Get_VecCell()->size() - 1;
			pMyForm->GetTab1()->PlusCount();
		}
	}
	else if (1 == TabNumber)
	{
		_vec3 LastPickingPos = _vec3(0.f, 0.f, 0.f);
		dynamic_cast<CToolScene*>(m_pScene)->Terrain_Picking(&LastPickingPos, point.x, point.y);
		// PickingPos == Terrain Buffer의 정점
		if (_vec3(-1.f, -1.f, -1.f) == LastPickingPos)
		{
			AfxMessageBox(L"Retry Clicked Terrain Pos!!");
			return;
		}
		PickingPos = LastPickingPos;

		CGameObject* pObject = nullptr;
		pObject = dynamic_cast<CToolScene*>(m_pScene)->Pick_Mesh(&PickingPos, point.x, point.y);
		if (nullptr != pObject)
		{
			CComponent* pComponent = pObject->Get_Component(L"Com_Transform");
			_vec3 vData = dynamic_cast<CTransform*>(pComponent)->Get_Position();
			_vec3 vScale = dynamic_cast<CTransform*>(pComponent)->Get_Scale();
			_vec3 vRot = dynamic_cast<CTransform*>(pComponent)->Get_Rotation();
			dynamic_cast<CToolScene*>(m_pScene)->Picking_Object(pObject);
			pMyForm->GetTab2()->Set_TransData(&vData, &vScale, &vRot);
			return;
		}
		pObject = dynamic_cast<CToolScene*>(m_pScene)->Get_SelObject();
		if (nullptr != pObject)
		{
			CComponent* pComponent = pObject->Get_Component(L"Com_Transform");
			_vec3 vData = dynamic_cast<CTransform*>(pComponent)->Get_Position();
			_vec3 vScale = dynamic_cast<CTransform*>(pComponent)->Get_Scale();
			_vec3 vRot = dynamic_cast<CTransform*>(pComponent)->Get_Rotation();
			dynamic_cast<CToolScene*>(m_pScene)->Set_SelObjectTrans(&PickingPos, 0);
			pMyForm->GetTab2()->Set_TransData(&vData, &vScale, &vRot);
			return;
		}

		CTab2* tab2 = ((CMainFrame*)AfxGetMainWnd())->GetMapForm()->GetTab2();
		HTREEITEM hTreeItem = tab2->m_TreeControl.GetSelectedItem();
		CString Item = tab2->m_TreeControl.GetItemText(hTreeItem);

		CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

		list<IMAGE_INFO*>::iterator iter = tab2->Get_ListObj()->begin();
		list<IMAGE_INFO*>::iterator iter_end = tab2->Get_ListObj()->end();


		for (; iter != iter_end ; ++iter)
		{
			if (!lstrcmp(Item, (*iter)->FileName))
			{
				break;
			}
		}
		if (iter == iter_end)
			return;

		_vec3 vScale = _vec3(0.f, 0.f, 0.f);
		CString strValue;
		tab2->GetDlgItemText(IDC_EDIT4, strValue);
		vScale.x = float(_tstof(strValue));
		tab2->GetDlgItemText(IDC_EDIT5, strValue);
		vScale.y = float(_tstof(strValue));
		tab2->GetDlgItemText(IDC_EDIT6, strValue);
		vScale.z = float(_tstof(strValue));
		_bool	Static = true;

		_vec3 vRot = _vec3(0.f, 0.f, 0.f);

		if (tab2->IsDlgButtonChecked(IDC_RADIO5))
			Static = true;
		else if (tab2->IsDlgButtonChecked(IDC_RADIO6))
			Static = false;
		if (FAILED(dynamic_cast<CToolScene*>(pScene)->Add_MeshObject((*iter)->FullPath, (*iter)->FileName, (*iter)->ComName, vScale, PickingPos, vRot, Static)))
		{
			AfxMessageBox(L"Failed Add Mesh Component");
			return;
		}

		pMyForm->GetTab2()->Set_TransData(&PickingPos, &vScale,&vRot);
	}
	else if (2 == TabNumber )
	{
		if (nullptr != CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
		{
			CTab3* tab3 = ((CMainFrame*)AfxGetMainWnd())->GetMapForm()->GetTab3();

			auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
			dynamic_cast<CToolTerrain*>(iter)->Set_ViewBrush(false);
			dynamic_cast<CToolTerrain*>(iter)->Set_MousePoint(point);

			if (m_pInput_Device->Get_DIMouseState(m_pInput_Device->DIM_LBUTTON) & 0x80)
			{
				_uint iIdx = tab3->m_ComboBox.GetCurSel();

				dynamic_cast<CToolTerrain*>(iter)->Set_Spletting(iIdx);
			}
		}
	}
}



void CTool76View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnMouseMove(nFlags, point);
	CMyForm* pMyForm = ((CMainFrame*)AfxGetMainWnd())->GetMapForm();
	_uint TabNumber = pMyForm->GetTabCtrl()->GetCurSel();
	if (2 == TabNumber)
	{
		OnLButtonDown(nFlags, point);
	}
	else if (1 == TabNumber)
	{
		if (nullptr != CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
		{
			auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
			dynamic_cast<CToolTerrain*>(iter)->Set_ViewBrush(true);
		}
	}
	else if (0 == TabNumber)
	{
		if (pMyForm->GetTab1()->m_bHeightUsing)
		{
			OnLButtonDown(nFlags, point);
		}
		if (nullptr != CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
		{
			auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
			dynamic_cast<CToolTerrain*>(iter)->Set_ViewBrush(false);
			dynamic_cast<CToolTerrain*>(iter)->Set_MousePoint(point);
		}
	}
}
