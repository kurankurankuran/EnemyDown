// マシンガン管理クラス [MGManager.h]
#pragma once
#include "WeaponManager.h"
#include "Machinegun.h"
#include <vector>

#include "Player.h"
#include "Land.h"

using namespace std;
namespace {
	const int MAX_MACHINEGUN = 240;
}

//struct TParts;

class CMGManager :public CWeaponManager
{
private:
	bool                      m_bDelFireRate;           // 発射レート減少フラグ
	float                     m_fFireRate;
	//ID3D11ShaderResourceView* m_pTex;
	CLand*                    m_pLand;
	int                       m_nEffekseerHandle;       // 発射光源エフェクシアのハンドル
	XMFLOAT3                  m_vGeneratePos;           // 光源位置
	int                       m_nChargeCnt;             // MP自動回復カウント
public:
	CMGManager(CScene* pScene);
	~CMGManager();

	virtual HRESULT Init(XMFLOAT4X4*);
	virtual void Fin();
	virtual void Update();

	virtual int Fire(int nDamage, int nTotalTime = 0, int nDelay = 0);
	//virtual int FireEnemy();
	virtual int Create( EGameObjTag Type, CModel* pEnemy = nullptr);
	


};

//TPartsInfo* GetMachinegunInitInfo();
