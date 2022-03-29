// ../Codes/MyForm.cpp : ���� �����Դϴ�.
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


// CMyForm �����Դϴ�.

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


// CMyForm �޽��� ó�����Դϴ�.

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

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}

void CMyForm::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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
