// ../Codes/MyForm.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "Tool76.h"
#include "MyForm.h"


// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_MYFORM)
{

}

CMyForm::~CMyForm()
{
	if (NULL != m_pTab1)
	{
		delete m_pTab1;
		m_pTab1 = NULL;
	}
	if (NULL != m_pTab2)
	{
		delete m_pTab2;
		m_pTab2 = NULL;
	}

	if (NULL != m_pTab3)
	{
		delete m_pTab3;
		m_pTab3 = NULL;
	}
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMyForm::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CMyForm 진단입니다.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기입니다.

void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_Tab.DeleteAllItems();
	m_Tab.InsertItem(0, L"Map");
	m_Tab.InsertItem(1, L"Object");
	m_Tab.InsertItem(2, L"Texture");

	CRect rect;

	m_pTab1 = new CTab1;
	m_pTab1->Create(IDD_TAB1, &m_Tab);
	m_pTab1->GetWindowRect(&rect);
	m_pTab1->MoveWindow(5, 25, rect.Width(), rect.Height());
	m_pTab1->ShowWindow(SW_SHOW);

	m_pTab2 = new CTab2;
	m_pTab2->Create(IDD_TAB2, &m_Tab);
	m_pTab2->GetWindowRect(&rect);
	m_pTab2->MoveWindow(5, 25, rect.Width(), rect.Height());
	m_pTab2->ShowWindow(SW_HIDE);

	m_pTab3 = new CTab3;
	m_pTab3->Create(IDD_TAB3, &m_Tab);
	m_pTab3->GetWindowRect(&rect);
	m_pTab3->MoveWindow(5, 25, rect.Width(), rect.Height());
	m_pTab3->ShowWindow(SW_HIDE);

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

void CMyForm::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int select = m_Tab.GetCurSel();

	switch (select)
	{
	case 0:
		m_pTab1->ShowWindow(SW_SHOW);
		m_pTab2->ShowWindow(SW_HIDE);
		m_pTab3->ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pTab1->ShowWindow(SW_HIDE);
		m_pTab2->ShowWindow(SW_SHOW);
		m_pTab3->ShowWindow(SW_HIDE);
		break;
	case 2:
		m_pTab1->ShowWindow(SW_HIDE);
		m_pTab2->ShowWindow(SW_HIDE);
		m_pTab3->ShowWindow(SW_SHOW);
		break;
	}
}
