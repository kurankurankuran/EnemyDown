// ���U���Ǘ��N���X [SwordAttackManager.h]
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
	int  m_nTime;      // ����������
	int  m_nTimeCnt;   // �U������o�����ԂȂǂ��Ǘ����邽�߂̃J�E���g
	int  m_nStartTime; // �U������o������(�����J�n�ォ��̌o�ߎ���)
	bool m_bAttackflg; // �U�����Ȃ�True
	XMFLOAT3 m_vPos;   // ���[�J���ʒu
	XMFLOAT3 m_vAngle;   // ���[�J����]��
	XMFLOAT3 m_vScale;   // ���[�J���g�k
	//static std::vector<std::string> m_strBoneName;
	const char*    m_strBoneName;
	int m_nDamage;
	LPCWSTR m_TexName; // �U���̋O�ՂɎg�p����e�N�X�`���̖��O
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

	void SetCurSwordPos(XMFLOAT3& head, XMFLOAT3& tail);  //���݃t���[���̌��̈ʒu��ۑ�����
};

//TPartsInfo* GetSwordAttackInitInfo();