#include "..\Headers\SnowPaticle.h"

USING(Engine)

CSnowPaticle::CSnowPaticle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPaticle(pGraphicDev)
	, m_numPaticle(0)
{
}

CSnowPaticle::CSnowPaticle(const CSnowPaticle & rhs)
	: CPaticle(rhs)
	, m_numPaticle(rhs.m_numPaticle)
{
}

HRESULT CSnowPaticle::Ready_SnowPaticle(const _tchar * pFilePath, _uint numPaticle)
{
	m_fSize		 = 0.3f;
	dwSize		 = numPaticle;
	dwOffset	 = 0;
	dwBatchSize	 = 512;

	m_numPaticle = numPaticle;

	if (FAILED(CPaticle::Ready_Paticle(pFilePath)))
		return E_FAIL;

	return NOERROR;
}

void CSnowPaticle::Add_Paticle()
{
	PATICLEATTRIBUTE		attribute;
	ZeroMemory(&attribute, sizeof(PATICLEATTRIBUTE));

	Reset_Paticle(&attribute);

	m_PaticleList.push_back(attribute);
}

void CSnowPaticle::Reset()
{
	list<PATICLEATTRIBUTE>::iterator iter = m_PaticleList.begin();
	list<PATICLEATTRIBUTE>::iterator iter_end = m_PaticleList.end();

	for (; iter != iter_end; ++iter)
	{
		Reset_Paticle(&(*iter));
	}
}

void CSnowPaticle::Reset_Paticle(PATICLEATTRIBUTE * attribute)
{
	attribute->fLifeTime = static_cast<float>(rand() % 6 + 5);
	attribute->fAge = 0.f;

	attribute->bisAlive = true;

	// 눈송이의 위치를 정하기 위하여 임의의 좌표를 얻는다
	GetRandomVector(&attribute->vPositon, &m_boundingbox.vMin, &m_boundingbox.vMax);

	// 높이 y는 항상 경계 상자의 최상단이 된다.
	attribute->vPositon.y = m_boundingbox.vMax.y;

	_float fXRd = _float(rand()%5);

	// 눈송이는 아래쪽으로 떨어지며 약간 왼쪽을 향한다.
	attribute->vVelocity.x = GetRandomFloat(0.0f, 1.0f) * -fXRd;
	attribute->vVelocity.y = (GetRandomFloat(0.0f, 1.0f) * -6.0f) - 3.f;
	attribute->vVelocity.z = 0.0f;

	// 흰색의 눈송이
	attribute->dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
}

void CSnowPaticle::Paticle_Update(const _float& fTimeDelta)
{
	list<PATICLEATTRIBUTE>::iterator iter = m_PaticleList.begin();
	list<PATICLEATTRIBUTE>::iterator iter_end = m_PaticleList.end();

	for ( ; iter != iter_end ; ++iter)
	{
		iter->vPositon += iter->vVelocity * fTimeDelta;
		iter->fAge += fTimeDelta;

		// 포인트의 경계를 벗어나는가?
		if (iter->fAge >= iter->fLifeTime)
		{
			// 경계를 벗어난 파티클을 재활용한다.
			Reset_Paticle(&(*iter));
		}
	}
}

void CSnowPaticle::Set_BoundingBox(_vec3 * vMin, _vec3 * vMax)
{
	m_boundingbox.vMin = *vMin;
	m_boundingbox.vMax = *vMax;
}

void CSnowPaticle::Set_Paticle()
{
	for (size_t i = 0; i < m_numPaticle; i++)
		Add_Paticle();
}

CComponent * CSnowPaticle::Clone(void)
{
	return new CSnowPaticle(*this);
}

CSnowPaticle * CSnowPaticle::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, _uint numPaticle)
{
	CSnowPaticle *	pInstance = new CSnowPaticle(pGraphicDev);

	if (FAILED(pInstance->Ready_SnowPaticle(pFilePath, numPaticle)))
	{
		MSG_BOX("CSnowPaticle Created Failed!!");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CSnowPaticle::Free(void)
{
	return CPaticle::Free();
}
