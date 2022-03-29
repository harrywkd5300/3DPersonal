
// Tool76View.h : CTool76View Ŭ������ �������̽�
//
#pragma once

#include "Graphic_Device.h"

#include "Tool_Define.h"

#include "Management.h"
#include "component_manager.h"

namespace Engine
{
	class CInput_Device;
	class CTimer_Manager;
	class CFrame_Manager;
}


class CTool76Doc;
class CTool76View : public CView
{
protected: // serialization������ ��������ϴ�.
	CTool76View();
	DECLARE_DYNCREATE(CTool76View)

private:
	CGraphic_Device*		m_pDevice = nullptr;
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	CManagement*			m_pManagement = nullptr;
	CComponent_Manager*		m_pComponent_Manager = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CScene*					m_pScene = nullptr;

private:
	_int					m_iSelectCellIdx = 0;

public:
	CInput_Device*			m_pInput_Device = nullptr;
	CTimer_Manager*			pTimer_Manager = nullptr;
	CFrame_Manager*			pFrame_Manager = nullptr;
	_float					fTimeDelta = 0.f;
	_vec3					PickingPos;

	_bool					m_bClick = false;
	_bool					m_bMoveClickPos = false;

// Ư���Դϴ�.
public:
	CTool76Doc* GetDocument() const;

// �۾��Դϴ�.
public:
	HRESULT Ready_DeviceSetting();
	HRESULT Ready_StaticComponent(void);
	HRESULT Ready_StartScene(void);
	void	TimeUpdate();
	void    Management_Update(_float& fTimeDelta);
	void    Management_Render();
	CScene*	GetScene() { return m_pScene; }
	
	const _int&		Get_CellIdx() { return m_iSelectCellIdx; }

	void			Set_CellIIdx(const _int& idx) {
		m_iSelectCellIdx = idx;
	}

	const _vec3&	Get_LastPos() { return PickingPos; }

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CTool76View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	_ulong	Free();

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // Tool76View.cpp�� ����� ����
inline CTool76Doc* CTool76View::GetDocument() const
   { return reinterpret_cast<CTool76Doc*>(m_pDocument); }
#endif

