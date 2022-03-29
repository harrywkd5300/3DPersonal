#pragma once

#include "Defines.h"
#include "Scene.h"
#include "Wave_Manager.h"
#include "CollisionMgr.h"

class CScene_Stage : public Engine::CScene
{
private:
	explicit CScene_Stage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Stage() = default;
public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;
private:
	HRESULT	Ready_LightInfo();
	HRESULT	Ready_Load_Object();
	HRESULT	Ready_Layer_Object(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_LeftDoor(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_RightDoor(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Boss(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Snowman(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Santa(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Snow(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_TEST(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_UI(const _tchar* pLayerTag);

private:
	HRESULT	Change_Scene();

private:
	HRESULT	Ready_Stage1();
	HRESULT	Ready_Stage2();
	HRESULT	Ready_Stage3();
	HRESULT	Ready_Snowman();
	HRESULT	Move_SantaTime();

	HRESULT	Add_BGM_ForStage();

	void	End_MakeSnowman();
	void	End_MakeBoss();

private:
	vector<OBJECT*>			m_vecObject;
	_bool					m_bChange = false;

	CWave_Manager*			m_pWave_Manager = nullptr;
	CCollisionMgr*			m_pColl_Manager = nullptr;

	_float					m_fTimeDeta = 0.f;

	_float					m_fClickTime = 0.f;
	_bool					m_bClick = true;
	_bool					m_bCheckClick = false;

	_bool					m_bStage1 = false;
	_bool					m_bStage2 = false;
	_bool					m_bStage3 = false;

	_bool					m_bWaveStart = true;

	//Wave
	// Mon1 리젠타임
	_float					m_fRegenTime0 = 0.f;
	_float					m_fRegenTime1 = 0.f;
	_float					m_fRegenTime2 = 0.f;
	_float					m_fRegenTime3 = 0.f;
	_float					m_fRegenTime4 = 0.f;
	_float					m_fRegenTime5 = 0.f;

	// 위치 뱔 몬스터 갯수
	_float					m_fMonCntPA1 = 0.f;
	_float					m_fMonCntPB1 = 0.f;
	_float					m_fMonCntPC1 = 0.f;

	_float					m_fMonCntPA2 = 0.f;
	_float					m_fMonCntPB2 = 0.f;
	_float					m_fMonCntPC2 = 0.f;

	// 스노우맨 등장
	_bool					m_bMakeEnd = false;
	_bool					m_bStartMake = false;
	_bool					m_bChangeCam = false;

	// 보스 
	_bool					m_bBossStart = false;
private:

public:
	void	Change_BoolScene() {
		m_bChange = true;
	}
	void	Set_EndSnowman() { m_bStartMake = false; }
	void	Set_EndBoss() { m_bBossStart = false; }
	void	Set_ReChangeCam() { m_bChangeCam = false; }

public:
	static CScene_Stage* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual _ulong Free(void) override;
};

