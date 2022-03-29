#include "..\Headers\Renderer.h"
#include "GameObject.h"

USING(Engine)

CRenderer::CRenderer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject * pGameObject)
{
	if (eRenderGroup >= RENDER_END
		|| nullptr == pGameObject)
		return E_FAIL;

	m_RenderGroup[eRenderGroup].push_back(pGameObject);
	pGameObject->AddRef();

	return NOERROR;
}

HRESULT CRenderer::Ready_Renderer(void)
{
	return NOERROR;
}

void CRenderer::Render_RenderGroup(void)
{
	Render_Priority();
	Render_NonAlpha();
	Render_Alpha();
	Render_UI();

	Clear_RenderGroup();
}

void CRenderer::Render_Priority(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if(nullptr != pGameObject)
			pGameObject->Render_GameObject();
	}

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CRenderer::Render_NonAlpha(void)
{
	for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_GameObject();
	}
}

void CRenderer::Render_Alpha(void)
{
	if (nullptr == m_pGraphicDev)
		return;

	//sort(m_RenderGroup[RENDER_ALPHA].begin(), m_RenderGroup[RENDER_ALPHA].end(), CompareDist<CGameObject*>);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_RenderGroup[RENDER_ALPHA].sort(CompareDist<CGameObject*>);

	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_GameObject();
	}

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_UI(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_RenderGroup[RENDER_UI].sort();

	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_GameObject();
	}
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

}

void CRenderer::Clear_RenderGroup(void)
{
	for (auto& RenderList : m_RenderGroup)
	{
		for (auto& pGameObject : RenderList)
		{
			Engine::Safe_Release(pGameObject);
		}
		RenderList.clear();
	}
}

CComponent * CRenderer::Clone(void)
{
	AddRef();

	return this;
}

CRenderer * CRenderer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRenderer *		pInstance = new CRenderer(pGraphicDev);

	if (FAILED(pInstance->Ready_Renderer()))
	{
		MSG_BOX("CRenderer Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CRenderer::Free(void)
{
	_ulong dwRefCnt = 0;

	for (auto& RenderList : m_RenderGroup)
	{
		for (auto& pGameObject : RenderList)
		{
			Engine::Safe_Release(pGameObject);
		}
		RenderList.clear();
	}

	dwRefCnt = CComponent::Free();

	return dwRefCnt;
}
