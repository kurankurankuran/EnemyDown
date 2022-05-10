// 武器管理抽象管理クラス [WeaponManager.h]
#pragma once

#include "Weapon.h"
#include <vector>
using namespace std;

struct TParts;
class CWeaponManager
{
private:
	
	
protected:
	// 現在のシーン
	CScene* m_pScene;
	// 自分を所持する親
	CGameObj* m_pParent;
	// 親ワールド行列
	XMFLOAT4X4* m_pParentWorld;
	// 可視判定
	bool      m_bVisible;
	// １フレーム前の可視判定
	bool      m_bOldVisible;
	
	// 武器実態ポインタ(実弾)
	vector<CWeapon*> m_pWeapon; 
	// 装填数
	int m_nMagazine; 
	// 次装填可能までのカウント
	int m_nReloadCnt;
	// 装填可能フラグ
	bool m_bCreate;
	// プレイヤーのものか敵のものか
	EGameObjTag m_eType;

	int m_nChangeNum;
	int m_nChangeCnt;
public:
	CWeaponManager(CScene* pScene);
	~CWeaponManager();

	virtual HRESULT Init(XMFLOAT4X4*) = 0;
	virtual void Fin() = 0;
	virtual void Update() = 0;

	virtual int Fire(int nDamage, int nTotalTime = 0,int nDelay = 0) = 0;
	
	virtual int Create(EGameObjTag Type, CModel* pEnemy = nullptr) = 0;
	void AllDelete();

	void SetWorld(int i, XMFLOAT4X4& pWorld);

	int GetWeaponSize() { return m_pWeapon.size(); }

	void SetParent(CGameObj* Obj) { m_pParent = Obj; }

	bool GetChangeFlg();

	int GetMagazineNum() { return m_nMagazine; }

	bool GetCreateFlg() { return m_bCreate; }

	virtual void SetVisible(bool bVisible) { m_bVisible = bVisible; }
};

