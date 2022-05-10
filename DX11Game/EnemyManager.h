// �G�Ǘ��N���X [EnemyManager.h]
#pragma once

#include "Enemy.h"
#include <vector>
#include "Land.h"

using namespace std;

// �E�F�[�u���̓G�̏��
struct EnemyInfo {
	EModelType          t_Type;     // ���f���^�C�v
	XMFLOAT3            t_vPos;     // �ʒu
	XMFLOAT3            t_vAngle;   // ��]
	XMFLOAT3            t_vScale;   // �X�P�[��
};
// �E�F�[�u�̏��
struct WaveInfo {
	std::vector<EnemyInfo> _EnemyInfo;
};

class CEnemyManager : public CGameObj
{
private:
	static DATARECORD m_gDataRecord[];   // �ۑ�����f�[�^�z��
	/* 
	     �ۑ��p�ϐ�
		 �E���E�F�[�u��
		 �E�E�F�[�u���Ƃ̓G�̐�
		 �E�G�̏��
	*/
	// ���E�F�[�u��
	unsigned int m_nWaveNum;
	// �E�F�[�u���Ƃ̓G�̐�
	std::vector<int> m_nWaveEnemyNum;
	// �G�̏��
	std::vector<EnemyInfo> m_EnemyInfos;


	// ���[�h�����f�[�^�����ɍ\�z���Ďg�p����E�F�[�u�z��
	// �T�C�Y�͑��E�F�[�u��
	std::vector<WaveInfo> m_WaveInfos;
	
	// ���݂̃E�F�[�u�ԍ�
	int m_nWaveCurNum;

	


		// �����V�[��
	CScene* m_pScene;
	// ��������G�l�~�[���_���I�u�W�F�N�g
	CGameObj* m_pTarget;
	// �n�`
	CLand* m_pLand;
	// �t�B�[���h�ɂ���̓G�̐�
	static int m_nEnemyCnt;
protected:

public:
	CEnemyManager(CScene*);
	~CEnemyManager();

	virtual HRESULT Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();

	void SetTarget(CGameObj*);
	virtual void CreateEnemy(int WaveNum);

	int GetCurWaveNum() { return m_nWaveCurNum; }
	
	static int EnemyCnt() { return m_nEnemyCnt; }
	static void DelEnemyCnt() { --m_nEnemyCnt; }

	DATARECORD* GetDataRecord() { return m_gDataRecord; }
	int Save(CSaveManager* mgr);
	int Load(CSaveManager* mgr);
};
