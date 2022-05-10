// 敵（ﾐｭｰﾀﾝﾄ） [Enemy_Mutant.h]
#pragma once
#include "Enemy.h"

class CEnemy_Mutant final : public CEnemy
{
private:
	
protected:
	void AnimeUpdate();
	void NearAttack();
public:
	CEnemy_Mutant(CScene* pScene);		// コンストラクタ
	~CEnemy_Mutant();			        // デストラクタ

	HRESULT Init();		// 初期化
	void Fin();			// 終了処理
	void Update();		// 更新
	void Draw();		// 描画

private:
	
};
