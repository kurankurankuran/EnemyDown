// 敵管理クラス [EnemyManager.h]
#pragma once

#include "Enemy.h"
#include <vector>
#include "Land.h"

using namespace std;

// ウェーブ中の敵の情報
struct EnemyInfo {
	EModelType          t_Type;     // モデルタイプ
	XMFLOAT3            t_vPos;     // 位置
	XMFLOAT3            t_vAngle;   // 回転
	XMFLOAT3            t_vScale;   // スケール
};
// ウェーブの情報
struct WaveInfo {
	std::vector<EnemyInfo> _EnemyInfo;
};

class CEnemyManager : public CGameObj
{
private:
	static DATARECORD m_gDataRecord[];   // 保存するデータ配列
	/* 
	     保存用変数
		 ・総ウェーブ数
		 ・ウェーブごとの敵の数
		 ・敵の情報
	*/
	// 総ウェーブ数
	unsigned int m_nWaveNum;
	// ウェーブごとの敵の数
	std::vector<int> m_nWaveEnemyNum;
	// 敵の情報
	std::vector<EnemyInfo> m_EnemyInfos;


	// ロードしたデータを元に構築して使用するウェーブ配列
	// サイズは総ウェーブ数
	std::vector<WaveInfo> m_WaveInfos;
	
	// 現在のウェーブ番号
	int m_nWaveCurNum;

	


		// 所属シーン
	CScene* m_pScene;
	// 生成するエネミーが狙うオブジェクト
	CGameObj* m_pTarget;
	// 地形
	CLand* m_pLand;
	// フィールドにいるの敵の数
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
