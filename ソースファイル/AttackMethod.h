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
	// �ߐڕ���Ǘ��N���X�|�C���^
	std::shared_ptr<CWeaponManager> m_pHandRangeWeaponManager;
	// ����������Ǘ��N���X�|�C���^
	std::shared_ptr<CWeaponManager> m_pLongRangeWeaponManager;
	// �J����
	CCamera* m_pCamera;
};
