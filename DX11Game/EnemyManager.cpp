#include "EnemyManager.h"
#include "Enemy_Maria.h"
#include "EffekseerManager.h"
#include "Enemy_Mutant.h"
#include "Enemy_Wizard.h"
#include "Input.h"
#include "Fade.h"

// 保存する変数の位置を指定
DATARECORD CEnemyManager::m_gDataRecord[] =
{
   DATA_LOCAL(CEnemyManager, m_nWaveNum),
   DATA_MEM(CEnemyManager, m_nWaveEnemyNum, 0),
   DATA_MEM(CEnemyManager, m_EnemyInfos, 0),
   DATA_END
};

// 静的メンバ
// フィールドにいるの敵の数
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
	// 地形モデルポインタ
	m_pLand = (CLand*)m_pScene->Find(GOT_LAND);

	// kari
	if (CScene::GetCurrent()->GetID() == EScene::SCENE_GAMEEDITOR)
	{
		// 総ウェーブ数
		m_nWaveNum = 4;
		// ウェーブごとの敵の数
		m_nWaveEnemyNum.push_back(2);
		m_nWaveEnemyNum.push_back(2);
		m_nWaveEnemyNum.push_back(3);
		m_nWaveEnemyNum.push_back(5);

		// 敵の情報
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
		// kariここまで
	}
	else
	{ 
		// ウェーブ構築
		m_WaveInfos.clear();
		// ウェーブ数分メモリ確保
		m_WaveInfos.resize(m_nWaveNum);
		// 敵の情報を格納していく
		int nWaveNum = 0;
		// ウェーブごとに格納した敵の数
		int nEnemyCount = 0;
		for (unsigned int i = 0; i < m_EnemyInfos.size(); ++i)
		{
			m_WaveInfos[nWaveNum]._EnemyInfo.push_back(m_EnemyInfos[i]);
			++nEnemyCount;
			if (m_nWaveEnemyNum[nWaveNum] <= nEnemyCount)
			{
				// カウントリセット
				nEnemyCount = 0;
				++nWaveNum;
			}
		}

		// 始めるウェーブ番号指定
		m_nWaveCurNum = 0;
		// 敵生成
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
			// ゲームクリアー
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

	// エネミーカウントリセット
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
		// エネミーカウント更新
		++m_nEnemyCnt;
	}

}

// セーブ
int CEnemyManager::Save(CSaveManager* mgr)
{
	// 保存する値の設定
	m_gDataRecord[0] = DATA_LOCAL(CEnemyManager, m_nWaveNum);
	m_gDataRecord[1] = DATA_MEM(CEnemyManager, m_nWaveEnemyNum, (int)(sizeof(int) * m_nWaveEnemyNum.size()));
	m_gDataRecord[2] = DATA_MEM(CEnemyManager, m_EnemyInfos, (int)(sizeof(EnemyInfo) * m_EnemyInfos.size()));
	m_gDataRecord[3] = DATA_END;

	return mgr->Write(GetDataRecord(), this);   // 書き込み
}


int CEnemyManager::Load(CSaveManager* mgr)
{
    int nOk = mgr->Read(GetDataRecord(), this);   // 読み込み
	// ここでクラスの変数にデータが流し込まれたので、それを使って再構築の作業を行う
	// ロードした値を元にウェーブ構築
	m_WaveInfos.clear();
	// ウェーブ数分メモリ確保
	m_WaveInfos.resize(m_nWaveNum);
	// 敵の情報を格納していく
	int nWaveNum = 0;
	// ウェーブごとに格納した敵の数
	int nEnemyCount = 0;
	for (unsigned int i = 0; i < m_EnemyInfos.size(); ++i)
	{
		m_WaveInfos[nWaveNum]._EnemyInfo.push_back(m_EnemyInfos[i]);
		++nEnemyCount;
		if (m_nWaveEnemyNum[nWaveNum] <= nEnemyCount)
		{
			// カウントリセット
			nEnemyCount = 0;
			++nWaveNum;
		}
	}

	return nOk;
}