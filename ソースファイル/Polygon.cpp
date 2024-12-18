//=============================================================================
//
// 2Dポリゴン処理クラス実装 [Polygon.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "Polygon.h"
#include "Shader.h"

// シェーダに渡す値
struct SHADER_GLOBAL {
	XMMATRIX	mWorld;		// ワールド変換行列(転置行列)
	XMMATRIX	mView;		// ビュー変換行列(転置行列)
	XMMATRIX	mProj;		// 射影変換行列(転置行列)
	XMMATRIX	mTex;		// テクスチャ変換行列(転置行列)
};

// 静的メンバ
ID3D11ShaderResourceView*	CPolygon::m_pTexture;				// テクスチャへのポインタ

VERTEX_2D					CPolygon::m_vertexWk[NUM_VERTEX];	// 頂点情報格納ワーク

XMFLOAT3					CPolygon::m_vPos;					// ポリゴンの移動量
XMFLOAT3					CPolygon::m_vAngle;					// ポリゴンの回転量
XMFLOAT3					CPolygon::m_vScale;					// ポリゴンのサイズ
XMFLOAT4					CPolygon::m_vColor;					// ポリゴンの頂点カラー
bool						CPolygon::m_bInvalidate;			// 頂点データ更新フラグ

XMFLOAT2					CPolygon::m_vPosTexFrame;			// UV座標
XMFLOAT2					CPolygon::m_vSizeTexFrame;			// テクスチャ抽出サイズ

ID3D11Buffer*				CPolygon::m_pConstantBuffer;		// 定数バッファ
ID3D11Buffer*				CPolygon::m_pVertexBuffer;			// 頂点バッファ
ID3D11SamplerState*			CPolygon::m_pSamplerState;			// テクスチャ サンプラ
ID3D11VertexShader*			CPolygon::m_pVertexShader;			// 頂点シェーダ
ID3D11InputLayout*			CPolygon::m_pInputLayout;			// 頂点フォーマット
ID3D11PixelShader*			CPolygon::m_pPixelShader;			// ピクセルシェーダ
ID3D11PixelShader*			CPolygon::m_pRenderPS;			// ピクセルシェーダ

XMFLOAT4X4					CPolygon::m_mProj;					// 射影変換行列
XMFLOAT4X4					CPolygon::m_mView;					// ビュー変換行列
XMFLOAT4X4					CPolygon::m_mWorld;					// ワールド変換行列
XMFLOAT4X4					CPolygon::m_mTex;					// テクスチャ変換行列

// 初期化
HRESULT CPolygon::Init(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	// シェーダ初期化
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = LoadShader("Vertex2D", "Pixel2D",
		&m_pVertexShader, &m_pInputLayout, &m_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		return hr;
	}

	hr = LoadPixelShader("RenderPS", &m_pRenderPS);
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
	hr = pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
	if (FAILED(hr)) return hr;

	// テクスチャ サンプラ生成
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &m_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	// 変換行列初期化
	XMStoreFloat4x4(&m_mWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mView, XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&m_mProj,
		XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 100.0f));
	XMStoreFloat4x4(&m_mTex, XMMatrixIdentity());
	m_mTex._44 = 0.0f;

	m_vPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_vColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_bInvalidate = false;

	m_vPosTexFrame = XMFLOAT2(0.0f, 0.0f);
	m_vSizeTexFrame = XMFLOAT2(1.0f, 1.0f);

	// 頂点情報の作成
	hr = MakeVertex(pDevice);

	return hr;
}

// 終了処理
void CPolygon::Fin(void)
{
	// テクスチャ サンプラの開放
	SAFE_RELEASE(m_pSamplerState);
	// 頂点バッファの解放
	SAFE_RELEASE(m_pVertexBuffer);
	// 定数バッファの解放
	SAFE_RELEASE(m_pConstantBuffer);
	// ピクセルシェーダ解放
	SAFE_RELEASE(m_pRenderPS);
	// ピクセルシェーダ解放
	SAFE_RELEASE(m_pPixelShader);
	// 頂点フォーマット解放
	SAFE_RELEASE(m_pInputLayout);
	// 頂点シェーダ解放
	SAFE_RELEASE(m_pVertexShader);
}

// 描画
void CPolygon::Draw(ID3D11DeviceContext* pDeviceContext)
{
	// 拡縮
	XMMATRIX mWorld = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	// 回転
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_vAngle.x),
		XMConvertToRadians(m_vAngle.y), XMConvertToRadians(m_vAngle.z));
	// 移動
	mWorld *= XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);
	// ワールド マトリックスに設定
	XMStoreFloat4x4(&m_mWorld, mWorld);

	if (m_pTexture) {
		// 拡縮
		mWorld = XMMatrixScaling(m_vSizeTexFrame.x, m_vSizeTexFrame.y, 1.0f);
		// 移動
		mWorld *= XMMatrixTranslation(m_vPosTexFrame.x, m_vPosTexFrame.y, 0.0f);
		// テクスチャ マトリックスに設定
		XMStoreFloat4x4(&m_mTex, mWorld);
	}
	else {
		// テクスチャ無し
		m_mTex._44 = 0.0f;
	}

	// 頂点バッファ更新
	SetVertex();

	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture);

	SHADER_GLOBAL cb;
	cb.mProj = XMMatrixTranspose(XMLoadFloat4x4(&m_mProj));
	cb.mView = XMMatrixTranspose(XMLoadFloat4x4(&m_mView));
	cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_mWorld));
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&m_mTex));
	pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴンの描画
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

// 頂点の作成
HRESULT CPolygon::MakeVertex(ID3D11Device* pDevice)
{
	// 頂点座標の設定
	m_vertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	m_vertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	m_vertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	m_vertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);

	// 拡散反射光の設定
	m_vertexWk[0].diffuse = m_vColor;
	m_vertexWk[1].diffuse = m_vColor;
	m_vertexWk[2].diffuse = m_vColor;
	m_vertexWk[3].diffuse = m_vColor;

	// テクスチャ座標の設定
	m_vertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	m_vertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	m_vertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	m_vertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(m_vertexWk);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &m_vertexWk[0];

	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &m_pVertexBuffer);

	return hr;
}

// 頂点座標の設定
void CPolygon::SetVertex(void)
{
	if (m_bInvalidate) {
		//頂点バッファの中身を埋める
		ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
		HRESULT hr = S_OK;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		hr = pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		if (SUCCEEDED(hr)) {
			// 拡散反射光の設定
			m_vertexWk[0].diffuse = m_vColor;
			m_vertexWk[1].diffuse = m_vColor;
			m_vertexWk[2].diffuse = m_vColor;
			m_vertexWk[3].diffuse = m_vColor;
			// 頂点データを上書き
			memcpy_s(msr.pData, sizeof(m_vertexWk), m_vertexWk, sizeof(m_vertexWk));
			// 頂点データをアンロックする
			pDeviceContext->Unmap(m_pVertexBuffer, 0);
			// フラグをクリア
			m_bInvalidate = false;
		}
	}
}

// テクスチャの設定
void CPolygon::SetTexture(ID3D11ShaderResourceView* pTexture)
{
	m_pTexture = pTexture;
	m_mTex._44 = (m_pTexture) ? 1.0f : 0.0f;
}

// 表示座標の設定
void CPolygon::SetPos(float fX, float fY)
{
	m_vPos.x = fX;
	m_vPos.y = fY;
}

// 表示サイズの設定
void CPolygon::SetSize(float fScaleX, float fScaleY)
{
	m_vScale.x = fScaleX;
	m_vScale.y = fScaleY;
}

// 表示角度の設定(単位:度)
void CPolygon::SetAngle(float fAngle)
{
	m_vAngle.z = fAngle;
}

// 左上テクスチャ座標の設定 (0.0≦fU＜1.0, 0.0≦fV＜1.0)
void CPolygon::SetUV(float fU, float fV)
{
	m_vPosTexFrame.x = fU;
	m_vPosTexFrame.y = fV;
}

// テクスチャフレームサイズの設定 (0.0＜fWidth≦1.0, 0.0＜fHeight≦1.0)
void CPolygon::SetFrameSize(float fWidth, float fHeight)
{
	m_vSizeTexFrame.x = fWidth;
	m_vSizeTexFrame.y = fHeight;
}

// 頂点カラーの設定
void CPolygon::SetColor(float fRed, float fGreen, float fBlue)
{
	SetColor(XMFLOAT3(fRed, fGreen, fBlue));
}
void CPolygon::SetColor(XMFLOAT3 vColor)
{
	if (vColor.x != m_vColor.x || vColor.y != m_vColor.y || vColor.z != m_vColor.z) {
		m_vColor.x = vColor.x;
		m_vColor.y = vColor.y;
		m_vColor.z = vColor.z;
		m_bInvalidate = true;
	}
}

// 不透明度の設定
void CPolygon::SetAlpha(float fAlpha)
{
	if (fAlpha != m_vColor.w) {
		m_vColor.w = fAlpha;
		m_bInvalidate = true;
	}
}

// 頂点カラーの設定
void CPolygon::SetColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
	SetColor(XMFLOAT4(fRed, fGreen, fBlue, fAlpha));
}
void CPolygon::SetColor(XMFLOAT4 vColor)
{
	if (vColor.x != m_vColor.x || vColor.y != m_vColor.y || vColor.z != m_vColor.z || vColor.w != m_vColor.w) {
		m_vColor = vColor;
		m_bInvalidate = true;
	}
}
