// ../Codes/Tab1.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool76.h"
#include "Tab1.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "Tool76View.h"

#include "Navi_Mesh.h"
#include "Cell.h"

#include "object_Manager.h"
#include "Layer.h"

#include "ToolTerrain.h"

// CTab1 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTab1, CDialog)

CTab1::CTab1(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB1, pParent)
	, m_iCellCount(0)
	, m_iVertexX(0)
	, m_iVertexZ(0)
	, m_fInterval(0.f)
	, m_fPointX(0)
	, m_fPointY(0)
	, m_fPointZ(0)
	, m_iMakeCount(0)
	, m_bHeightUsing(FALSE)
	, m_fBrushSize(5)
	, m_fMaxHeight(5)
{

}

CTab1::~CTab1()
{
	Engine::Safe_Delete_Array(m_pVertexPos);
}

void CTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT7, m_iCellCount);
	DDX_Text(pDX, IDC_EDIT1, m_iVertexX);
	DDX_Text(pDX, IDC_EDIT2, m_iVertexZ);
	DDX_Text(pDX, IDC_EDIT3, m_fInterval);
	DDX_Text(pDX, IDC_EDIT8, m_fPointX);
	DDX_Text(pDX, IDC_EDIT9, m_fPointY);
	DDX_Text(pDX, IDC_EDIT10, m_fPointZ);
	DDX_Text(pDX, IDC_EDIT11, m_iMakeCount);
	DDX_Check(pDX, IDC_CHECK5, m_bHeightUsing);
	DDX_Text(pDX, IDC_EDIT5, m_fBrushSize);
	DDX_Control(pDX, IDC_RADIO1, m_bRadioHeight);
	DDX_Text(pDX, IDC_EDIT6, m_fMaxHeight);
}


BEGIN_MESSAGE_MAP(CTab1, CDialog)
	ON_BN_CLICKED(IDC_BUTTONCREATE, &CTab1::OnBnClickedButtoncreate)
	ON_BN_CLICKED(IDC_BUTTONTEXTURECREATE, &CTab1::OnBnClickedButtontexturecreate)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_CHECK1, &CTab1::OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT3, &CTab1::OnEnChangeEdit3)
	ON_BN_CLICKED(IDC_DELETE, &CTab1::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_CLEAR, &CTab1::OnBnClickedClear)
	ON_BN_CLICKED(IDC_SAVE, &CTab1::OnBnClickedSave)
	ON_BN_CLICKED(IDC_LOAD, &CTab1::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_CHANGE, &CTab1::OnBnClickedChange)
	ON_BN_CLICKED(IDC_CHECK5, &CTab1::OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_CHANGE2, &CTab1::OnBnClickedChange2)
	ON_EN_CHANGE(IDC_EDIT6, &CTab1::OnEnChangeEdit6)
	ON_BN_CLICKED(IDC_SAVE2, &CTab1::OnBnClickedSave2)
	ON_BN_CLICKED(IDC_LOAD2, &CTab1::OnBnClickedLoad2)
END_MESSAGE_MAP()


// CTab1 메시지 처리기입니다.


void CTab1::PlusCount()
{
	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();
	m_iCellCount = dynamic_cast<CToolScene*>(pScene)->Get_VecCell()->size() ;
	SetDlgItemInt(IDC_EDIT7, m_iCellCount);
}

void CTab1::Set_PointXYZ(const float & fX, const float & fY, const float & fZ)
{
	CString strValue;
	strValue.Format(_T("%f"), fX);
	SetDlgItemText(IDC_EDIT8, strValue);

	strValue.Format(_T("%f"), fY);
	SetDlgItemText(IDC_EDIT9, strValue);

	strValue.Format(_T("%f"), fZ);
	SetDlgItemText(IDC_EDIT10, strValue);

}

void CTab1::Set_MakeCount()
{
	++m_iMakeCount;
	if (3 == m_iMakeCount)
		m_iMakeCount = 0;

	SetDlgItemInt(IDC_EDIT11, m_iMakeCount);
}

_float& CTab1::Get_PointY()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT9, strValue);

	m_fPointY = float(_tstof(strValue));

	return m_fPointY;
}

BOOL CTab1::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CheckDlgButton(IDC_CHECK1, FALSE);
	CheckDlgButton(IDC_RADIO1, TRUE);

	m_bHeightUsing = false;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTab1::HorizontalScroll()
{
	CString strName;
	CSize	Size;

	//int iCX = 0;

	//CDC* pDc = m_TextureList.GetDC();
	//for (int i = 0; i < m_TextureList.GetCount(); ++i)
	//{
	//	m_TextureList.GetText(i, strName);
	//	Size = pDc->GetTextExtent(strName);
	//	if (Size.cx > iCX)
	//	{
	//		iCX = Size.cx;
	//	}
	//}
	//m_TextureList.ReleaseDC(pDc);

	//if (m_TextureList.GetHorizontalExtent() < iCX)
	//{
	//	m_TextureList.SetHorizontalExtent(iCX);
	//}
}

wstring CTab1::RelativePath(wstring szFullPath)
{
	TCHAR szRelativePath[MAX_PATH] = L"";
	TCHAR szDirectoryPath[MAX_PATH] = L"";

	GetCurrentDirectory(sizeof(szDirectoryPath), szDirectoryPath);

	PathRelativePathTo(szRelativePath, szDirectoryPath, FILE_ATTRIBUTE_DIRECTORY,
		szFullPath.c_str(), FILE_ATTRIBUTE_NORMAL);

	return szRelativePath;
}


void CTab1::OnBnClickedButtoncreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	_uint  iVertexX  = GetDlgItemInt(IDC_EDIT1);
	_uint  iVertexZ  = GetDlgItemInt(IDC_EDIT2);
	_float iInterval = _float(GetDlgItemInt(IDC_EDIT3));

	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();
	dynamic_cast<CToolScene*>(pScene)->Add_Terrain(iVertexX, iVertexZ, iInterval);
}


void CTab1::OnBnClickedButtontexturecreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strName;

	//int iSelectIndex = m_TextureList.GetCurSel();

	//if (iSelectIndex < 0)
	//	return;

	//m_TextureList.GetText(iSelectIndex, strName);
}


void CTab1::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	UpdateData(TRUE);

	int iFileCount = DragQueryFile(hDropInfo, -1, NULL, 0);

	TCHAR szFullPath[MAX_PATH] = L"";

	for (int i = 0; i < iFileCount; ++i)
	{
		DragQueryFile(hDropInfo, i, szFullPath, MAX_PATH);

		std::wstring wstrRelativePath = RelativePath(szFullPath);

	//	wstrRelativePath = wstrRelativePath.substr(0, wstrRelativePath.size() - 3) + L"png";

		CString strImageName;

		//##[17.07.18_02]
		//파일의 이름만 얻어오는 함수.
		strImageName = PathFindFileName(wstrRelativePath.c_str());

		//확장자를 제거해주는 함수.
		PathRemoveExtension((LPWSTR)strImageName.operator LPCWSTR());

		//m_TextureList.AddString(strImageName);
	}

	HorizontalScroll();

	CDialog::OnDropFiles(hDropInfo);

	UpdateData(FALSE);

}


void CTab1::OnBnClickedCheck1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();
	if (IsDlgButtonChecked(IDC_CHECK1))
	{
		dynamic_cast<CToolScene*>(pScene)->Change_Texture_view(true);
	}
	else
	{
		dynamic_cast<CToolScene*>(pScene)->Change_Texture_view(false);
	}
}


void CTab1::OnEnChangeEdit3()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CTab1::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iCellIdx = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->Get_CellIdx();

	if (-1 == iCellIdx)
		return;

	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

	dynamic_cast<CToolScene*>(pScene)->Sel_ClearNavigation(iCellIdx);

	((CMainFrame*)AfxGetMainWnd())->GetMapView()->Set_CellIIdx(-1);

	PlusCount();
}


void CTab1::OnBnClickedClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();
	dynamic_cast<CToolScene*>(pScene)->All_ClearNavigation();

	PlusCount();

	CString strValue;
	strValue.Format(_T("%f"), 0.f);
	SetDlgItemText(IDC_EDIT8, strValue);

	strValue.Format(_T("%f"), 0.f);
	SetDlgItemText(IDC_EDIT9, strValue);

	strValue.Format(_T("%f"), 0.f);
	SetDlgItemText(IDC_EDIT10, strValue);
}


void CTab1::OnBnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CFileDialog		Dlg(FALSE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE);

	TCHAR szDirectory[MAX_PATH] = L"";		//드라이브경로를 받을 문자열 변수.

	GetCurrentDirectory(MAX_PATH, szDirectory);		//현재 프로젝트의 드라이브경로를 받아온다.
	PathRemoveFileSpec(szDirectory);

	lstrcat(szDirectory, L"\\Test");

	Dlg.GetOFN().lpstrInitialDir = szDirectory;

	if (Dlg.DoModal() == IDOK)
	{
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS
			, FILE_ATTRIBUTE_NORMAL, NULL);


		if (INVALID_HANDLE_VALUE == hFile)
		{
			AfxMessageBox(L"저장실패.");
			return;
		}

		DWORD dwByte = 0;

		CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();
		auto iter = dynamic_cast<CToolScene*>(pScene)->Get_VecCell()->begin();
		auto iter_end = dynamic_cast<CToolScene*>(pScene)->Get_VecCell()->end();

		_ulong		dwNumCell = dynamic_cast<CToolScene*>(pScene)->Get_VecCell()->size();

		WriteFile(hFile, &dwNumCell, sizeof(_ulong), &dwByte, 0);

		for (; iter != iter_end; ++iter)
		{
			WriteFile(hFile, dynamic_cast<CCell*>(*iter)->Get_Point(CCell::POINT_A), sizeof(_vec3), &dwByte, NULL);
			WriteFile(hFile, dynamic_cast<CCell*>(*iter)->Get_Point(CCell::POINT_B), sizeof(_vec3), &dwByte, NULL);
			WriteFile(hFile, dynamic_cast<CCell*>(*iter)->Get_Point(CCell::POINT_C), sizeof(_vec3), &dwByte, NULL);
		}

		AfxMessageBox(L"Save Succesed!!");
		CloseHandle(hFile);
		return;
	}

	
}


void CTab1::OnBnClickedLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (nullptr == CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
	{
		AfxMessageBox(L"Don't Create Terrain!!");
		return;
	}

	UpdateData(TRUE);

	CFileDialog		Dlg(TRUE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE);

	TCHAR szDirectory[MAX_PATH] = L"";		//드라이브경로를 받을 문자열 변수.

	GetCurrentDirectory(MAX_PATH, szDirectory);		//현재 프로젝트의 드라이브경로를 받아온다.
	PathRemoveFileSpec(szDirectory);

	lstrcat(szDirectory, L"\\Test");

	Dlg.GetOFN().lpstrInitialDir = szDirectory;

	if (Dlg.DoModal() == IDOK)
	{
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_READ, NULL, NULL, OPEN_EXISTING
			, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			AfxMessageBox(L"로드실패.");
			return;
		}

		CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

		DWORD dwByte = 0;

		_ulong			dwNumCell = 0;

		ReadFile(hFile, &dwNumCell, sizeof(_ulong), &dwByte, nullptr);

		dynamic_cast<CToolScene*>(pScene)->Reserve_Navigation(dwNumCell);

		while (true)
		{
			if (0 == dwByte)
				break;
			
			_vec3			vPoint;

			ReadFile(hFile, vPoint, sizeof(_vec3), &dwByte, nullptr);
			dynamic_cast<CToolScene*>(pScene)->Make_Navigation(&vPoint);
		}
		AfxMessageBox(L"로드성공!!");
		CloseHandle(hFile);
	}
}


void CTab1::OnBnClickedChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

	_int  iIdx = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->Get_CellIdx();
	_vec3 LastPickPos = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->Get_LastPos();

	_vec3 vChangePos;


	CString strValue;
	GetDlgItemText(IDC_EDIT8, strValue);

	vChangePos.x = float(_tstof(strValue));

	GetDlgItemText(IDC_EDIT9, strValue);

	vChangePos.y = float(_tstof(strValue));

	GetDlgItemText(IDC_EDIT10, strValue);

	vChangePos.z = float(_tstof(strValue));

	dynamic_cast<CToolScene*>(pScene)->Set_POINT(iIdx, LastPickPos, vChangePos);

}


void CTab1::OnBnClickedCheck5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (!m_bHeightUsing)
		m_bHeightUsing = true;
	else
		m_bHeightUsing = false;

}


void CTab1::OnBnClickedChange2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strValue;
	GetDlgItemText(IDC_EDIT5, strValue);
	float fSize = float(_tstof(strValue));

	if (nullptr != CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
	{
		auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
		dynamic_cast<CToolTerrain*>(iter)->Set_BrushSize(fSize);
		return;
	}
}

_bool CTab1::CheckRadio(const _uint & iIdx)
{
	if (0 == iIdx)
	{
		return IsDlgButtonChecked(IDC_RADIO1);
	}
	else if (1 == iIdx)
	{
		return IsDlgButtonChecked(IDC_RADIO2);
	}
	else if (2 == iIdx)
	{
		return IsDlgButtonChecked(IDC_RADIO3);
	}
	else if (3 == iIdx)
	{
		return IsDlgButtonChecked(IDC_RADIO4);
	}
	else if (4 == iIdx)
	{
		return IsDlgButtonChecked(IDC_RADIO5);
	}

	return false;
}



void CTab1::OnEnChangeEdit6()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strValue;
	GetDlgItemText(IDC_EDIT6, strValue);
	m_fMaxHeight = float(_tstof(strValue));
}


void CTab1::OnBnClickedSave2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr != CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
	{
		CFileDialog		Dlg(FALSE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE);

		TCHAR szDirectory[MAX_PATH] = L"";		//드라이브경로를 받을 문자열 변수.

		GetCurrentDirectory(MAX_PATH, szDirectory);		//현재 프로젝트의 드라이브경로를 받아온다.
		PathRemoveFileSpec(szDirectory);

		lstrcat(szDirectory, L"\\Test");

		Dlg.GetOFN().lpstrInitialDir = szDirectory;

		if (Dlg.DoModal() == IDOK)
		{
			HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS
				, FILE_ATTRIBUTE_NORMAL, NULL);


			if (INVALID_HANDLE_VALUE == hFile)
			{
				AfxMessageBox(L"저장실패.");
				return;
			}

			DWORD dwByte = 0;

			auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();

			_vec3	vecTexData = *dynamic_cast<CToolTerrain*>(iter)->Get_TerrainBufferData();

			WriteFile(hFile, &vecTexData, sizeof(_vec3), &dwByte, 0);

			VTXTEXNORMAL*			pVertex = nullptr;

			const _vec3* vPosition = dynamic_cast<CToolTerrain*>(iter)->Get_TerrainData();

			for (size_t i = 0; i < vecTexData.y; ++i)
			{
				for (size_t j = 0; j < vecTexData.x; ++j)
				{
					_uint iIndex = i * _uint(vecTexData.y) + j;

					WriteFile(hFile, &vPosition[iIndex], sizeof(_vec3), &dwByte, NULL);
				}
			}

			AfxMessageBox(L"Save Succesed!!");
			CloseHandle(hFile);
			return;
		}
	}
	else
	{
		AfxMessageBox(L"Don't Create Terrain!!");
		return;
	}
}


void CTab1::OnBnClickedLoad2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (nullptr == CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
	{
		AfxMessageBox(L"Don't Create Terrain!!");
		return;
	}

	UpdateData(TRUE);

	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

	CFileDialog		Dlg(TRUE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE);

	TCHAR szDirectory[MAX_PATH] = L"";		//드라이브경로를 받을 문자열 변수.

	GetCurrentDirectory(MAX_PATH, szDirectory);		//현재 프로젝트의 드라이브경로를 받아온다.
	PathRemoveFileSpec(szDirectory);

	lstrcat(szDirectory, L"\\Test");

	Dlg.GetOFN().lpstrInitialDir = szDirectory;

	if (Dlg.DoModal() == IDOK)
	{
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_READ, NULL, NULL, OPEN_EXISTING
			, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			AfxMessageBox(L"로드실패.");
			return;
		}

		DWORD dwByte = 0;

		_vec3 vecFilter;

		ReadFile(hFile, &vecFilter, sizeof(_vec3), &dwByte, nullptr);

		m_pVertexPos = new _vec3[vecFilter.x * vecFilter.y];

		for (size_t i = 0; i < vecFilter.y; ++i)
		{
			for (size_t j = 0; j < vecFilter.x; ++j)
			{
				_uint iIndex = i * _uint(vecFilter.y) + j;

				ReadFile(hFile, &m_pVertexPos[iIndex], sizeof(_vec3), &dwByte, nullptr);
			}
		}

		auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();

		dynamic_cast<CToolTerrain*>(iter)->Load_TerrainData(m_pVertexPos);

		CloseHandle(hFile);

		AfxMessageBox(L"로드성공!!");
	}
}
