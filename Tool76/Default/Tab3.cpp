// Tab3.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool76.h"
#include "Tab3.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "Tool76View.h"


#include "object_Manager.h"
#include "Layer.h"

#include "ToolTerrain.h"


// CTab3 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTab3, CDialog)

CTab3::CTab3(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB3, pParent)
	, m_fBrushSize(5)
{

}

CTab3::~CTab3()
{
	map<CString, CImage*>::iterator iter = m_mapPngImage.begin();
	map<CString, CImage*>::iterator iter_end = m_mapPngImage.end();

	for (iter; iter != iter_end; ++iter)
	{
		iter->second->Destroy();		//이미지 객체를 파괴하자.
		Engine::Safe_Delete(iter->second);
	}
	if (!m_mapPngImage.empty())
		m_mapPngImage.clear();

}

void CTab3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_fBrushSize);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
}


BEGIN_MESSAGE_MAP(CTab3, CDialog)
	ON_EN_CHANGE(IDC_EDIT2, &CTab3::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON6, &CTab3::OnBnClickedBrushChange)
	ON_BN_CLICKED(IDC_BUTTON7, &CTab3::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CTab3::OnBnClickedButton8)
END_MESSAGE_MAP()


// CTab3 메시지 처리기입니다.

wstring CTab3::RelativePath(wstring szFullPath)
{
	TCHAR szRelativePath[MAX_PATH] = L"";
	TCHAR szDirectoryPath[MAX_PATH] = L"";

	GetCurrentDirectory(sizeof(szDirectoryPath), szDirectoryPath);

	PathRelativePathTo(szRelativePath, szDirectoryPath, FILE_ATTRIBUTE_DIRECTORY,
		szFullPath.c_str(), FILE_ATTRIBUTE_NORMAL);

	return szRelativePath;
}


void CTab3::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CTab3::OnBnClickedBrushChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strValue;
	GetDlgItemText(IDC_EDIT1, strValue);
	float fSize = float(_tstof(strValue));

	if (nullptr != CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
	{
		auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();
		dynamic_cast<CToolTerrain*>(iter)->Set_BrushSize(fSize);
		return;
	}
}


BOOL CTab3::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_ComboBox.AddString(_T("Snow_D"));
	m_ComboBox.AddString(_T("SnowBase"));
	m_ComboBox.AddString(_T("SnowMix02"));
	m_ComboBox.AddString(_T("SnowMud"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTab3::OnBnClickedButton7()
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

			D3DLOCKED_RECT		Lock_Rect;

			ZeroMemory(&Lock_Rect, sizeof(D3DLOCKED_RECT));

			dynamic_cast<CToolTerrain*>(iter)->Get_FilterTextrue()->LockRect(0, &Lock_Rect, nullptr, 0);

			dynamic_cast<CToolTerrain*>(iter)->Make_Height();

			for (size_t i = 0; i < vecTexData.y; ++i)
			{
				for (size_t j = 0; j < vecTexData.x; ++j)
				{
					_uint iIndex = i * _uint(vecTexData.y) + j;

					D3DCOLOR Color = ((_ulong*)Lock_Rect.pBits)[iIndex];

					WriteFile(hFile, &Color, sizeof(D3DCOLOR), &dwByte, NULL);
				}
			}
			dynamic_cast<CToolTerrain*>(iter)->Get_FilterTextrue()->UnlockRect(0);
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


void CTab3::OnBnClickedButton8()
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

		auto& iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain")->GetObjList()->front();

		D3DLOCKED_RECT		Lock_Rect;

		dynamic_cast<CToolTerrain*>(iter)->Get_FilterTextrue()->LockRect(0, &Lock_Rect, nullptr, 0);

		for (size_t i = 0; i < vecFilter.y; ++i)
		{
			for (size_t j = 0; j < vecFilter.x; ++j)
			{
				_uint iIndex = i * _uint(vecFilter.y) + j;

				D3DCOLOR Color = ((_ulong*)Lock_Rect.pBits)[iIndex];


				ReadFile(hFile, &((_ulong*)Lock_Rect.pBits)[iIndex], sizeof(D3DCOLOR), &dwByte, nullptr);

			}
		}
		dynamic_cast<CToolTerrain*>(iter)->Get_FilterTextrue()->UnlockRect(0);

		CloseHandle(hFile);

		AfxMessageBox(L"로드성공!!");
	}
}
