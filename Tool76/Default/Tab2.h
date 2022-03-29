#pragma once

#include "Tool_Define.h"
#include "afxwin.h"

#include "ToolScene.h"
#include "afxcmn.h"

// CTab2 대화 상자입니다.

class CTab2 : public CDialog
{
	DECLARE_DYNAMIC(CTab2)

public:
	CTab2(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTab2();

private:
	map<CString, CString>				m_mapMesh;
	typedef map<CString, CString>		MAPMESH;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	void	HorizontalScroll();
	wstring RelativePath(wstring szFullPath);

private:
	const _tchar* pComponentName = L"";
	list<IMAGE_INFO*>		m_ListImageInfo;

	vector<OBJECT*>			m_vecObject;

private:
	void	Release();

private:
	void DirectoryInfoExtraction(const wstring& wstrPath,
		list<IMAGE_INFO*>& rPathInfoList);

public:
	list<IMAGE_INFO*>*	Get_ListObj() { return &m_ListImageInfo; }
	void				Set_TransData(const _vec3 * vPos, const _vec3 * vScale, const _vec3 * vRot);

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeObjectlist();
	afx_msg void OnBnClickedCreate();
	afx_msg void OnBnClickedDelete();
	CTreeCtrl m_TreeControl;
	float m_fMakeScaleX;
	float m_fMakeScaleY;
	float m_fMakeScaleZ;
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedChange();
	float m_fPosX;
	float m_fPosY;
	float m_fPosZ;
	float m_fScaleX;
	float m_fScaleY;
	float m_fScaleZ;
	float m_fRotationX;
	float m_fRotationY;
	float m_fRotationZ;
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	BOOL m_bViewWireFrame;
	afx_msg void OnBnClickedCheck5();
};
