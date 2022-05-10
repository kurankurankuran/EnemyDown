// ���U�� [Sword.h]
#pragma once
#include "Weapon.h"
#include "Mesh.h"





class CEnemy;
class CSwordAttack : public CWeapon
{
private:
	CGameObj* m_pParent;
	std::string m_strBoneName;

	// �O�Օ`��p�e�N�X�`��
	LPCWSTR m_TexName;
	// �O�Օ`��p���b�V��
	CMesh m_Mesh;
	//���̈ʒu��ۑ�����\����
	struct PosBuffer
	{
		XMFLOAT3 headPos = { 0,0,0 }; //���̐�[�̈ʒu
		XMFLOAT3 tailPos = { 0,0,0 }; //���̖��[�̈ʒu
		bool isUsed = false;
	};

	std::vector<PosBuffer> posArray;             //���̈ʒu��ۑ�����o�b�t�@
	std::vector<VERTEX_3D> vertex;               //���_�o�b�t�@
	std::vector<int> index;               //�C���f�b�N�X�o�b�t�@
	PosBuffer tempPos;                           //���݃t���[���ł̌��̈ʒu

	float m_fLongScale;                   // �U���̋O�Ղ̎Q�l�ɂ��钷��
public:
	CSwordAttack(CScene* pScene);
	~CSwordAttack();

	virtual HRESULT Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();

	int Fire(XMFLOAT4X4* pWorld, EGameObjTag Type, CModel* pEnemy = nullptr);

	//int Fire();

	void SetParentObj(CGameObj* pGameObj) { m_pParent = pGameObj; }
	void SetBoneName(std::string strBoneName) { m_strBoneName = strBoneName; }
	void SetTexture(LPCWSTR pName) { m_TexName = pName; }
	void CreateCurveVertex(std::vector<PosBuffer>& usedPosArray);
	
	void SetTraceScale(XMFLOAT3 vScale);

private:
	void MakeTraces();
};
