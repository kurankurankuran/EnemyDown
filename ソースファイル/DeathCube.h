// �G�ꂽ�玀�ʃL���[�u [Deth.h]
#pragma once
#include "Model.h"
#include "Player.h"
class CDeathCube : public CModel
{
private:
	//CPlayer* m_pPlayer;
public:
	CDeathCube(CScene* pScene);	// �R���X�g���N�^
	virtual ~CDeathCube();		// �f�X�g���N�^

	virtual HRESULT Init();	// ������
	//virtual void Fin();		// �I������
	virtual void Update();  // �X�V����
	virtual void Draw();    // �`�揈��

	//virtual bool CollisionOBB(CGameObj* pOther); // OBB�����蔻��

	//void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
};
