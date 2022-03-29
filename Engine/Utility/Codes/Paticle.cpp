#include "..\Headers\Paticle.h"

USING(Engine)

CPaticle::CPaticle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_vOrigin(0.f, 0.f, 0.f), m_fEmitRate(0.f), m_fSize(0.f)
	, m_pTexture(nullptr), m_pVertex(nullptr), m_iMaxPaticle(0), dwSize(0), dwOffset(0), dwBatchSize(0)
{
}

CPaticle::CPaticle(const CPaticle & rhs)
	: CComponent(rhs), m_vOrigin(rhs.m_vOrigin), m_boundingbox(rhs.m_boundingbox)
	, m_fEmitRate(rhs.m_fEmitRate), m_fSize(rhs.m_fSize), m_pTexture(rhs.m_pTexture)
	, m_pVertex(rhs.m_pVertex), m_PaticleList(rhs.m_PaticleList), m_iMaxPaticle(rhs.m_iMaxPaticle)
	, dwSize(rhs.dwSize), dwOffset(rhs.dwOffset), dwBatchSize(rhs.dwBatchSize)
{
	m_pTexture->AddRef();
	m_pVertex->AddRef();
}

HRESULT CPaticle::Ready_Paticle(const _tchar* pFilePath)
{
	HRESULT	Hr = 0;


	Hr = m_pGraphicDev->CreateVertexBuffer(dwSize * sizeof(PARTICLE),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &m_pVertex, 0);

	if (FAILED(Hr))
	{
		MSG_BOX("Created Paticle Vertex Buffer Failed!!");
		return E_FAIL;
	}

	Hr = D3DXCreateTextureFromFile(m_pGraphicDev, pFilePath, &m_pTexture);

	if (FAILED(Hr))
	{
		MSG_BOX("Created Paticle Texture Failed!!");
		return E_FAIL;
	}

	return NOERROR;
}

void CPaticle::Paticle_preRender()
{
	// 랜더링하기 이전에 지정해야 할 초기 렌더상태를 지정한다.
	// 시스템에 따라달라질수 있음 이상태가 디폴트 상태	
	// D3D
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, FtoDw(m_fSize));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

	// 거리에 따른 파티클의 크기를 제어한다.
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

	// u텍스쳐의 알파 채널을 이용한다.
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	/////////////////////////////////////////////////////////////////
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);


}
// D3D
void CPaticle::Paticle_Render()
{
	//
	// Remarks:  The render method works by filling a section of the vertex buffer with data,
	//           then we render that section.  While that section is rendering we lock a new
	//           section and begin to fill that section.  Once that sections filled we render it.
	//           This process continues until all the particles have been drawn.  The benifit
	//           of this method is that we keep the video card and the CPU busy. 

	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	if (!m_PaticleList.empty())
	{
		//
		// set render states
		// 랜더 상태를 지정한다.
		//

		Paticle_preRender();

		m_pGraphicDev->SetTexture(0, m_pTexture);
		m_pGraphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		m_pGraphicDev->SetStreamSource(0, m_pVertex, 0, sizeof(PARTICLE));

		//
		// render batches one by one
		//

		// start at beginning if we're at the end of the vb
		// 버텍스 버퍼를  벗어날 경우 처음부터 시작한다.
		if (dwOffset >= dwSize)
			dwOffset = 0;

		PARTICLE*	pVertex = nullptr;

		m_pVertex->Lock(dwOffset * sizeof(PARTICLE), dwBatchSize * sizeof(PARTICLE),
			(void**)&pVertex, dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		//
		// Until all particles have been rendered.
		// 모든 파티클이 랜더링될 떄까지
		//
		list<PATICLEATTRIBUTE>::iterator iter = m_PaticleList.begin();
		list<PATICLEATTRIBUTE>::iterator iter_end = m_PaticleList.end();

		for ( ; iter != iter_end ; ++iter)
		{
			if (iter->bisAlive)
			{
				//
				// Copy a batch of the living particles to the
				// next vertex buffer segment
				// 한 단계의 생존한 파티클을 다음
				// 버텍스 버퍼 세그먼트로 복사한다.
				//
				pVertex->vPosition = iter->vPositon;
				pVertex->vColor = (D3DCOLOR)iter->dwColor;
				++pVertex; // next element;

				++numParticlesInBatch; //increase batch counter
									   // 단계 카운터를 증가시킨다.

									   // if this batch full?
									   // 현재 단계에 모두 채워져 있는가?
										
				if (numParticlesInBatch >= dwBatchSize)
				{
					//
					// Draw the last batch of particles that was
					// copied to the vertex buffer.
					// 버텍스 버퍼로 복사된 마지막 단계의
					// 파티클들을 그린다
					//
					m_pVertex->Unlock();

					m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, dwOffset, dwBatchSize);

					//
					// While that batch is drawing, start filling the
					// next batch with particles.
					// 단계가 채워지는 동안 다음 단계를 파티클로 채운다.
					//

					// move the offset to the start of the next batch
					// 다음 단계의 처음 오프셋으로 이동한다.
					dwOffset += dwBatchSize;

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					// 버텍스 버퍼의 경계를 넘는 메모리로 오프셋을 설정하지 않는다.
					// 경계를 넘을 경우 처음부터 다시한다.
					if (dwOffset >= dwSize)
						dwOffset = 0;

					m_pVertex->Lock(dwOffset * sizeof(PARTICLE), dwBatchSize * sizeof(PARTICLE),
						(void**)&pVertex, dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch  // 다음 단계를 위한 리셋
				}
			}
		}

		m_pVertex->Unlock();

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied.  We draw the last partially filled batch now.
		// 조건이 만족되지 못하여 마지막 단계가 랜더링 되지 않는 경우가 발생할 수  있다.
		// 일부만 채워진 단계는 바로 이곳에서 랜더링한다.

		if (numParticlesInBatch)
			m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, dwOffset, numParticlesInBatch);

		// next block  // 다음블록
		dwOffset += dwBatchSize;

		//
		// reset render states
		//

		Particle_PostRender();
	}
}

void CPaticle::Paticle_Render(LPD3DXEFFECT pEffect, const char * pConstant_Texture_Name)
{
	if (!m_PaticleList.empty())
	{
		//
		// set render states
		// 랜더 상태를 지정한다.
		//

		//Paticle_preRender();

		//m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, FtoDw(m_fSize));
		//m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

		//// 거리에 따른 파티클의 크기를 제어한다.
		//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
		//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
		//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

		pEffect->SetTexture(pConstant_Texture_Name, m_pTexture);

		//m_pGraphicDev->SetTexture(0, m_pTexture);
		m_pGraphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

		m_pGraphicDev->SetStreamSource(0, m_pVertex, 0, sizeof(PARTICLE));

		//
		// render batches one by one
		//

		// start at beginning if we're at the end of the vb
		// 버텍스 버퍼를  벗어날 경우 처음부터 시작한다.
		if (dwOffset >= dwSize)
			dwOffset = 0;

		PARTICLE*	pVertex = nullptr;

		//HRESULT hh = m_pVertex->Lock(dwOffset * sizeof(PARTICLE), dwBatchSize * sizeof(PARTICLE),
		//	(void**)&pVertex, dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		HRESULT hh = m_pVertex->Lock(0, 0, (void**)&pVertex, 0);

		DWORD numParticlesInBatch = 0;

		//
		// Until all particles have been rendered.
		// 모든 파티클이 랜더링될 떄까지
		//
		list<PATICLEATTRIBUTE>::iterator iter = m_PaticleList.begin();
		list<PATICLEATTRIBUTE>::iterator iter_end = m_PaticleList.end();

		for (; iter != iter_end; ++iter)
		{
			if (iter->bisAlive)
			{
				//
				// Copy a batch of the living particles to the
				// next vertex buffer segment
				// 한 단계의 생존한 파티클을 다음
				// 버텍스 버퍼 세그먼트로 복사한다.
				//
				pVertex->vPosition = iter->vPositon;
				pVertex->vColor = (D3DCOLOR)iter->dwColor;
				++pVertex; // next element;

				//_vec4 vPos = _vec4(iter->vPositon, 1.f);

				//pEffect->SetVector("g_vObject_Pos", &vPos);

				++numParticlesInBatch; //increase batch counter
									   // 단계 카운터를 증가시킨다.

									   // if this batch full?
									   // 현재 단계에 모두 채워져 있는가?

				if (numParticlesInBatch >= dwBatchSize)
				{
					//
					// Draw the last batch of particles that was
					// copied to the vertex buffer.
					// 버텍스 버퍼로 복사된 마지막 단계의
					// 파티클들을 그린다
					//
					//m_pVertex->Unlock();

					pEffect->CommitChanges();

					m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, dwOffset, dwBatchSize);

					//
					// While that batch is drawing, start filling the
					// next batch with particles.
					// 단계가 채워지는 동안 다음 단계를 파티클로 채운다.
					//

					// move the offset to the start of the next batch
					// 다음 단계의 처음 오프셋으로 이동한다.
					dwOffset += dwBatchSize;

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					// 버텍스 버퍼의 경계를 넘는 메모리로 오프셋을 설정하지 않는다.
					// 경계를 넘을 경우 처음부터 다시한다.
					if (dwOffset >= dwSize)
						dwOffset = 0;

					//m_pVertex->Lock(dwOffset * sizeof(PARTICLE), dwBatchSize * sizeof(PARTICLE),
					//	(void**)&pVertex, dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch  // 다음 단계를 위한 리셋
				}
			}
		}

		m_pVertex->Unlock();

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied.  We draw the last partially filled batch now.
		// 조건이 만족되지 못하여 마지막 단계가 랜더링 되지 않는 경우가 발생할 수  있다.
		// 일부만 채워진 단계는 바로 이곳에서 랜더링한다.

		if (numParticlesInBatch)
			m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, dwOffset, numParticlesInBatch);

		// next block  // 다음블록
		dwOffset += dwBatchSize;

		//
		// reset render states
		//

		//Particle_PostRender();
	}
}

void CPaticle::Particle_PostRender()
{
	// 디폴트상태
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

bool CPaticle::isEmpty()
{
	return m_PaticleList.empty();
}

bool CPaticle::isDead()
{
	list<PATICLEATTRIBUTE>::iterator iter = m_PaticleList.begin();
	list<PATICLEATTRIBUTE>::iterator iter_end = m_PaticleList.end();

	for ( ; iter != iter_end ; ++iter)
	{
		// is there at least one living particle?  If yes,
		// the system is not dead.
		if (iter->bisAlive)
			return false;
	}
	// no living particles found, the system must be dead.
	return true;
}


void CPaticle::All_IsDead()
{
	list<PATICLEATTRIBUTE>::iterator iter = m_PaticleList.begin();
	list<PATICLEATTRIBUTE>::iterator iter_end = m_PaticleList.end();

	for (; iter != iter_end; ++iter)
	{
		iter->bisAlive = false;
	}
}

void CPaticle::Remove_DeadPaticle()
{
	// 리스트에서 죽은 파티클을 리스트에서 제거한다.

	list<PATICLEATTRIBUTE>::iterator iter = m_PaticleList.begin();
	list<PATICLEATTRIBUTE>::iterator iter_end = m_PaticleList.end();

	while ( iter != iter_end)
	{
		if (iter->bisAlive == false)
		{
			// erase returns the next iterator, so no need to
			// incrememnt to the next one ourselves.
			iter = m_PaticleList.erase(iter);
		}
		else
		{
			iter++; // next in list
		}
	}
}

_ulong CPaticle::Free(void)
{
	_ulong dwRefCnt = 0;

	All_IsDead();
	Remove_DeadPaticle();
	m_PaticleList.clear();

	dwRefCnt = Engine::Safe_Release(m_pVertex);

	dwRefCnt = Engine::Safe_Release(m_pTexture);

	return CComponent::Free();
}
