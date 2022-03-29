
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Tool76.h"

#include "MainFrm.h"
#include "MyForm.h"
#include "Tool76View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

BEGIN_MESSAGE_MAP(CSplitterX, CSplitterWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

LRESULT CSplitterX::OnNcHitTest(CPoint point)
{
	return HTNOWHERE;
}

extern CTool76View *g_pView;

HWND g_hWnd;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CRect rect;
	GetClientRect(&rect);

	if (!m_MainSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	int iSizeX = rect.right - rect.left;
	int iMenuSize = iSizeX / 5;

	if (!m_MainSplitter.CreateView(0, 0, RUNTIME_CLASS(CMyForm), CSize(iMenuSize, 0), pContext))
		return FALSE;

	if (!m_MainSplitter.CreateView(0, 1, RUNTIME_CLASS(CTool76View), CSize(iSizeX - iMenuSize, 0), pContext))
		return FALSE;

	m_pMapForm = (CMyForm*)m_MainSplitter.GetPane(0, 0);
	g_pView = m_pMapView = (CTool76View*)m_MainSplitter.GetPane(0, 1);


	return TRUE;
}
