// ../Codes/Tab2.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool76.h"
#include "Tab2.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "Tool76View.h"


#include "object_Manager.h"
#include "Layer.h"
#include "StaticMesh.h"
#include "DynamicObject.h"

// CTab2 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTab2, CDialog)

CTab2::CTab2(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB2, pParent)
	, m_fMakeScaleX(0.01f)
	, m_fMakeScaleY(0.01f)
	, m_fMakeScaleZ(0.01f)
	, m_fPosX(0.f)
	, m_fPosY(0.f)
	, m_fPosZ(0.f)
	, m_fScaleX(0.01)
	, m_fScaleY(0.01)
	, m_fScaleZ(0.01)
	, m_fRotationX(0)
	, m_fRotationY(0)
	, m_fRotationZ(0)
	, m_bViewWireFrame(FALSE)
{

}

CTab2::~CTab2()
{
	Release();
}

void CTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_OBJECTLIST, m_ObjectListBox);
	DDX_Control(pDX, IDC_TREE1, m_TreeControl);
	DDX_Text(pDX, IDC_EDIT4, m_fMakeScaleX);
	DDX_Text(pDX, IDC_EDIT5, m_fMakeScaleY);
	DDX_Text(pDX, IDC_EDIT6, m_fMakeScaleZ);
	DDX_Text(pDX, IDC_EDIT1, m_fPosX);
	DDX_Text(pDX, IDC_EDIT2, m_fPosY);
	DDX_Text(pDX, IDC_EDIT3, m_fPosZ);
	DDX_Text(pDX, IDC_EDIT12, m_fScaleX);
	DDX_Text(pDX, IDC_EDIT13, m_fScaleY);
	DDX_Text(pDX, IDC_EDIT14, m_fScaleZ);
	DDX_Text(pDX, IDC_EDIT15, m_fRotationX);
	DDX_Text(pDX, IDC_EDIT16, m_fRotationY);
	DDX_Text(pDX, IDC_EDIT11, m_fRotationZ);
	DDX_Check(pDX, IDC_CHECK5, m_bViewWireFrame);
}

void CTab2::HorizontalScroll()
{
	CString strName;
	CSize	Size;

	int iCX = 0;


}

wstring CTab2::RelativePath(wstring szFullPath)
{
	TCHAR szRelativePath[MAX_PATH] = L"";
	TCHAR szDirectoryPath[MAX_PATH] = L"";

	GetCurrentDirectory(sizeof(szDirectoryPath), szDirectoryPath);

	PathRelativePathTo(szRelativePath, szDirectoryPath, FILE_ATTRIBUTE_DIRECTORY,
		szFullPath.c_str(), FILE_ATTRIBUTE_NORMAL);

	return szRelativePath;
}


BEGIN_MESSAGE_MAP(CTab2, CDialog)
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE(IDC_OBJECTLIST, &CTab2::OnLbnSelchangeObjectlist)
	ON_BN_CLICKED(IDC_BUTTON1, &CTab2::OnBnClickedCreate)
	ON_BN_CLICKED(IDC_BUTTON2, &CTab2::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_CHECK1, &CTab2::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON3, &CTab2::OnBnClickedChange)
	ON_BN_CLICKED(IDC_BUTTON5, &CTab2::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON4, &CTab2::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_CHECK5, &CTab2::OnBnClickedCheck5)
END_MESSAGE_MAP()


// CTab2 메시지 처리기입니다.


void CTab2::Release()
{
	auto& iter = m_vecObject.begin();
	auto& iter_end = m_vecObject.end();
	for (; iter != iter_end; ++iter)
	{
		Safe_Delete((*iter));
	}
	m_vecObject.clear();
}

void CTab2::Set_TransData(const _vec3 * vPos, const _vec3 * vScale, const _vec3 * vRot)
{
	CString strValue;

	m_fPosX = vPos->x;
	m_fPosY = vPos->y;
	m_fPosZ = vPos->z;

	strValue.Format(_T("%f"), vPos->x);
	SetDlgItemText(IDC_EDIT1, strValue);

	strValue.Format(_T("%f"), vPos->y);
	SetDlgItemText(IDC_EDIT2, strValue);

	strValue.Format(_T("%f"), vPos->z);
	SetDlgItemText(IDC_EDIT3, strValue);

	m_fScaleX = vScale->x;
	m_fScaleY = vScale->y;
	m_fScaleZ = vScale->z;

	strValue.Format(_T("%f"), vScale->x);
	SetDlgItemText(IDC_EDIT12, strValue);

	strValue.Format(_T("%f"), vScale->y);
	SetDlgItemText(IDC_EDIT13, strValue);

	strValue.Format(_T("%f"), vScale->z);
	SetDlgItemText(IDC_EDIT14, strValue);

	m_fRotationX = vRot->x;
	m_fRotationY = vRot->y;
	m_fRotationZ = vRot->z;

	strValue.Format(_T("%f"), vRot->x);
	SetDlgItemText(IDC_EDIT15, strValue);

	strValue.Format(_T("%f"), vRot->y);
	SetDlgItemText(IDC_EDIT16, strValue);

	strValue.Format(_T("%f"), vRot->z);
	SetDlgItemText(IDC_EDIT11, strValue);


}

void CTab2::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnDropFiles(hDropInfo);

	UpdateData(TRUE);

	int iFileCount = DragQueryFile(hDropInfo, -1, NULL, 0);

	TCHAR szFullPath[MAX_PATH] = L"";

	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

	for (int i = 0; i < iFileCount; ++i)
	{
		DragQueryFile(hDropInfo, i, szFullPath, MAX_PATH);

		DirectoryInfoExtraction(szFullPath, m_ListImageInfo);

		list<IMAGE_INFO*>::iterator iter = m_ListImageInfo.begin();

		HTREEITEM		hRoot = nullptr;
		HTREEITEM		hItem[64];
		ZeroMemory(hItem, sizeof(HTREEITEM) * 64);

		HTREEITEM		hXFile[64];
		ZeroMemory(hXFile, sizeof(HTREEITEM) * 64);


		for (; iter != m_ListImageInfo.end(); ++iter)
		{
			_uint iIdx = 0;
			_uint iItemIdx = 0;

			_bool bMeshState = true;

			if (!lstrcmp((*iter)->RootName.c_str(), L"StaticMesh"))
				bMeshState = true;
			else
				bMeshState = false;

			if (FAILED(dynamic_cast<CToolScene*>(pScene)->Add_MsehComponent((*iter)->FullPath, (*iter)->FileName, (*iter)->ComName, bMeshState)))
				continue;

			if (m_TreeControl.GetItemText(hRoot) != (*iter)->RootName.c_str())
			{
				hRoot = m_TreeControl.InsertItem((*iter)->RootName.c_str(), 0, 1, TVI_ROOT, TVI_LAST);
			}

			if (m_TreeControl.GetItemText(hItem[iIdx]) != (*iter)->FileRootName.c_str())
			{
				hItem[iIdx] = m_TreeControl.InsertItem((*iter)->FileRootName.c_str(), 1, 1, hRoot, TVI_LAST);
			}

			if (m_TreeControl.GetItemText(hXFile[iIdx]) != (*iter)->FileName)
			{
				if (m_TreeControl.GetItemText(hItem[iIdx]) != (*iter)->FileRootName.c_str())
					++iIdx;

				hXFile[iItemIdx] = m_TreeControl.InsertItem((*iter)->FileName, 1, 1, hItem[iIdx], TVI_LAST);
			}
		}

		//CloseHandle(hFile);
	}
	CDialog::OnDropFiles(hDropInfo);

	UpdateData(FALSE);

}


BOOL CTab2::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CheckDlgButton(IDC_RADIO5, TRUE);

	CheckDlgButton(IDC_CHECK1, TRUE);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTab2::OnLbnSelchangeObjectlist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}


void CTab2::OnBnClickedCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hTreeItem = m_TreeControl.GetSelectedItem();

	CString Item = m_TreeControl.GetItemText(hTreeItem);

	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

	list<IMAGE_INFO*>::iterator iter = m_ListImageInfo.begin();

	for (; iter != m_ListImageInfo.end(); ++iter)
	{
		if (!lstrcmp(Item, (*iter)->FileName))
		{
			break;
		}
	}
	_vec3 vScale = _vec3(0.f, 0.f, 0.f);
	CString strValue;
	GetDlgItemText(IDC_EDIT4, strValue);
	vScale.x = float(_tstof(strValue));
	GetDlgItemText(IDC_EDIT5, strValue);
	vScale.y = float(_tstof(strValue));
	GetDlgItemText(IDC_EDIT6, strValue);
	vScale.z = float(_tstof(strValue));

	_bool	Static = true;
	if (IsDlgButtonChecked(IDC_RADIO5))
		Static = true;
	else if(IsDlgButtonChecked(IDC_RADIO6))
		Static = false;

	if (FAILED(dynamic_cast<CToolScene*>(pScene)->Add_MeshObject((*iter)->FullPath, (*iter)->FileName, (*iter)->ComName, vScale, _vec3(0.f, 0.f, 0.f), _vec3(0.f, 0.f, 0.f), Static)))
	{
		AfxMessageBox(L"Failed Add Mesh Component");
		return;
	}
}


void CTab2::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();
	dynamic_cast<CToolScene*>(pScene)->Set_DeleteSelObject();
}


void CTab2::DirectoryInfoExtraction(const wstring& wstrPath,
	list<IMAGE_INFO*>& rPathInfoList)
{
	//MFC에서 지원해주는 클래스.
	CFileFind	Find;

	Find.FindFile(wstrPath.c_str());

	int iContinue = 1;

	while (iContinue)
	{
		iContinue = Find.FindNextFile();
		//FindNextFile() : 파일 유무를 검사하는 함수.

		if (Find.IsDots())		//내 경로중에 .과 ..을 걸러내기 위한 함수.
			continue;

		//검색되는 파일이 폴더인지? 파일인지를 구분하자.
		if (Find.IsDirectory())		//현재 파일이 폴더이다.
		{
			//재귀함수를 통해서 파일을 찾을때까지 호출하도록 하자.

			DirectoryInfoExtraction(wstring(Find.GetFilePath() + L"\\*.*"), rPathInfoList);
		}
		else		//파일
		{
			//파일을 찾았지만 찾은 파일이 시스템 파일인 경우에 예외처리를 하자.
			if (Find.IsSystem())
				continue;

			IMAGE_INFO*		pImagePath = new IMAGE_INFO;

			TCHAR	szPathBuffer[MAX_PATH] = L"";


			//추출한 경로를 문자열로 복사를 하자.
			lstrcpy(szPathBuffer, Find.GetFilePath().operator LPCWSTR());
			//\Texture\Stage\Effect\BossMultiAttack\BossMultiAttack0.png

			wstring wstrRelativePath = RelativePath(szPathBuffer);
			wstring strcmpPath = wstrRelativePath.substr(wstrRelativePath.size() - 1, wstrRelativePath.size());
			wstring x = L"x";
			wstring y = L"X";

			if (x.compare(strcmpPath) == 0 || y.compare(strcmpPath) == 0)
			{
				//파일이름을 얻어오자.
				wstring wstrImageName = Find.GetFileTitle().operator LPCWSTR();

				CString strPlusName = L"Component_Buffer_";

				strPlusName += wstrImageName.c_str();

				lstrcpy(pImagePath->ComName, strPlusName);

				wstrImageName += L".X";

				lstrcpy(pImagePath->FileName, wstrImageName.c_str());
			}
			else
				continue;

			wstring FullPath;

			PathRemoveFileSpec(szPathBuffer);

			pImagePath->FileRootName = PathFindFileName(szPathBuffer);

			FullPath = L"/" + pImagePath->FileRootName + L"/";

			PathRemoveFileSpec(szPathBuffer);

			pImagePath->RootName = PathFindFileName(szPathBuffer);

			FullPath = L"/" + pImagePath->RootName  + FullPath;

			PathRemoveFileSpec(szPathBuffer);

			wstring str1 = PathFindFileName(szPathBuffer);

			FullPath = L"/" + str1 + FullPath;

			PathRemoveFileSpec(szPathBuffer);

			wstring str2 = PathFindFileName(szPathBuffer);

			FullPath = L"/" + str2 + FullPath;

			PathRemoveFileSpec(szPathBuffer);

			wstring str3 = PathFindFileName(szPathBuffer);

			FullPath = L"../" + str3 + FullPath;

			lstrcpy(pImagePath->FullPath, FullPath.c_str());
			
			auto iter = rPathInfoList.begin();

			for (; iter != rPathInfoList.end(); ++iter)
			{
				if (!lstrcmp((*iter)->FileName, pImagePath->FileName))
					break;
			}

			if (iter == rPathInfoList.end())
				rPathInfoList.push_back(pImagePath);
		}
	}
	Find.FindFile(Find.GetFilePath());
	Find.FindNextFile();
}

void CTab2::OnBnClickedCheck1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();
	if (IsDlgButtonChecked(IDC_CHECK1))
	{
		dynamic_cast<CToolScene*>(pScene)->Set_ObjectCollBox(true);
	}
	else
	{
		dynamic_cast<CToolScene*>(pScene)->Set_ObjectCollBox(false);
	}
}


void CTab2::OnBnClickedChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

	CGameObject* pObject = nullptr;
	pObject = dynamic_cast<CToolScene*>(pScene)->Get_SelObject();
	if (nullptr != pObject)
	{
		if (IsDlgButtonChecked(IDC_CHECK2))
		{
			_vec3 vChangePos;
			CString strValue;
			GetDlgItemText(IDC_EDIT1, strValue);
			vChangePos.x = float(_tstof(strValue));
			GetDlgItemText(IDC_EDIT2, strValue);
			vChangePos.y = float(_tstof(strValue));
			GetDlgItemText(IDC_EDIT3, strValue);
			vChangePos.z = float(_tstof(strValue));

			dynamic_cast<CToolScene*>(pScene)->Set_SelObjectTrans(&vChangePos, 0);
			return;
		}
		else if (IsDlgButtonChecked(IDC_CHECK3))
		{
			_vec3 vChangePos;
			CString strValue;
			GetDlgItemText(IDC_EDIT12, strValue);
			vChangePos.x = float(_tstof(strValue));
			GetDlgItemText(IDC_EDIT13, strValue);
			vChangePos.y = float(_tstof(strValue));
			GetDlgItemText(IDC_EDIT14, strValue);
			vChangePos.z = float(_tstof(strValue));

			dynamic_cast<CToolScene*>(pScene)->Set_SelObjectTrans(&vChangePos, 1);
			return;
		}
		else if (IsDlgButtonChecked(IDC_CHECK4))
		{
			_vec3 vChangePos;
			CString strValue;
			GetDlgItemText(IDC_EDIT15, strValue);
			vChangePos.x = float(_tstof(strValue));
			GetDlgItemText(IDC_EDIT16, strValue);
			vChangePos.y = float(_tstof(strValue));
			GetDlgItemText(IDC_EDIT11, strValue);
			vChangePos.z = float(_tstof(strValue));

			dynamic_cast<CToolScene*>(pScene)->Set_SelObjectTrans(&vChangePos, 2);
			return;
		}
	}
	else
	{
		AfxMessageBox(L"Don't Select Object!!");
		return;
	}
}


void CTab2::OnBnClickedSave()
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

		if (nullptr == CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object"))
			return;

		auto iter = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->begin();
		auto iter_end = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->end();

		_ulong		dwNumCell = CObject_Manager::GetInstance()->Find_Layer(TOOL_STATIC, L"Layer_Object")->GetObjList()->size();

		WriteFile(hFile, &dwNumCell, sizeof(_ulong), &dwByte, 0);

		for (; iter != iter_end; ++iter)
		{
			WriteFile(hFile, dynamic_cast<CStaticMesh*>((*iter))->Get_ObjectData() , sizeof(OBJECT), &dwByte, NULL);
		}

		AfxMessageBox(L"Save Succesed!!");
		CloseHandle(hFile);
		return;
	}

}


void CTab2::OnBnClickedLoad()
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

		_ulong dwNumObject = 0;

		ReadFile(hFile, &dwNumObject, sizeof(_ulong), &dwByte, nullptr);

		m_vecObject.reserve(dwNumObject);

		while (true)
		{
			OBJECT* pObject = new OBJECT;
			ZeroMemory(pObject, sizeof(OBJECT));

			ReadFile(hFile, pObject, sizeof(OBJECT), &dwByte, nullptr);

			if (0 == dwByte)
			{
				Engine::Safe_Delete(pObject);
				break;
			}
			m_vecObject.push_back(pObject);
		}
		CloseHandle(hFile);

		auto& iter = m_vecObject.begin();
		auto& iter_end = m_vecObject.end();

		for ( ; iter != iter_end ; ++iter)
		{
			TCHAR szFullPath[256], szFileName[64], szComName[64];
			ZeroMemory(szFullPath, sizeof(TCHAR) * 256);
			ZeroMemory(szFileName, sizeof(TCHAR) * 64 );
			ZeroMemory(szComName , sizeof(TCHAR) * 64 );
			lstrcpy(szFullPath, (*iter)->FullPath);
			lstrcpy(szFileName, (*iter)->FileName);
			lstrcpy(szComName, (*iter)->ComName);

			if (FAILED(dynamic_cast<CToolScene*>(pScene)->Add_MsehComponent(szFullPath, szFileName, szComName, true)))
			{
				AfxMessageBox(L"Failed Add Component");
				return;
			}

			if (FAILED(dynamic_cast<CToolScene*>(pScene)->Add_MeshObject(szFullPath, szFileName, szComName, (*iter)->vScale, (*iter)->vPostion, (*iter)->vRotation, true)))
			{
				AfxMessageBox(L"Failed Add Mesh Component");
				return;
			}
		}

		AfxMessageBox(L"로드성공!!");
	}
}


void CTab2::OnBnClickedCheck5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CScene* pScene = ((CMainFrame*)AfxGetMainWnd())->GetMapView()->GetScene();

	if (IsDlgButtonChecked(IDC_CHECK5))
	{
		dynamic_cast<CToolScene*>(pScene)->Obj_WireFrame(true);
	}
	else
	{
		dynamic_cast<CToolScene*>(pScene)->Obj_WireFrame(false);
	}
}
