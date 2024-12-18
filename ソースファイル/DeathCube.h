// 触れたら死ぬキューブ [Deth.h]
#pragma once
#include "Model.h"
#include "Player.h"
class CDeathCube : public CModel
{
private:
	//CPlayer* m_pPlayer;
public:
	CDeathCube(CScene* pScene);	// コンストラクタ
	virtual ~CDeathCube();		// デストラクタ

	virtual HRESULT Init();	// 初期化
	//virtual void Fin();		// 終了処理
	virtual void Update();  // 更新処理
	virtual void Draw();    // 描画処理

	//virtual bool CollisionOBB(CGameObj* pOther); // OBB当たり判定

	//void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
};
