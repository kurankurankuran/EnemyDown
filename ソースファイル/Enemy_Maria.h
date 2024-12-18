// マリア（ボス） [Enemy_Maria.h]
#pragma once
#include "Enemy.h"

class CEnemy_Maria final : public CEnemy
{
private:
	
protected:

public:
	CEnemy_Maria(CScene* pScene);		// コンストラクタ
	virtual ~CEnemy_Maria();			// デストラクタ

	virtual HRESULT Init();		// 初期化
	virtual void Fin();			// 終了処理
	virtual void Update();		// 更新
	virtual void Draw();		// 描画

private:
protected:
	void AnimeUpdate();
	void NearAttack();
};
