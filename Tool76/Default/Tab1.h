#pragma once
#include "afxwin.h"

// CTab1 대화 상자입니다.
#include "ToolScene.h"

class CTab1 : public CDialog
{
	DECLARE_DYNAMIC(CTab1)

public:
	CTab1(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTab1();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB1 };
#endif

private:
	void	HorizontalScroll();
	wstring RelativePath(wstring szFullPath);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	int m_iCellCount;
	int m_iVertexX;
	int m_iVertexZ;
	int m_iMakeCount;

	float m_fInterval;
	float m_fPointX;
	float m_fPointY;
	float m_fPointZ;

	_vec3*	m_pVertexPos = nullptr;

public:
	void		 PlusCount();
	void		 Set_PointXYZ(const float& fX, const float& fY, const float& fZ);
	void		 Set_MakeCount();
	_float&		 Get_PointY();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtoncreate();
	afx_msg void OnBnClickedButtontexturecreate();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedChange();

	BOOL m_bHeightUsing;
	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedChange2();
	float m_fBrushSize;

	CButton m_bRadioHeight;

	_bool CheckRadio(const _uint& iIdx);
	float m_fMaxHeight;
	afx_msg void OnEnChangeEdit6();
	afx_msg void OnBnClickedSave2();
	afx_msg void OnBnClickedLoad2();
};
