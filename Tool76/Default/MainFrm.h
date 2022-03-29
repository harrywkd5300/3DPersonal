
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

class CSplitterX : public CSplitterWnd
{
public:
	CSplitterX() {};
	~CSplitterX() {};
protected:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP();
};

class CTool76View;
class CMyForm;
class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

private:
	CSplitterX		m_MainSplitter;
	CMyForm*		m_pMapForm = nullptr;
	CTool76View*	m_pMapView = nullptr;

// Ư���Դϴ�.
public:

	CMyForm*		GetMapForm()
	{
		return m_pMapForm;
	}
	CTool76View*	GetMapView()
	{
		return m_pMapView;
	}

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};


