// 直方体 [Box.cpp]
#include "Box.h"
#include "Shader.h"

// 静的メンバ
ID3D11Buffer* CBox::m_pConstantBuffer;	// 定数バッファ
ID3D11VertexShader* CBox::m_pVertexShader;	// 頂点シェーダ
ID3D11InputLayout* CBox::m_pInputLayout;	// 頂点フォーマット
ID3D11PixelShader* CBox::m_pPixelShader;	// ピクセルシェーダ

// シェーダに渡す値
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ワールド×ビュー×射影行列(転置行列)
	XMFLOAT4    vColor;     // 描画色
};


HRESULT CBox::InitShader()
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
	hr = LoadShader("DepthWriteVS", "DepthWritePS", &m_pVertexShader, &m_pInputLayout, &m_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		return hr;
	}

	// コンスタントバッファ作成 変換行列渡し用
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_GLOBAL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	hr = pDevice->CreateBuffer(&cb, nullptr, &m_pConstantBuffer);
	if (FAILED(hr)) {
		return false;
	}

	return hr;
}

// コンストラクタ
CBox::CBox() : CMesh()
{
	m_pVertex = nullptr;
	m_pIndex = nullptr;
	m_nVertexNum = 0;
	m_nIndexNum = 0;
	m_bTrigger = false;
	m_pMiniMapCamera = nullptr;
}

// シェーダ終了処理
void CBox::FinShader()
{
	// 定数バッファの解放
	SAFE_RELEASE(m_pConstantBuffer);
	// ピクセルシェーダ解放
	SAFE_RELEASE(m_pPixelShader);
	// 頂点フォーマット解放
	SAFE_RELEASE(m_pInputLayout);
	// 頂点シェーダ解放
	SAFE_RELEASE(m_pVertexShader);
}


// デストラクタ
CBox::~CBox()
{
	SAFE_DELETE_ARRAY(m_pVertex);
	SAFE_DELETE_ARRAY(m_pIndex);
}

// 初期化
HRESULT CBox::Init(XMFLOAT3 vBBox)
{
	HRESULT hr = S_OK;

	SAFE_DELETE_ARRAY(m_pVertex);
	SAFE_DELETE_ARRAY(m_pIndex);


	const static float vtx[] = {
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
	};
	const static float nor[] = {
		 0.0f,  0.0f, -1.0f,
		 1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
	};
	const static int face[] = {
		4, 0, 5, 1, 5, 0,
		5, 1, 7, 3, 7, 1,
		7, 3, 6, 2, 6, 3,
		6, 2, 4, 0, 4, 2,
		0, 2, 1, 3, 1, 2,
		6, 4, 7, 5, 7, 4,
	};
	const static float uv[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};
	m_pVertex = new VERTEX_3D[24];	
	m_pIndex = new UINT[36];
	
	VERTEX_3D* pVtx = m_pVertex;
	int nIdx = 0;
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 4; ++j) {
			pVtx->vtx.x = vtx[face[i * 6 + j] * 3 + 0] * vBBox.x;
			pVtx->vtx.y = vtx[face[i * 6 + j] * 3 + 1] * vBBox.y;
			pVtx->vtx.z = vtx[face[i * 6 + j] * 3 + 2] * vBBox.z;
			pVtx->nor.x = nor[i * 3 + 0];
			pVtx->nor.y = nor[i * 3 + 1];
			pVtx->nor.z = nor[i * 3 + 2];
			pVtx->tex.x = uv[j * 2 + 0];
			pVtx->tex.y = uv[j * 2 + 1];
			pVtx->diffuse = XMFLOAT4(1, 1, 1, 1);
			++pVtx;
			m_pIndex[i * 6 + j] = nIdx;
			++nIdx;
		}
		m_pIndex[i * 6 + 4] = m_pIndex[i * 6 + 2];
		m_pIndex[i * 6 + 5] = m_pIndex[i * 6 + 1];
	}

	m_nVertexNum = 24;
	m_nIndexNum = 36;
	hr = CMesh::Init(m_pVertex, 24, (int*)m_pIndex, 36);

	
	for (int i = 0; i < 8; ++i)
	{
		m_VertexPosArray[i] = 
			XMFLOAT3(
				vtx[i * 3 + 0] * vBBox.x, 
				vtx[i * 3 + 1] * vBBox.y, 
				vtx[i * 3 + 2] * vBBox.z);
	}

	XMStoreFloat(&m_fCenterToVertexDistance ,XMVector3Length(XMVectorSet(vBBox.x, vBBox.y, vBBox.z, 1.0f)));
	
	return hr;
}

void CBox::SetColor(XMFLOAT3 color)
{
	m_vColor = XMFLOAT4(color.x, color.y, color.z, 1.0f);
}

void CBox::DepthDraw()
{
	if (nullptr == m_pMiniMapCamera)return;

	ID3D11DeviceContext* pDC = GetDeviceContext();

	// 使用するシェーダーの登録	
	pDC->VSSetShader(m_pVertexShader, nullptr, 0);
	pDC->PSSetShader(m_pPixelShader, nullptr, 0);
	// 頂点インプットレイアウトをセット
	pDC->IASetInputLayout(m_pInputLayout);
	// プリミティブ形状をセット
	pDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 定数領域更新
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(pDC->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
		SHADER_GLOBAL sg;
		XMMATRIX mtxWorld = XMLoadFloat4x4(&m_mWorld);
		XMMATRIX mtxView = XMLoadFloat4x4(&m_pMiniMapCamera->GetViewMatrix());
		XMMATRIX mtxProj = XMLoadFloat4x4(&m_pMiniMapCamera->GetProjMatrix());
		sg.mWVP = mtxWorld * mtxView * mtxProj;
		sg.mWVP = XMMatrixTranspose(sg.mWVP);
		sg.vColor = m_vColor;
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(sg));
		GetDeviceContext()->Unmap(m_pConstantBuffer, 0);
	}

	pDC->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pDC->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// 頂点バッファ/インデックス バッファをセット
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	pDC->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDC->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 描画
	pDC->DrawIndexed(UINT(m_nIndexNum), 0, 0);
}