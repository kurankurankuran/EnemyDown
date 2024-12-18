// 直方体 [Box.h]
#pragma once
#include "Mesh.h"
#include<memory>

class CBox : public CMesh
{
private:
	static ID3D11VertexShader* m_pVertexShader;	// 頂点シェーダ
	static ID3D11InputLayout* m_pInputLayout;	// 頂点フォーマット
	static ID3D11PixelShader* m_pPixelShader;	// ピクセルシェーダ


	VERTEX_3D* m_pVertex;
	UINT*      m_pIndex;
	UINT       m_nVertexNum;
	UINT       m_nIndexNum;
	XMFLOAT3   m_VertexPosArray[8];// OBBの角８頂点の座標（当たり判定で使用）
	float      m_fCenterToVertexDistance; // OBBの中心点から頂点までの距離

	static ID3D11Buffer* m_pConstantBuffer;// ミニマップ描画シェーダに渡す用
	std::shared_ptr<CCamera> m_pMiniMapCamera;             // ミニマップ用カメラポインタ
	XMFLOAT4 m_vColor;
	bool     m_bTrigger;                   // イベント起動用当たり判定としてのみ使う（trueですり抜ける）
public:
	CBox();
	virtual ~CBox();

	static HRESULT InitShader();
	static void FinShader();

	HRESULT Init(XMFLOAT3 vBBox);
	void SetColor(XMFLOAT3 color);
	void DepthDraw();// ミニマップ用テクスチャへの書き込み

	VERTEX_3D* GetVertex() { return m_pVertex; }
	UINT*      GetIndex() { return m_pIndex; }
	UINT       GetVertexNum() { return m_nVertexNum; }
	UINT       GetIndexNum() { return m_nIndexNum; }

	XMFLOAT3*  GetVertexPosArray() { return m_VertexPosArray; }
	float GetCenterToVertexDistance() { return m_fCenterToVertexDistance; }

	void SetMiniMapCamera(std::shared_ptr<CCamera> pCamera) { m_pMiniMapCamera = pCamera; }
	void SetTrigger(bool bTrigger) { m_bTrigger = bTrigger; }
	bool GetTrigger() { return m_bTrigger; }
};
