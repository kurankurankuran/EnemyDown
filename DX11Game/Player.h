// �v���C���[ [Player.h]
#pragma once
#include <vector>
#include "Model.h"
#include "WeaponManager.h"
#include "Land.h"
#include "AttackMethod.h"
#include<memory>


enum EParts {
	PARTS_BODY = 0,
	PARTS_CANON,


	MAX_PARTS

};



struct TPartsInfo {
	int nModel;
	int nParent;
	XMFLOAT3 vPos;
	XMFLOAT3 vAngle;
	XMFLOAT3 vScl;
	bool bUnit;
};

class CWeaponManager;
class CPlayer : public CModel
{
public:
	
private:
	// ����
	std::unique_ptr<CAttackMethod> m_uqpAttackMethod;
	// �_�b�V���\��
	int m_nDashNum;
	// �_�b�V���ړ���
	float m_fDashVol;
	// �n�`
	CLand* m_pLand;
	// �J����
	CCamera* m_pCamera;


	int m_nTimeCnt;   // ��ʘg��Ԃ�����p�i���j
	int m_nPlusCnt;   // ��ʘg��Ԃ�����p�i���j

public:
	CPlayer(CScene* pScene); // �R���X�g���N�^
	virtual ~CPlayer();		 // �f�X�g���N�^
	virtual HRESULT Init();	 // ������
	virtual void Fin();		 // �I������
	virtual void Update();	 // �X�V
	virtual void Draw();	 // �`��
	virtual void DrawUI();	 // UI�`��

	//void ChangeWeapon();
	//XMFLOAT4X4 RotationModel(XMFLOAT3 vMove);
	void SetVisible(bool bVisible = true);

private:
	void AttackMethod(XMFLOAT4X4& mW);
	void MouseMove(XMFLOAT3& vMove, XMFLOAT4X4& mW);
	
	void DrawCurrentState(CharaState state);
	void AnimationUpdate();
};
