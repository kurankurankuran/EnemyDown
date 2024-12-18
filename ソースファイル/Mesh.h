// ユーザ定義メッシュ [Mesh.h]
#pragma once
#include "main.h"
#include "Camera.h"
#include "Light.h"

enum ePrimitiveType {
	PT_UNDEFINED = 0,
	PT_POINT,
	PT_LINE,
	PT_LINESTRIP,
	PT_TRIANGLE,
	PT_TRIANGLESTRIP,

	MAX_PRIMITIVETYPE
};

class CMeshMaterial {
public:
	XMFLOAT4	m_Diffuse;		// Diffuse color RGBA
	XMFLOAT4	m_Ambient;		// Ambient color RGB
	XMFLOAT4	m_Specular;		// Specular 'shininess'
	XMFLOAT4	m_Emissive;		// Emissive color RGB
	float		m_Power;		// Sharpness if specular highlight
};

class CMesh
{
private:
	

	

	CMeshMaterial m_material;		// マテリアル

	static ID3D11Buffer* m_pConstantBuffer[2];	// 定数バッファ
	static ID3D11SamplerState* m_pSamplerState;	// テクスチャ サンプラ
	static ID3D11VertexShader* m_pVertexShader;	// 頂点シェーダ
	static ID3D11InputLayout* m_pInputLayout;	// 頂点フォーマット
	static ID3D11PixelShader* m_pPixelShader;	// ピクセルシェーダ


	ID3D11ShaderResourceView* m_pTexture;	// テクスチャへのポインタ
	XMFLOAT4X4 mTex;                        // テクスチャ行列

	static XMFLOAT2 m_vPosTexFrame;					// UV座標
	static XMFLOAT2 m_vSizeTexFrame;				// テクスチャ抽出サイズ

protected:

	XMFLOAT4X4 m_mWorld;
	CCamera* m_pCamera;
	CLight* m_pLight;

	int m_nNumVertex;				// 総頂点数	
	int m_nNumIndex;				// 総インデックス数
	ID3D11Buffer* m_pVertexBuffer;	// 頂点バッファインターフェースへのポインタ
	ID3D11Buffer* m_pIndexBuffer;	// インデックスバッファインターフェースへのポインタ
	ePrimitiveType m_primitiveType;			// プリミティブ型
public:
	CMesh();
	virtual ~CMesh();

	static HRESULT InitShader();
	static void FinShader();

	HRESULT Init(const VERTEX_3D vertexWk[], int nVertex, const int indexWk[], int nIndex);
	virtual void Fin();
	virtual void Draw();

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	void SetLight(CLight* pLight) { m_pLight = pLight; }
	void SetWorld(XMFLOAT4X4* pWorld) { m_mWorld = *pWorld; }
	void SetMaterial(const CMeshMaterial* pMaterial);
	void GetMaterial(CMeshMaterial* pMaterial) { *pMaterial = m_material; }
	void SetDiffuse(XMFLOAT4 vDiffuse) { m_material.m_Diffuse = vDiffuse; }

	XMFLOAT4X4* GetWorld() { return &m_mWorld; }

	void SetTexture(ID3D11ShaderResourceView*);
	void SetPimitiveType(ePrimitiveType primitive) { m_primitiveType = primitive; }

	static void SetUV(float fU, float fV);
	static void SetFrameSize(float fWidth, float fHeight);

	static ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }
	int GetVertexNum() { return m_nNumVertex; }
	ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
	ID3D11Buffer** GetVertexBufferPoint() { return &m_pVertexBuffer; }
	int GetIndexNum() { return m_nNumIndex; }
	ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }

};
