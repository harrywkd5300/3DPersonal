// Tab3.cpp : ���� �����Դϴ�.
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


// CTab3 ��ȭ �����Դϴ�.

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
		iter->second->Destroy();		//�̹��� ��ü�� �ı�����.
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


// CTab3 �޽��� ó�����Դϴ�.

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
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CTab3::OnBnClickedBrushChange()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_ComboBox.AddString(_T("Snow_D"));
	m_ComboBox.AddString(_T("SnowBase"));
	m_ComboBox.AddString(_T("SnowMix02"));
	m_ComboBox.AddString(_T("SnowMud"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CTab3::OnBnClickedButton7()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr != CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
	{
		CFileDialog		Dlg(FALSE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE);

		TCHAR szDirectory[MAX_PATH] = L"";		//����̺��θ� ���� ���ڿ� ����.

		GetCurrentDirectory(MAX_PATH, szDirectory);		//���� ������Ʈ�� ����̺��θ� �޾ƿ´�.
		PathRemoveFileSpec(szDirectory);

		lstrcat(szDirectory, L"\\Test");

		Dlg.GetOFN().lpstrInitialDir = szDirectory;

		if (Dlg.DoModal() == IDOK)
		{
			HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS
				, FILE_ATTRIBUTE_NORMAL, NULL);


			if (INVALID_HANDLE_VALUE == hFile)
			{
				AfxMessageBox(L"�������.");
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if (nullptr == CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Terrain"))
	{
		AfxMessageBox(L"Don't Create Terrain!!");
		return;
	}

	UpdateData(TRUE);

	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

	CFileDialog		Dlg(TRUE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE);

	TCHAR szDirectory[MAX_PATH] = L"";		//����̺��θ� ���� ���ڿ� ����.

	GetCurrentDirectory(MAX_PATH, szDirectory);		//���� ������Ʈ�� ����̺��θ� �޾ƿ´�.
	PathRemoveFileSpec(szDirectory);

	lstrcat(szDirectory, L"\\Test");

	Dlg.GetOFN().lpstrInitialDir = szDirectory;

	if (Dlg.DoModal() == IDOK)
	{
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_READ, NULL, NULL, OPEN_EXISTING
			, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			AfxMessageBox(L"�ε����.");
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

		AfxMessageBox(L"�ε强��!!");
	}
}
