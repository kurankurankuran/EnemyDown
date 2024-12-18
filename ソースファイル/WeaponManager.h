// ����Ǘ����ۊǗ��N���X [WeaponManager.h]
#pragma once

#include "Weapon.h"
#include <vector>
using namespace std;

struct TParts;
class CWeaponManager
{
private:
	
	
protected:
	// ���݂̃V�[��
	CScene* m_pScene;
	// ��������������e
	CGameObj* m_pParent;
	// �e���[���h�s��
	XMFLOAT4X4* m_pParentWorld;
	// ������
	bool      m_bVisible;
	// �P�t���[���O�̉�����
	bool      m_bOldVisible;
	
	// ������ԃ|�C���^(���e)
	vector<CWeapon*> m_pWeapon; 
	// ���U��
	int m_nMagazine; 
	// �����U�\�܂ł̃J�E���g
	int m_nReloadCnt;
	// ���U�\�t���O
	bool m_bCreate;
	// �v���C���[�̂��̂��G�̂��̂�
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

