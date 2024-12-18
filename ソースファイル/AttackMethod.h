#pragma once
#include "WeaponManager.h"
#include "Camera.h"
#include "GameObj.h"
#include<memory>

class CAttackMethod
{
public:
	CAttackMethod(CGameObj* pParent);
	~CAttackMethod();

	void Init();
	void Fin();
	void Update();
	void AttackMethod();

	CWeaponManager* GetHandWeapon();
	CWeaponManager* GetLongWeapon();
	/*std::weak_ptr<CWeaponManager> GetHandWeapon();
	std::weak_ptr<CWeaponManager> GetLongWeapon();*/
private:
	const enum EWepons {
		WEAPON_MACHINEGUN = 0,
		WEAPON_SWORDATTACK,

		MAX_WEAPON
	};
	CGameObj* m_pParent;
	// 近接武器管理クラスポインタ
	std::shared_ptr<CWeaponManager> m_pHandRangeWeaponManager;
	// 遠距離武器管理クラスポインタ
	std::shared_ptr<CWeaponManager> m_pLongRangeWeaponManager;
	// カメラ
	CCamera* m_pCamera;
};
