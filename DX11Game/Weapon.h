// ’ŠÛ•ŠíƒNƒ‰ƒX [Weapon.h]
#pragma once
#include "Model.h"
#include "Land.h"
class CEnemy;
class CWeapon : public CModel
{
private:

protected:
	// ©•ª‚ğŠ‚·‚ée
	CGameObj* m_pParent;
	int m_nDamage;
	CLand* m_pLand;
	CModel* m_pTarget;
	EGameObjTag m_Type;
	float m_fSpeed;
	int m_nEffectCnt;
	std::vector<CGameObj*> m_ColObjVector;
public:
	CWeapon(CScene* pScene);
	~CWeapon();

	virtual HRESULT Init() = 0;
	virtual void Update() = 0;

	virtual int Fire(XMFLOAT4X4* pWorld, EGameObjTag Type, CModel* pEnemy = nullptr) = 0;
	//virtual int Fire() = 0;

	void SetDamage(int nDamage) { m_nDamage = nDamage; }

	void SetTarget(CModel* pTarget) { m_pTarget = pTarget; }
	void SetType(EGameObjTag Type) { m_Type = Type; }

	CModel* GetTarget() { return m_pTarget; }
	EGameObjTag GetType() { return m_Type; }

	bool SerchTarget();
	void SetLand(CLand* pLand) { m_pLand = pLand; }
	void SetParent(CGameObj* Obj) { m_pParent = Obj; }
};
