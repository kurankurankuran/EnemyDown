// �}���A�i�{�X�j [Enemy_Maria.h]
#pragma once
#include "Enemy.h"

class CEnemy_Maria final : public CEnemy
{
private:
	
protected:

public:
	CEnemy_Maria(CScene* pScene);		// �R���X�g���N�^
	virtual ~CEnemy_Maria();			// �f�X�g���N�^

	virtual HRESULT Init();		// ������
	virtual void Fin();			// �I������
	virtual void Update();		// �X�V
	virtual void Draw();		// �`��

private:
protected:
	void AnimeUpdate();
	void NearAttack();
};
