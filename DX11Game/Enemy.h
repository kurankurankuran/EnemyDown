// 敵機 [Enemy.h]
#pragma once
#include "Model.h"
//#include "Effect.h"
#include "Land.h"
#include <vector>
#include "WeaponManager.h"
#include<map>
class CPlayer;
class CEnemy : public CModel
{
private:
protected:
	enum EBaseAnimation
	{
		IDLE_BA = 0,
		WALK_BA,
		DYING_BA,
		HITBACK_BA,
	};

protected:
	CGameObj* m_pTarget;
	int m_nEffectCnt;
	int m_nFireCnt;
	// 地形
	CLand* m_pLand;
	// 敵生成時エフェクトで使用変数
	int m_nEfkID;
	int m_nCreateEffectTimer;
	// 武器管理クラスポインタ
	CWeaponManager* m_pWeaponManager;
	float m_fAtkRad; // 攻撃範囲半径
	float m_fSeachRad;// 作的範囲
	std::map<EBaseAnimation, int> m_empEBaseAnimationToInt;

protected:
	virtual void AnimeUpdate() = 0;
	virtual void NearAttack() = 0;  // 近距離攻撃
public:
	CEnemy(CScene* pScene);		// コンストラクタ
	virtual ~CEnemy();			// デストラクタ

	virtual HRESULT Init();		// 初期化
	virtual void Fin();			// 終了処理
	virtual void Update();		// 更新
	virtual void Draw();		// 描画
	virtual void DrawUI();		// UI描画

	

	//void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetTarget(CGameObj* pTarget) { m_pTarget = pTarget; }
	//void SetLand(CLand* pLand) { m_pLand = pLand; }

	bool BS(XMFLOAT3, XMFLOAT3, float, float);

};
