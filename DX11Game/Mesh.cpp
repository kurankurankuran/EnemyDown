// ユーザ定義メッシュ [Mesh.cpp]
#include "Mesh.h"
#include "Shader.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define M_DIFFUSE		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR		XMFLOAT4(0.1f,0.1f,0.1f,1.0f)
#define M_AMBIENT		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_EMISSIVE		XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// シェーダに渡す値
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ワールド×ビュー×射影行列(転置行列)
	XMMATRIX	mW;			// ワールド行列(転置行列)
	XMMATRIX	mTex;		// テクスチャ行列(転置行列)
};
struct SHADER_GLOBAL2 {
	XMVECTOR	vEye;		// 視点座標
	// 光源
	XMVECTOR	vLightDir;	// 光源方向
	XMVECTOR	vLa;		// 光源色(アンビエント)
	XMVECTOR	vLd;		// 光源色(ディフューズ)
	XMVECTOR	vLs;		// 光源色(スペキュラ)
	// マテリアル
	XMVECTOR	vAmbient;	// アンビエント色(+テクスチャ有無)
	XMVECTOR	vDiffuse;	// ディフューズ色
	XMVECTOR	vSpecular;	// スペキュラ色(+スペキュラ強度)
	XMVECTOR	vEmissive;	// エミッシブ色
};

// 静的メンバ
ID3D11Buffer* CMesh::m_pConstantBuffer[2];	// 定数バッファ
ID3D11SamplerState* CMesh::m_pSamplerState;	// テクスチャ サンプラ
ID3D11VertexShader* CMesh::m_pVertexShader;	// 頂点シェーダ
ID3D11InputLayout* CMesh::m_pInputLayout;	// 頂点フォーマット
ID3D11PixelShader* CMesh::m_pPixelShader;	// ピクセルシェーダ

XMFLOAT2					CMesh::m_vPosTexFrame;			// UV座標
XMFLOAT2					CMesh::m_vSizeTexFrame;			// テクスチャ抽出サイズ
// コンストラクタ
CMesh::CMesh()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pCamera = nullptr;
	m_pLight = nullptr;
	XMStoreFloat4x4(&m_mWorld, XMMatrixIdentity());

	m_pTexture = nullptr;
	XMStoreFloat4x4(&mTex, XMMatrixIdentity());
	mTex._44 = 0.0f;

	m_primitiveType = PT_TRIANGLESTRIP;
 
}

// デストラクタ
CMesh::~CMesh()
{
}

// シェーダ初期化
HRESULT CMesh::InitShader()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	// シェーダ初期化
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = LoadShader("Vertex", "Pixel", &m_pVertexShader, &m_pInputLayout, &m_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		return hr;
	}




	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer[0]);
	if (FAILED(hr)) return hr;
	bd.ByteWidth = sizeof(SHADER_GLOBAL2);
	hr = pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer[1]);
	if (FAILED(hr)) return hr;

	// テクスチャ サンプラ生成
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &m_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	return hr;
}

// シェーダ終了処理
void CMesh::FinShader()
{
	// 定数バッファの解放
	for (int i = 0; i < _countof(m_pConstantBuffer); ++i) {
		SAFE_RELEASE(m_pConstantBuffer[i]);
	}
	// ピクセルシェーダ解放
	SAFE_RELEASE(m_pPixelShader);
	// 頂点フォーマット解放
	SAFE_RELEASE(m_pInputLayout);
	// 頂点シェーダ解放
	SAFE_RELEASE(m_pVertexShader);
}

// 初期化
HRESULT CMesh::Init(const VERTEX_3D vertexWk[], int nVertex, const int indexWk[], int nIndex)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	m_nNumVertex = nVertex;

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VERTEX_3D) * m_nNumVertex;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertexWk;
	hr = pDevice->CreateBuffer(&vbd, &initData, &m_pVertexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	m_nNumIndex = nIndex;

	CD3D11_BUFFER_DESC ibd(m_nNumIndex * sizeof(int), D3D11_BIND_INDEX_BUFFER);
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = indexWk;
	hr = pDevice->CreateBuffer(&ibd, &initData, &m_pIndexBuffer);

	// マテリアルの初期設定
	m_material.m_Diffuse = M_DIFFUSE;
	m_material.m_Ambient = M_AMBIENT;
	m_material.m_Specular = M_SPECULAR;
	m_material.m_Power = 0.0f;
	m_material.m_Emissive = M_EMISSIVE;

	m_vPosTexFrame = XMFLOAT2(0.0f, 0.0f);
	m_vSizeTexFrame = XMFLOAT2(1.0f, 1.0f);

	return hr;
}

// 終了処理
void CMesh::Fin()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

// 描画
void CMesh::Draw()
{
	if (m_pTexture) {

		XMMATRIX mWorld;
		// 拡縮
		mWorld = XMMatrixScaling(m_vSizeTexFrame.x, m_vSizeTexFrame.y, 1.0f);
		// 移動
		mWorld *= XMMatrixTranslation(m_vPosTexFrame.x, m_vPosTexFrame.y, 0.0f);
		// テクスチャ マトリックスに設定
		XMStoreFloat4x4(&mTex, mWorld);
		mTex._44 = 1.0f;
	}

	// シェーダ設定
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// 頂点バッファをセット
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	// インデックスバッファをセット
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
	//pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture);
	pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture);

	SHADER_GLOBAL cb;
	XMMATRIX mtxWorld = XMLoadFloat4x4(&m_mWorld);
	cb.mWVP = XMMatrixTranspose(mtxWorld * XMLoadFloat4x4(&m_pCamera->GetViewMatrix()) * XMLoadFloat4x4(&m_pCamera->GetProjMatrix()));
	cb.mW = XMMatrixTranspose(mtxWorld);

	//XMFLOAT4X4 mTex(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
	
	XMMATRIX mtxTex = XMLoadFloat4x4(&mTex);
	cb.mTex = XMMatrixTranspose(mtxTex);
	pDeviceContext->UpdateSubresource(m_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer[0]);
	SHADER_GLOBAL2 cb2;
	cb2.vEye = XMLoadFloat3(&m_pCamera->GetPos());
	cb2.vLightDir = XMLoadFloat3(&m_pLight->GetDir());
	cb2.vLa = XMLoadFloat4(&m_pLight->GetAmbient());
	cb2.vLd = XMLoadFloat4(&m_pLight->GetDiffuse());
	cb2.vLs = XMLoadFloat4(&m_pLight->GetSpecular());
	cb2.vDiffuse = XMLoadFloat4(&m_material.m_Diffuse);
	cb2.vAmbient = XMVectorSet(m_material.m_Ambient.x, m_material.m_Ambient.y, m_material.m_Ambient.z, mTex._44/*0.f*/);
	cb2.vSpecular = XMVectorSet(m_material.m_Specular.x, m_material.m_Specular.y, m_material.m_Specular.z, m_material.m_Power);
	cb2.vEmissive = XMLoadFloat4(&m_material.m_Emissive);
	pDeviceContext->UpdateSubresource(m_pConstantBuffer[1], 0, nullptr, &cb2, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer[1]);

	// プリミティブ形状をセット
	static const D3D11_PRIMITIVE_TOPOLOGY pt[] = {
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,	// 0なら三角形ストリップ
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	};

	//pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetPrimitiveTopology(pt[m_primitiveType]);

	// ポリゴンの描画
	pDeviceContext->DrawIndexed(m_nNumIndex, 0, 0);
}

// マテリアル設定
void CMesh::SetMaterial(const CMeshMaterial* pMaterial)
{
	if (pMaterial) {
		m_material = *pMaterial;
	} else {
		m_material.m_Diffuse = M_DIFFUSE;
		m_material.m_Ambient = M_AMBIENT;
		m_material.m_Specular = M_SPECULAR;
		m_material.m_Power = 0.0f;
		m_material.m_Emissive = M_EMISSIVE;
	}
}

void CMesh::SetTexture(ID3D11ShaderResourceView* pTex)
{
	m_pTexture = pTex;
	//mTex._44 = 1.0f;
}


// 左上テクスチャ座標の設定 (0.0≦fU＜1.0, 0.0≦fV＜1.0)
void CMesh::SetUV(float fU, float fV)
{
	m_vPosTexFrame.x = fU;
	m_vPosTexFrame.y = fV;
}

// テクスチャフレームサイズの設定 (0.0＜fWidth≦1.0, 0.0＜fHeight≦1.0)
void CMesh::SetFrameSize(float fWidth, float fHeight)
{
	m_vSizeTexFrame.x = fWidth;
	m_vSizeTexFrame.y = fHeight;
}

