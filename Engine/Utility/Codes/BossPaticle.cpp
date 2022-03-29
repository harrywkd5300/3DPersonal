#include "..\Headers\BossPaticle.h"

#include <time.h>

USING(Engine)

CBossPaticle::CBossPaticle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPaticle(pGraphicDev)
	, m_numPaticle(0)
{
}

CBossPaticle::CBossPaticle(const CBossPaticle & rhs)
	: CPaticle(rhs)
	, m_numPaticle(rhs.m_numPaticle)
	, m_vDir(rhs.m_vDir)
{
}

HRESULT CBossPaticle::Ready_SnowPaticle(const _tchar * pFilePath, _uint numPaticle)
{
	m_fSize = 0.2f;
	dwSize = numPaticle;
	dwOffset = 0;
	dwBatchSize = 512;

	m_numPaticle = numPaticle;

	if (FAILED(CPaticle::Ready_Paticle(pFilePath)))
		return E_FAIL;

	return NOERROR;
}

void CBossPaticle::Add_Paticle()
{
	PATICLEATTRIBUTE		attribute;
	ZeroMemory(&attribute, sizeof(PATICLEATTRIBUTE));

	Reset_Paticle(&attribute);

	m_PaticleList.push_back(attribute);
}

void CBossPaticle::Reset()
{
	list<PATICLEATTRIBUTE>::iterator iter = m_PaticleList.begin();
	list<PATICLEATTRIBUTE>::iterator iter_end = m_PaticleList.end();

	for (; iter != iter_end; ++iter)
	{
		Reset_Paticle(&(*iter));
	}
}

void CBossPaticle::Reset_Paticle(PATICLEATTRIBUTE * attribute)
{
	attribute->fLifeTime = static_cast<float>(rand() % 3);
	attribute->fAge = 0.f;

	attribute->bisAlive = true;

	attribute->vPositon = m_vOrigin;

	if (m_bBossStage)
	{
		_vec3 vMin = _vec3(-0.5f, -0.5f, -0.5f);
		_vec3 vMax = _vec3(0.5f, 0.5f, 0.5f);

		GetRandomVector(&attribute->vPositon, &(attribute->vPositon + vMin), &(attribute->vPositon + vMax));

		attribute->vVelocity = m_vDir;
	}
	else
	{
		_vec3 vMin = _vec3(-0.5f, -0.5f, -0.5f);
		_vec3 vMax = _vec3(0.5f, 0.5f, 0.5f);

		GetRandomVector(&attribute->vVelocity, &vMin, &vMax);

		//_float vCheckX = attribute->vVelocity.x * m_vDir.x;
		//if (vCheckX > 0)
		//	attribute->vVelocity.x *= -1.f;

		if (attribute->vVelocity.y > 0)
			attribute->vVelocity.y *= -1.f;

		//_float vCheckZ = attribute->vVelocity.z * m_vDir.z;
		//if (vCheckZ > 0)
		//	attribute->vVelocity.z *= -1.f;
	}

	D3DXVec3Normalize(&attribute->vVelocity, &attribute->vVelocity);

	_float fSpeed = _float(rand() % 5);

	attribute->vVelocity *= fSpeed;

	attribute->dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
}

void CBossPaticle::Paticle_Update(const _float & fTimeDelta)
{
	list<PATICLEATTRIBUTE>::iterator iter = m_PaticleList.begin();
	list<PATICLEATTRIBUTE>::iterator iter_end = m_PaticleList.end();

	for (; iter != iter_end; ++iter)
	{
		iter->vPositon += iter->vVelocity * fTimeDelta;
		iter->fAge += fTimeDelta;
		// is the point outside bounds?
		// 포인트의 경계를 벗어나는가?
		if (iter->fAge >= iter->fLifeTime)
		{
			//if(!m_boundingbox.isPointInside(iter->vPositon))
			//{
			// nope so kill it, but we want to recycle dead 
			// particles, so respawn it instead.
			// 경계를 벗어난 파티클을 재활용한다.
			Reset_Paticle(&(*iter));
		}
	}
}

void CBossPaticle::Set_OriginPos(_vec3 * vPos)
{
	m_vOrigin = *vPos;
	//m_vOrigin.y += 5.f;
}

void CBossPaticle::Set_Paticle()
{
	for (size_t i = 0; i < m_numPaticle; ++i)
		Add_Paticle();
}

void CBossPaticle::Set_Ditection(_vec3 * vDir)
{
	m_vDir = *vDir;
}

CComponent * CBossPaticle::Clone(void)
{
	return new CBossPaticle(*this);
}

CBossPaticle * CBossPaticle::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, _uint numPaticle)
{
	CBossPaticle *	pInstance = new CBossPaticle(pGraphicDev);

	if (FAILED(pInstance->Ready_SnowPaticle(pFilePath, numPaticle)))
	{
		MSG_BOX("CBossPaticle Created Failed!!");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CBossPaticle::Free(void)
{
	_ulong dwRef = 0;

	dwRef = CPaticle::Free();

	return dwRef;
}
