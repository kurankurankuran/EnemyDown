// �G�i���@�g���j [Enemy_Wizard.h]
#pragma once
#include "Enemy.h"

class CEnemy_Wizard final : public CEnemy
{
private:
	int m_nAttackCnt;// �U���^�C�~���O�����p

protected:
	
public:
	CEnemy_Wizard(CScene* pScene);		// �R���X�g���N�^
	virtual ~CEnemy_Wizard();			// �f�X�g���N�^

	virtual HRESULT Init();		// ������
	virtual void Fin();			// �I������
	virtual void Update();		// �X�V
	virtual void Draw();		// �`��

private:
	
protected:
	void AnimeUpdate();
	void NearAttack();
};
