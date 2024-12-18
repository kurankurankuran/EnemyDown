// 地面 [Land.h]
#pragma once
#include "Model.h"
#include "octree.h"

// 定数定義
// 重力(全キャラ共通ならシーン自体に設定すべき?)
const float GRAVITY_SCALE = 4.5f;


class CLand : public CModel
{
private:
	//CPlayer* m_pPlayer;
	TAssimpVertex* m_pVertex;
	UINT m_nVertex;
	UINT* m_pIndex;
	UINT m_nIndex;

	Octree* pOctree;

public:
	CLand(CScene* pScene);	// コンストラクタ
	virtual ~CLand();		// デストラクタ

	virtual HRESULT Awake(); // 最初の初期化
	virtual HRESULT Init();	// 初期化
	virtual void Fin();		// 終了処理
	virtual void Update();  // 更新処理
	virtual void Draw();    // 描画処理

	bool Collision(XMFLOAT3 vPos, XMFLOAT3 vP0, XMFLOAT3 vP1, XMFLOAT3* pX, XMFLOAT3* pN = nullptr);
	bool Collision(CGameObj* pObj , XMFLOAT3 vP0, XMFLOAT3 vP1, XMFLOAT3* pX, XMFLOAT3* pN = nullptr);
	//void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
};
