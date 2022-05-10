// 敵（魔法使い） [Enemy_Wizard.h]
#pragma once
#include "Enemy.h"

class CEnemy_Wizard final : public CEnemy
{
private:
	int m_nAttackCnt;// 攻撃タイミング調整用

protected:
	
public:
	CEnemy_Wizard(CScene* pScene);		// コンストラクタ
	virtual ~CEnemy_Wizard();			// デストラクタ

	virtual HRESULT Init();		// 初期化
	virtual void Fin();			// 終了処理
	virtual void Update();		// 更新
	virtual void Draw();		// 描画

private:
	
protected:
	void AnimeUpdate();
	void NearAttack();
};
