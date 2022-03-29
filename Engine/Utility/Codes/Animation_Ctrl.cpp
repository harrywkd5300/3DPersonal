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
	// 애니메이션 컨트롤러에 복사해준 데이터를 복제해준다.  ==> 깊은 복사로 해준다 
	// 객체가 여러가지일 경우에 얕은 복사로 하면 모든 객체들이 같은 애니메이션을 가지게 되어서 깊은 복사로 처리한다.
	m_pAnimation_Ctrl->AddRef();
	m_pGraphicDev->AddRef();
}

HRESULT CAnimation_Ctrl::Ready_Animation_Ctrl()
{
	return NOERROR;
}

HRESULT CAnimation_Ctrl::Set_AnimationSet(const _uint & iAni_idx)
{
	if (iAni_idx == m_iOldAniSetIdx) // 이전의 애니메이션과 현재 Set하는 애니메이션이 같으면 나가라 
		return E_FAIL;

	if (iAni_idx >= m_iMaxNumAniSet)  // 총 애니메이션의 갯수가 최대 갯수를 넘어가면 나간다. 최대갯수는 위 클론해줄때 받는다
		return E_FAIL;

	if (nullptr == m_pAnimation_Ctrl)
		return E_FAIL;

	m_iNewTrack = m_iCurrentTrack == 0 ? 1 : 0;   // m_iNewTrack 과 m_iCurrentTrack이 같은 수를 가질수 없게 만든다.

	LPD3DXANIMATIONSET		pAniSet = nullptr;   // 애니메이션들을 담을 공간

	if (FAILED(m_pAnimation_Ctrl->GetAnimationSet(iAni_idx, &pAniSet)))    // X파일에 저장된 애니메이션의 주소(iAni_idx)를 넣어 가지고온다.
		return E_FAIL;

	if (FAILED(m_pAnimation_Ctrl->SetTrackAnimationSet(m_iNewTrack, pAniSet)))  // m_iNewTrack 에 애니메이션을 셋한다.
		return E_FAIL;

	// 트랙에 있는 모든 이벤트를 없애준다.
	m_pAnimation_Ctrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAnimation_Ctrl->UnkeyAllTrackEvents(m_iNewTrack);

	// 현재 트랙에서 현재 시간으로 부터 블랜딩이 끝나는 시간만큼
	m_pAnimation_Ctrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fTimeAcc + 0.25f);
	m_pAnimation_Ctrl->KeyTrackSpeed(m_iCurrentTrack, 1.0f, m_fTimeAcc, 0.25f, D3DXTRANSITION_LINEAR);
	m_pAnimation_Ctrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fTimeAcc, 0.25f, D3DXTRANSITION_LINEAR);

	// 애니메이션을 트랙에 저장한다. 위에 Set에서는 재생을 못하고 Set에 담은 것을 이 트랙으로 가지고와서 애니메이션을 돌려야한다.
	// 만약 Set에 담은 채로 애니메이션을 돌리면 각 애니메이션마다의 연계가 부자연스러워진다. 트랙에서는 딜레이타임을 줘 연계가 자연스러워진다.

	if (FAILED(m_pAnimation_Ctrl->SetTrackEnable(m_iNewTrack, TRUE)))
		return E_FAIL;
	// 트랙을 사용가능하게 만들어준다.

	m_pAnimation_Ctrl->KeyTrackSpeed(m_iNewTrack, 1.0f, m_fTimeAcc, 0.25f, D3DXTRANSITION_LINEAR);
	// 현재 트랙의 속도를 현재시간으로부터 0으로 선형보간한다.

	m_pAnimation_Ctrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fTimeAcc, 0.25f, D3DXTRANSITION_LINEAR);
	// 현재 트랙의 가중치를 현재시간으로부터 0으로 선형보간한다.

	m_pAnimation_Ctrl->SetTrackPosition(m_iNewTrack, 0.0);  // 다른 애니메이션으로 바뀌면 트랙의 위치를 제일 앞으로 옮긴다.
																// 하지않으면 중간에 끝난 그 위치에서 실행이 되어 버린다.
	m_pAnimation_Ctrl->ResetTime();   // 누적시킨 시간을 초기화 시킨다.

	m_fTimeAcc = 0.f;				  // 초기화한다.

	m_iOldAniSetIdx = iAni_idx;       // 애니메이션 갱신

	m_iCurrentTrack = m_iNewTrack;

	m_dwPeriod = pAniSet->GetPeriod();   // 현재애니메이션의 최대시간을 넣어준다

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

	D3DXTRACK_DESC				TrackInfo;   // 트랙의 정보를 담는 공간
											 // Weight, Speed, Position, Enable 등등 존재

	m_pAnimation_Ctrl->GetTrackDesc(m_iCurrentTrack, &TrackInfo);   // 트랙의 정보를 받아온다.

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

	D3DXTRACK_DESC				TrackInfo;   // 트랙의 정보를 담는 공간

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
