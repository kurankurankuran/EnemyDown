//=============================================================================
//
// エネミーウェーブ設定 クラス定義 [EW_Setting_Base.h]
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
	unsigned int m_nTotalWaveNum;              // 総ウェーブ数
	unsigned int m_nCurWaveNum;                // 現在設定中のウェーブ番号
	unsigned int m_nEnemyNum;                  // ウェーブ中の敵の数
	EModelType m_Type;                         // 生成する敵のモデルタイプ
	XMFLOAT3 m_vPos;                           // 生成する敵の位置
	std::map<ETextIndex, int> m_mapTextIndex;  // 表示しているテキストの管理用インデックス
};
