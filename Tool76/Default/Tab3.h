#pragma once

#include "Tool_Define.h"
#include "afxwin.h"

#include "ToolScene.h"

// CTab3 ��ȭ �����Դϴ�.

class CTab3 : public CDialog
{
	DECLARE_DYNAMIC(CTab3)

public:
	CTab3(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTab3();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	list<IMAGE_INFO*>		m_ListImageInfo;
	map<CString, CImage*>		m_mapPngImage;

private:
	wstring RelativePath(wstring szFullPath);

public:
	afx_msg void OnEnChangeEdit2();
	float m_fBrushSize;
	afx_msg void OnBnClickedBrushChange();
	virtual BOOL OnInitDialog();

	CComboBox m_ComboBox;
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
};
