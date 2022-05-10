// 剣攻撃管理クラス [SwordAttackManager.h]
#pragma once
#include "WeaponManager.h"
#include "SwordAttack.h"
#include <vector>
#include "Player.h"
using namespace std;
namespace {
	const int MAX_SWORDATTACK = 1;
}

class CSwordAttackManager :public CWeaponManager
{
private:
	int  m_nTime;      // 総処理時間
	int  m_nTimeCnt;   // 攻撃判定出現時間などを管理するためのカウント
	int  m_nStartTime; // 攻撃判定出現時間(処理開始後からの経過時間)
	bool m_bAttackflg; // 攻撃中ならTrue
	XMFLOAT3 m_vPos;   // ローカル位置
	XMFLOAT3 m_vAngle;   // ローカル回転量
	XMFLOAT3 m_vScale;   // ローカル拡縮
	//static std::vector<std::string> m_strBoneName;
	const char*    m_strBoneName;
	int m_nDamage;
	LPCWSTR m_TexName; // 攻撃の軌跡に使用するテクスチャの名前
public:
	CSwordAttackManager(CScene* pScene);
	~CSwordAttackManager();

	virtual HRESULT Init(XMFLOAT4X4*);
	virtual void Fin();
	virtual void Update();

	virtual int Fire(int nDamage, int nTotalTime = 0, int nDelay = 0);
	//virtual int FireEnemy();
	virtual int Create( EGameObjTag Type, CModel* pEnemy = nullptr);

	virtual void SetVisible(bool bVisible);

	void SetCurSwordPos(XMFLOAT3& head, XMFLOAT3& tail);  //現在フレームの剣の位置を保存する
};

//TPartsInfo* GetSwordAttackInitInfo();