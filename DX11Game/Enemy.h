// �G�@ [Enemy.h]
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
	// �n�`
	CLand* m_pLand;
	// �G�������G�t�F�N�g�Ŏg�p�ϐ�
	int m_nEfkID;
	int m_nCreateEffectTimer;
	// ����Ǘ��N���X�|�C���^
	CWeaponManager* m_pWeaponManager;
	float m_fAtkRad; // �U���͈͔��a
	float m_fSeachRad;// ��I�͈�
	std::map<EBaseAnimation, int> m_empEBaseAnimationToInt;

protected:
	virtual void AnimeUpdate() = 0;
	virtual void NearAttack() = 0;  // �ߋ����U��
public:
	CEnemy(CScene* pScene);		// �R���X�g���N�^
	virtual ~CEnemy();			// �f�X�g���N�^

	virtual HRESULT Init();		// ������
	virtual void Fin();			// �I������
	virtual void Update();		// �X�V
	virtual void Draw();		// �`��
	virtual void DrawUI();		// UI�`��

	

	//void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetTarget(CGameObj* pTarget) { m_pTarget = pTarget; }
	//void SetLand(CLand* pLand) { m_pLand = pLand; }

	bool BS(XMFLOAT3, XMFLOAT3, float, float);

};
