#include "..\Headers\Animation_Ctrl.h"

USING(Engine)

CAnimation_Ctrl::CAnimation_Ctrl(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXANIMATIONCONTROLLER	pAnimation_Ctrl)
	: m_pGraphicDev(pGraphicDev)
	, m_pAnimation_Ctrl(pAnimation_Ctrl)
	, m_iCurrentTrack(0)
	, m_iMaxNumAniSet(0)
	, m_iNewTrack(1)
	, m_fTimeAcc(0.f)
	, m_iOldAniSetIdx(0)
	, m_dwPeriod(0)
{
	m_pGraphicDev->AddRef();
	m_pAnimation_Ctrl->AddRef();
}

CAnimation_Ctrl::CAnimation_Ctrl(const CAnimation_Ctrl & rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_pAnimation_Ctrl(nullptr)
	, m_iCurrentTrack(rhs.m_iCurrentTrack)
	, m_iNewTrack(rhs.m_iNewTrack)
	, m_fTimeAcc(rhs.m_fTimeAcc)
	, m_iOldAniSetIdx(rhs.m_iOldAniSetIdx)
	, m_dwPeriod(rhs.m_dwPeriod)
{
	rhs.m_pAnimation_Ctrl->CloneAnimationController(rhs.m_pAnimation_Ctrl->GetMaxNumAnimationOutputs(),
													m_iMaxNumAniSet = rhs.m_pAnimation_Ctrl->GetNumAnimationSets(),
													rhs.m_pAnimation_Ctrl->GetMaxNumTracks(),
													rhs.m_pAnimation_Ctrl->GetMaxNumEvents(),
													&m_pAnimation_Ctrl);
	// �ִϸ��̼� ��Ʈ�ѷ��� �������� �����͸� �������ش�.  ==> ���� ����� ���ش� 
	// ��ü�� ���������� ��쿡 ���� ����� �ϸ� ��� ��ü���� ���� �ִϸ��̼��� ������ �Ǿ ���� ����� ó���Ѵ�.
	m_pAnimation_Ctrl->AddRef();
	m_pGraphicDev->AddRef();
}

HRESULT CAnimation_Ctrl::Ready_Animation_Ctrl()
{
	return NOERROR;
}

HRESULT CAnimation_Ctrl::Set_AnimationSet(const _uint & iAni_idx)
{
	if (iAni_idx == m_iOldAniSetIdx) // ������ �ִϸ��̼ǰ� ���� Set�ϴ� �ִϸ��̼��� ������ ������ 
		return E_FAIL;

	if (iAni_idx >= m_iMaxNumAniSet)  // �� �ִϸ��̼��� ������ �ִ� ������ �Ѿ�� ������. �ִ밹���� �� Ŭ�����ٶ� �޴´�
		return E_FAIL;

	if (nullptr == m_pAnimation_Ctrl)
		return E_FAIL;

	m_iNewTrack = m_iCurrentTrack == 0 ? 1 : 0;   // m_iNewTrack �� m_iCurrentTrack�� ���� ���� ������ ���� �����.

	LPD3DXANIMATIONSET		pAniSet = nullptr;   // �ִϸ��̼ǵ��� ���� ����

	if (FAILED(m_pAnimation_Ctrl->GetAnimationSet(iAni_idx, &pAniSet)))    // X���Ͽ� ����� �ִϸ��̼��� �ּ�(iAni_idx)�� �־� ������´�.
		return E_FAIL;

	if (FAILED(m_pAnimation_Ctrl->SetTrackAnimationSet(m_iNewTrack, pAniSet)))  // m_iNewTrack �� �ִϸ��̼��� ���Ѵ�.
		return E_FAIL;

	// Ʈ���� �ִ� ��� �̺�Ʈ�� �����ش�.
	m_pAnimation_Ctrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAnimation_Ctrl->UnkeyAllTrackEvents(m_iNewTrack);

	// ���� Ʈ������ ���� �ð����� ���� ������ ������ �ð���ŭ
	m_pAnimation_Ctrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fTimeAcc + 0.25f);
	m_pAnimation_Ctrl->KeyTrackSpeed(m_iCurrentTrack, 1.0f, m_fTimeAcc, 0.25f, D3DXTRANSITION_LINEAR);
	m_pAnimation_Ctrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fTimeAcc, 0.25f, D3DXTRANSITION_LINEAR);

	// �ִϸ��̼��� Ʈ���� �����Ѵ�. ���� Set������ ����� ���ϰ� Set�� ���� ���� �� Ʈ������ ������ͼ� �ִϸ��̼��� �������Ѵ�.
	// ���� Set�� ���� ä�� �ִϸ��̼��� ������ �� �ִϸ��̼Ǹ����� ���谡 ���ڿ�����������. Ʈ�������� ������Ÿ���� �� ���谡 �ڿ�����������.

	if (FAILED(m_pAnimation_Ctrl->SetTrackEnable(m_iNewTrack, TRUE)))
		return E_FAIL;
	// Ʈ���� ��밡���ϰ� ������ش�.

	m_pAnimation_Ctrl->KeyTrackSpeed(m_iNewTrack, 1.0f, m_fTimeAcc, 0.25f, D3DXTRANSITION_LINEAR);
	// ���� Ʈ���� �ӵ��� ����ð����κ��� 0���� ���������Ѵ�.

	m_pAnimation_Ctrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fTimeAcc, 0.25f, D3DXTRANSITION_LINEAR);
	// ���� Ʈ���� ����ġ�� ����ð����κ��� 0���� ���������Ѵ�.

	m_pAnimation_Ctrl->SetTrackPosition(m_iNewTrack, 0.0);  // �ٸ� �ִϸ��̼����� �ٲ�� Ʈ���� ��ġ�� ���� ������ �ű��.
																// ���������� �߰��� ���� �� ��ġ���� ������ �Ǿ� ������.
	m_pAnimation_Ctrl->ResetTime();   // ������Ų �ð��� �ʱ�ȭ ��Ų��.

	m_fTimeAcc = 0.f;				  // �ʱ�ȭ�Ѵ�.

	m_iOldAniSetIdx = iAni_idx;       // �ִϸ��̼� ����

	m_iCurrentTrack = m_iNewTrack;

	m_dwPeriod = pAniSet->GetPeriod();   // ����ִϸ��̼��� �ִ�ð��� �־��ش�

	return NOERROR;
}


void CAnimation_Ctrl::Play_Animation(const _float & fDeltaTime, const _float & fAniSpeed)
{
	if (nullptr == m_pAnimation_Ctrl)
		return;

	m_pAnimation_Ctrl->AdvanceTime(fDeltaTime * fAniSpeed, nullptr);

	m_fTimeAcc += fDeltaTime;
}

_bool CAnimation_Ctrl::End_Animation(const _double& Delay)
{
	if (nullptr == m_pAnimation_Ctrl)
		return false;

	D3DXTRACK_DESC				TrackInfo;   // Ʈ���� ������ ��� ����
											 // Weight, Speed, Position, Enable ��� ����

	m_pAnimation_Ctrl->GetTrackDesc(m_iCurrentTrack, &TrackInfo);   // Ʈ���� ������ �޾ƿ´�.

	if (m_dwPeriod < TrackInfo.Position + Delay)
	{
		return true;
	}

	return false;
}

void CAnimation_Ctrl::Set_Animation()
{
	if (nullptr == m_pAnimation_Ctrl)
		return;

	D3DXTRACK_DESC				TrackInfo;   // Ʈ���� ������ ��� ����

	m_pAnimation_Ctrl->GetTrackDesc(m_iCurrentTrack, &TrackInfo);

	m_pAnimation_Ctrl->SetTrackPosition(m_iCurrentTrack, 0.0);
}

CAnimation_Ctrl * CAnimation_Ctrl::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXANIMATIONCONTROLLER	pAnimation_Ctrl)
{
	CAnimation_Ctrl*	pInstance = new CAnimation_Ctrl(pGraphicDev, pAnimation_Ctrl);

	if (FAILED(pInstance->Ready_Animation_Ctrl()))
	{
		MSG_BOX("CAnitmation_Ctrl Created Failed!!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation_Ctrl * CAnimation_Ctrl::Create(const CAnimation_Ctrl & rhs)
{
	CAnimation_Ctrl*	pInstance = new CAnimation_Ctrl(rhs);

	if (FAILED(pInstance->Ready_Animation_Ctrl()))
	{
		MSG_BOX("CAnitmation_Ctrl Created Failed!!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CAnimation_Ctrl::Free()
{
	Safe_Release(m_pAnimation_Ctrl);
	Safe_Release(m_pGraphicDev);

	return 0;
}
