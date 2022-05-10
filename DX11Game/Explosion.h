// 爆発エフェクト クラス [Explosion.h]
#pragma once
#include "GameObj.h"
#include "AssimpModel.h"

class CExplosion;

// 破片
class CDebris
{
private:
	CExplosion* m_pParent;	// 爆発エフェクト
	CDebris* m_pBack;		// 前の破片
	CDebris* m_pNext;		// 次の破片

	ID3D11Buffer* m_pVertexBuffer;	// 頂点バッファ
	XMFLOAT4X4 m_world;		// ワールド変換
	XMFLOAT3 m_velocity;	// 速度
	XMFLOAT3 m_accel;		// 加速度
	XMFLOAT3 m_axis;		// 回転軸
	float m_rotate;			// 回転速度
	float m_alpha;			// 不透明度

public:
	CDebris(CExplosion* pExp);	// コンストラクタ
	~CDebris();					// デストラクタ

	CDebris* GetTop();			// リスト先頭を取得
	void SetTop(CDebris* pTop);	// リスト先頭を設定
	CDebris* GetNext() { return m_pNext; }

	HRESULT Init(XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2,
		float fRadius, XMFLOAT4X4* pWorld, XMFLOAT3* pVelocity);	// 初期化
	void Uninit();	// 終了処理
	void Update();	// 更新
	void Draw();	// 描画
};

// 爆発エフェクト
class CExplosion //: public CGameObj
{
private:
	CDebris* m_pDebris;	// 破片
	static ID3D11Buffer* m_pConstantBuffer[2];		// 定数バッファ
	static ID3D11VertexShader* m_pVertexShader;		// 頂点シェーダ
	static ID3D11InputLayout* m_pInputLayout;		// 頂点フォーマット
	static ID3D11PixelShader* m_pPixelShader;		// ピクセルシェーダ

	TAssimpVertex* m_pVertex;	// 頂点配列
	UINT m_uVertex;				// 頂点数
	UINT* m_pIndex;				// インデックス配列
	UINT m_uIndex;				// インデックス数

	float m_fRadius;
	CScene* m_pScene;

public:
	CExplosion(CScene* pScene);
	~CExplosion();

	CDebris* GetTop() { return m_pDebris; }
	void SetTop(CDebris* pTop) { m_pDebris = pTop; }

	static HRESULT InitShader();
	static void FinShader();

	HRESULT Init();		// 初期化
	void Fin();			// 終了処理
	void Update();		// 更新
	void Draw();		// 描画
	void DrawAlpha();	// 半透明描画

	// モデル設定
	HRESULT SetModel(CAssimpModel* pModel);

	// 爆発開始
	HRESULT Start(XMFLOAT4X4* pWorld, XMFLOAT3* pVelocity = nullptr);

	// シェーダの定数バッファを更新
	void UpdateBuffer(int nShader, void* pData);

	// シーン取得
	CScene* GetScene() { return m_pScene; }

	void SetRadius(float fRadius) { m_fRadius = fRadius; }
	float GetRadius() { return m_fRadius; }

	//int GetDebrisSize();

	void FinDeb();
};
