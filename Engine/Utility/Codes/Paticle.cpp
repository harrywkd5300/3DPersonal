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
	// �������ϱ� ������ �����ؾ� �� �ʱ� �������¸� �����Ѵ�.
	// �ý��ۿ� ����޶����� ���� �̻��°� ����Ʈ ����	
	// D3D
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, FtoDw(m_fSize));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

	// �Ÿ��� ���� ��ƼŬ�� ũ�⸦ �����Ѵ�.
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

	// u�ؽ����� ���� ä���� �̿��Ѵ�.
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
		// ���� ���¸� �����Ѵ�.
		//

		Paticle_preRender();

		m_pGraphicDev->SetTexture(0, m_pTexture);
		m_pGraphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		m_pGraphicDev->SetStreamSource(0, m_pVertex, 0, sizeof(PARTICLE));

		//
		// render batches one by one
		//

		// start at beginning if we're at the end of the vb
		// ���ؽ� ���۸�  ��� ��� ó������ �����Ѵ�.
		if (dwOffset >= dwSize)
			dwOffset = 0;

		PARTICLE*	pVertex = nullptr;

		m_pVertex->Lock(dwOffset * sizeof(PARTICLE), dwBatchSize * sizeof(PARTICLE),
			(void**)&pVertex, dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		//
		// Until all particles have been rendered.
		// ��� ��ƼŬ�� �������� ������
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
				// �� �ܰ��� ������ ��ƼŬ�� ����
				// ���ؽ� ���� ���׸�Ʈ�� �����Ѵ�.
				//
				pVertex->vPosition = iter->vPositon;
				pVertex->vColor = (D3DCOLOR)iter->dwColor;
				++pVertex; // next element;

				++numParticlesInBatch; //increase batch counter
									   // �ܰ� ī���͸� ������Ų��.

									   // if this batch full?
									   // ���� �ܰ迡 ��� ä���� �ִ°�?
										
				if (numParticlesInBatch >= dwBatchSize)
				{
					//
					// Draw the last batch of particles that was
					// copied to the vertex buffer.
					// ���ؽ� ���۷� ����� ������ �ܰ���
					// ��ƼŬ���� �׸���
					//
					m_pVertex->Unlock();

					m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, dwOffset, dwBatchSize);

					//
					// While that batch is drawing, start filling the
					// next batch with particles.
					// �ܰ谡 ä������ ���� ���� �ܰ踦 ��ƼŬ�� ä���.
					//

					// move the offset to the start of the next batch
					// ���� �ܰ��� ó�� ���������� �̵��Ѵ�.
					dwOffset += dwBatchSize;

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					// ���ؽ� ������ ��踦 �Ѵ� �޸𸮷� �������� �������� �ʴ´�.
					// ��踦 ���� ��� ó������ �ٽ��Ѵ�.
					if (dwOffset >= dwSize)
						dwOffset = 0;

					m_pVertex->Lock(dwOffset * sizeof(PARTICLE), dwBatchSize * sizeof(PARTICLE),
						(void**)&pVertex, dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch  // ���� �ܰ踦 ���� ����
				}
			}
		}

		m_pVertex->Unlock();

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied.  We draw the last partially filled batch now.
		// ������ �������� ���Ͽ� ������ �ܰ谡 ������ ���� �ʴ� ��찡 �߻��� ��  �ִ�.
		// �Ϻθ� ä���� �ܰ�� �ٷ� �̰����� �������Ѵ�.

		if (numParticlesInBatch)
			m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, dwOffset, numParticlesInBatch);

		// next block  // �������
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
		// ���� ���¸� �����Ѵ�.
		//

		//Paticle_preRender();

		//m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, FtoDw(m_fSize));
		//m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

		//// �Ÿ��� ���� ��ƼŬ�� ũ�⸦ �����Ѵ�.
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
		// ���ؽ� ���۸�  ��� ��� ó������ �����Ѵ�.
		if (dwOffset >= dwSize)
			dwOffset = 0;

		PARTICLE*	pVertex = nullptr;

		//HRESULT hh = m_pVertex->Lock(dwOffset * sizeof(PARTICLE), dwBatchSize * sizeof(PARTICLE),
		//	(void**)&pVertex, dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		HRESULT hh = m_pVertex->Lock(0, 0, (void**)&pVertex, 0);

		DWORD numParticlesInBatch = 0;

		//
		// Until all particles have been rendered.
		// ��� ��ƼŬ�� �������� ������
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
				// �� �ܰ��� ������ ��ƼŬ�� ����
				// ���ؽ� ���� ���׸�Ʈ�� �����Ѵ�.
				//
				pVertex->vPosition = iter->vPositon;
				pVertex->vColor = (D3DCOLOR)iter->dwColor;
				++pVertex; // next element;

				//_vec4 vPos = _vec4(iter->vPositon, 1.f);

				//pEffect->SetVector("g_vObject_Pos", &vPos);

				++numParticlesInBatch; //increase batch counter
									   // �ܰ� ī���͸� ������Ų��.

									   // if this batch full?
									   // ���� �ܰ迡 ��� ä���� �ִ°�?

				if (numParticlesInBatch >= dwBatchSize)
				{
					//
					// Draw the last batch of particles that was
					// copied to the vertex buffer.
					// ���ؽ� ���۷� ����� ������ �ܰ���
					// ��ƼŬ���� �׸���
					//
					//m_pVertex->Unlock();

					pEffect->CommitChanges();

					m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, dwOffset, dwBatchSize);

					//
					// While that batch is drawing, start filling the
					// next batch with particles.
					// �ܰ谡 ä������ ���� ���� �ܰ踦 ��ƼŬ�� ä���.
					//

					// move the offset to the start of the next batch
					// ���� �ܰ��� ó�� ���������� �̵��Ѵ�.
					dwOffset += dwBatchSize;

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					// ���ؽ� ������ ��踦 �Ѵ� �޸𸮷� �������� �������� �ʴ´�.
					// ��踦 ���� ��� ó������ �ٽ��Ѵ�.
					if (dwOffset >= dwSize)
						dwOffset = 0;

					//m_pVertex->Lock(dwOffset * sizeof(PARTICLE), dwBatchSize * sizeof(PARTICLE),
					//	(void**)&pVertex, dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch  // ���� �ܰ踦 ���� ����
				}
			}
		}

		m_pVertex->Unlock();

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied.  We draw the last partially filled batch now.
		// ������ �������� ���Ͽ� ������ �ܰ谡 ������ ���� �ʴ� ��찡 �߻��� ��  �ִ�.
		// �Ϻθ� ä���� �ܰ�� �ٷ� �̰����� �������Ѵ�.

		if (numParticlesInBatch)
			m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, dwOffset, numParticlesInBatch);

		// next block  // �������
		dwOffset += dwBatchSize;

		//
		// reset render states
		//

		//Particle_PostRender();
	}
}

void CPaticle::Particle_PostRender()
{
	// ����Ʈ����
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
	// ����Ʈ���� ���� ��ƼŬ�� ����Ʈ���� �����Ѵ�.

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
