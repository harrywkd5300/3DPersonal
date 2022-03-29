#pragma once

#include "afxcmn.h"

// CMyForm 폼 뷰입니다.
#include "Tab1.h"
#include "Tab2.h"
#include "Tab3.h"

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMyForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CTabCtrl		m_Tab;
	CTab1*			m_pTab1 = nullptr;
	CTab2*			m_pTab2 = nullptr;
	CTab3*			m_pTab3 = nullptr;
	virtual void OnInitialUpdate();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);

public:

	CTab1*	GetTab1()
	{
		return m_pTab1;
	}

	CTab2*	GetTab2()
	{
		return m_pTab2;
	}

	CTab3*	GetTab3()
	{
		return m_pTab3;
	}

	CTabCtrl*	GetTabCtrl()
	{
		return &m_Tab;
	}
};


