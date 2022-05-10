// �n�� [Land.h]
#pragma once
#include "Model.h"
#include "octree.h"

// �萔��`
// �d��(�S�L�������ʂȂ�V�[�����̂ɐݒ肷�ׂ�?)
const float GRAVITY_SCALE = 4.5f;


class CLand : public CModel
{
private:
	//CPlayer* m_pPlayer;
	TAssimpVertex* m_pVertex;
	UINT m_nVertex;
	UINT* m_pIndex;
	UINT m_nIndex;

	Octree* pOctree;

public:
	CLand(CScene* pScene);	// �R���X�g���N�^
	virtual ~CLand();		// �f�X�g���N�^

	virtual HRESULT Awake(); // �ŏ��̏�����
	virtual HRESULT Init();	// ������
	virtual void Fin();		// �I������
	virtual void Update();  // �X�V����
	virtual void Draw();    // �`�揈��

	bool Collision(XMFLOAT3 vPos, XMFLOAT3 vP0, XMFLOAT3 vP1, XMFLOAT3* pX, XMFLOAT3* pN = nullptr);
	bool Collision(CGameObj* pObj , XMFLOAT3 vP0, XMFLOAT3 vP1, XMFLOAT3* pX, XMFLOAT3* pN = nullptr);
	//void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
};
