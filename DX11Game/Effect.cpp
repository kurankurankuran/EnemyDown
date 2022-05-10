// エフェクト [Effect.cpp]
#include "Effect.h"
#include "Polygon.h"
#include "Texture.h"

#include "Player.h"



namespace {
	/*const LPCWSTR g_pszPathTex = L"data/texture/FiterSmoke.png";*/
	const LPCWSTR g_pszPathTex = L"data/texture/smoke.png";
	const CMeshMaterial g_material = {
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.3f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		1.0f
	};
}



// コンストラクタ
CEffect::CEffect() : CMesh()
{
	
	/*m_vPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_fAlpha = 1.0f;*/
}

// デストラクタ
CEffect::~CEffect()
{
}

// 初期化
HRESULT CEffect::Init(CScene* pScene)
{
	HRESULT hr = S_OK;
	
	/*m_vScale = XMFLOAT3(250.0f, 250.0f, 1.0f);
	m_vPos = XMFLOAT3(0.0f, 300.0f, 0.0f);
	m_fAlpha = 1.0f;*/

	m_pScene = pScene;


	const static float vtx[] = {
		-0.5f, -0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
	};
	const static float nor[] = {
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
	};

	const static int Index[] = {
		0, 1, 2, 3
		
	};
	
	const static float uv[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};
	VERTEX_3D* pVertexWk = new VERTEX_3D[4];
	int* pIndexWk = new int[4];
	VERTEX_3D* pVtx = pVertexWk;
	int nIdx = 0;

	for (int j = 0; j < 4; ++j) {
		pVtx->vtx.x = vtx[Index[j] * 3 + 0];
		pVtx->vtx.y = vtx[Index[j] * 3 + 1];
		pVtx->vtx.z = vtx[Index[j] * 3 + 2];
		pVtx->nor.x = nor[j * 3 + 0];
		pVtx->nor.y = nor[j * 3 + 1];
		pVtx->nor.z = nor[j * 3 + 2];
		pVtx->tex.x = uv[j * 2 + 0];
		pVtx->tex.y = uv[j * 2 + 1];
		pVtx->diffuse = XMFLOAT4(1, 1, 1, 1);
		++pVtx;
		//pIndexWk[/*i * 6 + */j] = nIdx;
		//++nIdx;
	}
	for (int i = 0; i < 4; ++i) {
		pIndexWk[i] = Index[i];
	}

	hr = CMesh::Init(pVertexWk, 4, pIndexWk, 4);
	delete[] pIndexWk;
	delete[] pVertexWk;



	ID3D11Device* pDevice = GetDevice();

	hr = CreateTextureFromFile(pDevice, g_pszPathTex, &m_pTex);
	if (FAILED(hr)) {
		Fin();
		return false;
	}
	SetTexture(m_pTex);

	m_primitiveType = PT_UNDEFINED;

	return hr;
}

void CEffect::Fin()
{
	//SAFE_RELEASE(m_pTex);

	CMesh::Fin();
}

void CEffect::Update()
{
	
	if (m_EffectPar.empty())return;

	for (auto itr = m_EffectPar.begin(); itr != m_EffectPar.end(); ++itr) {
		if (itr->m_fAlpha > 0.0f) {
			itr->m_fAlpha -= 1.0f / 500.0f;
		}
		else
		{
			itr = m_EffectPar.erase(itr);
			if (m_EffectPar.empty())return;
		}
		
	}
	
}

void CEffect::Draw()
{
	if (m_EffectPar.empty())return;

	// 加算合成
	SetBlendState(BS_ADDITIVE);
	// 半透明合成
	//SetBlendState(BS_ALPHABLEND);
	//SetZWrite(false);				// Zバッファ更新しない

	// ここでオフにする
	m_pScene->GetLight()->SetEnable(false);

	SetCamera(m_pScene->GetCameraManager()->GetCamera());
	SetLight(m_pScene->GetLight());
	

	//ID3D11Device* pDevice = GetDevice();

	// ビューマトリックスを取得
	XMFLOAT4X4& mtxView = m_pScene->GetCameraManager()->GetCamera()->GetViewMatrix();
	XMFLOAT4X4* pWorld = GetWorld();
	XMMATRIX mtxWorld, mtxScl, mtxTranslate;

	for (auto itr = m_EffectPar.begin(); itr != m_EffectPar.end(); ++itr) {
		
		// ビルボード処理
		mtxWorld = XMMatrixIdentity();
		XMStoreFloat4x4(pWorld, mtxWorld);

		pWorld->_11 = mtxView._11;
		pWorld->_12 = mtxView._21;
		pWorld->_13 = mtxView._31;
		pWorld->_21 = mtxView._12;
		pWorld->_22 = mtxView._22;
		pWorld->_23 = mtxView._32;
		pWorld->_31 = mtxView._13;
		pWorld->_32 = mtxView._23;
		pWorld->_33 = mtxView._33;


		// パラメータ設定
		mtxWorld = XMLoadFloat4x4(pWorld);

		mtxScl = XMMatrixScaling(itr->m_vScale.x, itr->m_vScale.y, 1.0f);
		mtxWorld = XMMatrixMultiply(mtxScl, mtxWorld);

		mtxTranslate = XMMatrixTranslation(itr->m_vPos.x, itr->m_vPos.y, itr->m_vPos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		XMStoreFloat4x4(pWorld, mtxWorld);

		SetWorld(pWorld);


		// テクスチャ
		switch (itr->m_Type)
		{
		case SMOKE_EFFECT:
			SetTexture(m_pTex);
			break;

		default:
			//SetTexture(m_pTex);
			break;
		}
		
		// 透過度
		SetDiffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, itr->m_fAlpha));

		CMesh::Draw();
	}
	
	
	//SetZWrite(true);				// Zバッファ更新する
	// ここでオンにする
	m_pScene->GetLight()->SetEnable(true);

	SetBlendState(BS_NONE);		// アルファブレンド無効
	// 半透明合成
	//SetBlendState(BS_ALPHABLEND);
}



void CEffect::SetEffect(EFFECT_TYPE nType, XMFLOAT3 pos, XMFLOAT2 size)
{
	EffectPar _EffectPar;

	_EffectPar.m_Type = nType;
	_EffectPar.m_fAlpha = 1.0f;
	_EffectPar.m_vPos = pos;
	_EffectPar.m_vScale = size;



	m_EffectPar.push_back(_EffectPar);
}