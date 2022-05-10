//=============================================================================
//
// �G�l�~�[�E�F�[�u�ݒ� �N���X��` [EW_Setting_Base.h]
// Author : KAZUKI KURA
//
//=============================================================================

#pragma once
#include<vector>
#include<map>
#include "Model.h"
class CEWSetting_Base
{
public:
	CEWSetting_Base();
	~CEWSetting_Base();

	void Init();
	void Fin();
	void Update();
	//void Draw();

	
private:
	enum ETextIndex {
		TotalWaveNum,
		CurWaveNum,
		EnemyNum,
		Type,
		EnemyPos,
	};
	
	void UpdateTotalWaveNum();
	void UpdateCurWaveNum();
	
private:
	unsigned int m_nTotalWaveNum;              // ���E�F�[�u��
	unsigned int m_nCurWaveNum;                // ���ݐݒ蒆�̃E�F�[�u�ԍ�
	unsigned int m_nEnemyNum;                  // �E�F�[�u���̓G�̐�
	EModelType m_Type;                         // ��������G�̃��f���^�C�v
	XMFLOAT3 m_vPos;                           // ��������G�̈ʒu
	std::map<ETextIndex, int> m_mapTextIndex;  // �\�����Ă���e�L�X�g�̊Ǘ��p�C���f�b�N�X
};
