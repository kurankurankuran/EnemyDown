#include "EnemyManager.h"
#include "Enemy_Maria.h"
#include "EffekseerManager.h"
#include "Enemy_Mutant.h"
#include "Enemy_Wizard.h"
#include "Input.h"
#include "Fade.h"

// �ۑ�����ϐ��̈ʒu���w��
DATARECORD CEnemyManager::m_gDataRecord[] =
{
   DATA_LOCAL(CEnemyManager, m_nWaveNum),
   DATA_MEM(CEnemyManager, m_nWaveEnemyNum, 0),
   DATA_MEM(CEnemyManager, m_EnemyInfos, 0),
   DATA_END
};

// �ÓI�����o
// �t�B�[���h�ɂ���̓G�̐�
int CEnemyManager::m_nEnemyCnt = 0;


CEnemyManager::CEnemyManager(CScene* pScene) : CGameObj(pScene)
{
	SetTag(GOT_ENEMYMANAGER);
	m_pScene = pScene;
	m_pTarget = nullptr;
	m_pLand = nullptr;

	SetObjID(EObjID::OI_ENEMY_MANAGER);
	
}

CEnemyManager::~CEnemyManager()
{

}

HRESULT CEnemyManager::Init()
{
	HRESULT hr = S_OK;
	m_pTarget = m_pScene->Find(GOT_PLAYER);
	// �n�`���f���|�C���^
	m_pLand = (CLand*)m_pScene->Find(GOT_LAND);

	// kari
	if (CScene::GetCurrent()->GetID() == EScene::SCENE_GAMEEDITOR)
	{
		// ���E�F�[�u��
		m_nWaveNum = 4;
		// �E�F�[�u���Ƃ̓G�̐�
		m_nWaveEnemyNum.push_back(2);
		m_nWaveEnemyNum.push_back(2);
		m_nWaveEnemyNum.push_back(3);
		m_nWaveEnemyNum.push_back(5);

		// �G�̏��
		EnemyInfo _EnemyInfo;
		//1
		_EnemyInfo.t_Type = MT_MUTANT;
		_EnemyInfo.t_vPos = XMFLOAT3(300, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);
		_EnemyInfo.t_Type = MT_MUTANT;
		_EnemyInfo.t_vPos = XMFLOAT3(-300, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);

		//2
		_EnemyInfo.t_Type = MT_WIZARD;
		_EnemyInfo.t_vPos = XMFLOAT3(300, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);
		_EnemyInfo.t_Type = MT_WIZARD;
		_EnemyInfo.t_vPos = XMFLOAT3(-300, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);

		//3
		_EnemyInfo.t_Type = MT_WIZARD;
		_EnemyInfo.t_vPos = XMFLOAT3(500, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);
		_EnemyInfo.t_Type = MT_MUTANT;
		_EnemyInfo.t_vPos = XMFLOAT3(0, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);
		_EnemyInfo.t_Type = MT_WIZARD;
		_EnemyInfo.t_vPos = XMFLOAT3(-500, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);

		//4
		_EnemyInfo.t_Type = MT_WIZARD;
		_EnemyInfo.t_vPos = XMFLOAT3(800, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);
		_EnemyInfo.t_Type = MT_MUTANT;
		_EnemyInfo.t_vPos = XMFLOAT3(400, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);
		_EnemyInfo.t_Type = MT_MARIA;
		_EnemyInfo.t_vPos = XMFLOAT3(0, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);
		_EnemyInfo.t_Type = MT_MUTANT;
		_EnemyInfo.t_vPos = XMFLOAT3(-400, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);
		_EnemyInfo.t_Type = MT_WIZARD;
		_EnemyInfo.t_vPos = XMFLOAT3(-800, 0, 1000);
		_EnemyInfo.t_vAngle = XMFLOAT3(0, 180, 0);
		_EnemyInfo.t_vScale = XMFLOAT3(1, 1, 1);
		m_EnemyInfos.push_back(_EnemyInfo);
		// kari�����܂�
	}
	else
	{ 
		// �E�F�[�u�\�z
		m_WaveInfos.clear();
		// �E�F�[�u�����������m��
		m_WaveInfos.resize(m_nWaveNum);
		// �G�̏����i�[���Ă���
		int nWaveNum = 0;
		// �E�F�[�u���ƂɊi�[�����G�̐�
		int nEnemyCount = 0;
		for (unsigned int i = 0; i < m_EnemyInfos.size(); ++i)
		{
			m_WaveInfos[nWaveNum]._EnemyInfo.push_back(m_EnemyInfos[i]);
			++nEnemyCount;
			if (m_nWaveEnemyNum[nWaveNum] <= nEnemyCount)
			{
				// �J�E���g���Z�b�g
				nEnemyCount = 0;
				++nWaveNum;
			}
		}

		// �n�߂�E�F�[�u�ԍ��w��
		m_nWaveCurNum = 0;
		// �G����
		CreateEnemy(m_nWaveCurNum);
	}


	
	return hr;
}

void CEnemyManager::Fin()
{
	m_EnemyInfos.clear();
}

void CEnemyManager::Update()
{
	if (m_nEnemyCnt <= 0)
	{
		++m_nWaveCurNum;
		if ((unsigned int)m_nWaveCurNum < m_WaveInfos.size())
		{
			CreateEnemy(m_nWaveCurNum);
		}
		else
		{
			// �Q�[���N���A�[
			CFade::Out(SCENE_GAMECLEAR);
		}
	}
}

void CEnemyManager::Draw()
{

}

void CEnemyManager::SetTarget(CGameObj* pObj)
{
	m_pTarget = pObj;
}

void CEnemyManager::CreateEnemy(int WaveNum)
{
	CGameObj* pGameObj = CScene::GetCurrent()->Find(GOT_PLAYER);

	// �G�l�~�[�J�E���g���Z�b�g
	m_nEnemyCnt = 0;
	for (unsigned int i = 0; i < m_WaveInfos[WaveNum]._EnemyInfo.size(); ++i)
	{
		CEnemy* pEnemy;
		switch (m_WaveInfos[WaveNum]._EnemyInfo[i].t_Type)
		{
		case EModelType::MT_MUTANT:
			pEnemy = new CEnemy_Mutant(m_pScene);
			break;
		case EModelType::MT_WIZARD:
			pEnemy = new CEnemy_Wizard(m_pScene);
			break;
		case EModelType::MT_MARIA:
			pEnemy = new CEnemy_Maria(m_pScene);
			break;
		default:
			pEnemy = nullptr;
			break;
		}
		if (pEnemy) {
			pEnemy->SetStartPos(m_WaveInfos[WaveNum]._EnemyInfo[i].t_vPos);
			pEnemy->SetStartAngle(m_WaveInfos[WaveNum]._EnemyInfo[i].t_vAngle);
			pEnemy->SetStartScale(m_WaveInfos[WaveNum]._EnemyInfo[i].t_vScale);
			pEnemy->SetTarget(pGameObj);
			pEnemy->Init();
		}
		// �G�l�~�[�J�E���g�X�V
		++m_nEnemyCnt;
	}

}

// �Z�[�u
int CEnemyManager::Save(CSaveManager* mgr)
{
	// �ۑ�����l�̐ݒ�
	m_gDataRecord[0] = DATA_LOCAL(CEnemyManager, m_nWaveNum);
	m_gDataRecord[1] = DATA_MEM(CEnemyManager, m_nWaveEnemyNum, (int)(sizeof(int) * m_nWaveEnemyNum.size()));
	m_gDataRecord[2] = DATA_MEM(CEnemyManager, m_EnemyInfos, (int)(sizeof(EnemyInfo) * m_EnemyInfos.size()));
	m_gDataRecord[3] = DATA_END;

	return mgr->Write(GetDataRecord(), this);   // ��������
}


int CEnemyManager::Load(CSaveManager* mgr)
{
    int nOk = mgr->Read(GetDataRecord(), this);   // �ǂݍ���
	// �����ŃN���X�̕ϐ��Ƀf�[�^���������܂ꂽ�̂ŁA������g���čč\�z�̍�Ƃ��s��
	// ���[�h�����l�����ɃE�F�[�u�\�z
	m_WaveInfos.clear();
	// �E�F�[�u�����������m��
	m_WaveInfos.resize(m_nWaveNum);
	// �G�̏����i�[���Ă���
	int nWaveNum = 0;
	// �E�F�[�u���ƂɊi�[�����G�̐�
	int nEnemyCount = 0;
	for (unsigned int i = 0; i < m_EnemyInfos.size(); ++i)
	{
		m_WaveInfos[nWaveNum]._EnemyInfo.push_back(m_EnemyInfos[i]);
		++nEnemyCount;
		if (m_nWaveEnemyNum[nWaveNum] <= nEnemyCount)
		{
			// �J�E���g���Z�b�g
			nEnemyCount = 0;
			++nWaveNum;
		}
	}

	return nOk;
}