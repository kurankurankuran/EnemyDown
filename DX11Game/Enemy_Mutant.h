// �G�iЭ���āj [Enemy_Mutant.h]
#pragma once
#include "Enemy.h"

class CEnemy_Mutant final : public CEnemy
{
private:
	
protected:
	void AnimeUpdate();
	void NearAttack();
public:
	CEnemy_Mutant(CScene* pScene);		// �R���X�g���N�^
	~CEnemy_Mutant();			        // �f�X�g���N�^

	HRESULT Init();		// ������
	void Fin();			// �I������
	void Update();		// �X�V
	void Draw();		// �`��

private:
	
};
