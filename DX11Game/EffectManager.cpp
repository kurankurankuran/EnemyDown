//=============================================================================
//
// 各種エフェクト処理 [EffectManager.cpp]
// Author : KAZUKI KURA
//
//=============================================================================



#include "EffectManager.h"
#include "polygon.h"
#include "Texture.h"
#include "Scene.h"

// 静的メンバ
std::list<EFFECT>	CEffectManager::m_Effect;	                // エフェクト情報


//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct {
	XMFLOAT2	vSize;			// 表示サイズ
	SIZE		sFrame;			// 分割数
	int         nCount;         // １フレームごとの表示時間カウンタ 
} EFFECT_TYPE;

//*****************************************************************************
// 定数定義
//*****************************************************************************
namespace {
	LPCWSTR g_pszPathTex[MAX_EFFECTTYPE] = {
		L"data/texture/dead000.png",
		L"data/texture/smoke.png",
	};
	EFFECT_TYPE	g_effectType[MAX_EFFECTTYPE] = {
	{{128.0f, 128.0f}, {4, 4},4},
	{{1.0f, 1.0f}, {1, 1},0},
	};
	const float DelAlpha = 1.0f / 500.0f;
}


CEffectManager::CEffectManager(CScene* pScene):CGameObj(pScene)
{

}

CEffectManager::~CEffectManager()
{

}


//=============================================================================
// エフェクトの初期化処理
//=============================================================================
HRESULT CEffectManager::Init()
{
	HRESULT hr = S_OK;

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
	// テクスチャ読み込み
	for (int i = 0; i < MAX_EFFECTTYPE; ++i) {
		m_pTexture.push_back(nullptr);
		hr = CreateTextureFromFile(
			pDevice,					// デバイスへのポインタ
			g_pszPathTex[i],		    // ファイルの名前
			&m_pTexture[i]);			// 読み込むメモリー
		if (FAILED(hr)) {
			return hr;
		}
	}

	SetCamera(m_pScene->GetCameraManager()->GetCamera());
	SetLight(m_pScene->GetLight());

	return hr;
}

//=============================================================================
// エフェクトの終了処理
//=============================================================================
void CEffectManager::Fin()
{
	// テクスチャ開放
	for (int i = 0; i < MAX_EFFECTTYPE; ++i) {
		SAFE_RELEASE(m_pTexture[i]);
	}

	m_Effect.clear();
}

//=============================================================================
// エフェクトの更新処理
//=============================================================================
void CEffectManager::Update()
{
	// エフェクトの更新
	EFFECT_TYPE* pEffectType;
	
	for (auto itr = m_Effect.begin(); itr != m_Effect.end(); ) {
		

		switch (itr->nType)
		{
		case EFFECT_SMOKE:
			itr->fAlpha -= DelAlpha;
			if (itr->fAlpha <= 0)
			{
				itr = m_Effect.erase(itr);
				continue;
			}
			++itr;
			continue;
		}

		// カウンタ更新
		--itr->nCount;
		if (itr->nCount <= 0) {
			// アニメーション更新
			++itr->nAnim;
			pEffectType = &g_effectType[itr->nType];
			if (itr->nAnim >= pEffectType->sFrame.cx * pEffectType->sFrame.cy) {	// 終端
				if (itr->bLoop)
				{
					itr->nAnim -= g_effectType[itr->nType].sFrame.cx * g_effectType[itr->nType].sFrame.cy;
				}
				else
				{
					itr = m_Effect.erase(itr);
					continue;
				}
				
			}
			// カウンタ初期化
			itr->nCount = g_effectType[itr->nType].nCount;
		}


		++itr;
	}
}

//=============================================================================
// エフェクトの描画処理
//=============================================================================
void CEffectManager::Draw()
{
	// 加算合成
	SetBlendState(BS_ADDITIVE);

	// ここでオフにする
	m_pScene->GetLight()->SetEnable(false);

	// エフェクト描画
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	for (auto itr = m_Effect.begin(); itr != m_Effect.end();) {
		
		SetFrameSize(1.0f/ g_effectType[itr->nType].sFrame.cx, 1.0f / g_effectType[itr->nType].sFrame.cy);
		int u, v;
		u = itr->nAnim % g_effectType[itr->nType].sFrame.cx;
		v = itr->nAnim / g_effectType[itr->nType].sFrame.cx;
		SetUV(u / (float)g_effectType[itr->nType].sFrame.cx, v / (float)g_effectType[itr->nType].sFrame.cy);
		// 透過度
		SetDiffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, itr->fAlpha));
		SetTexture(m_pTexture[itr->nType]);

		// ビューマトリックスを取得
		XMFLOAT4X4& mtxView = m_pScene->GetCameraManager()->GetCamera()->GetViewMatrix();
		XMFLOAT4X4* pWorld = CMesh::GetWorld();
		XMMATRIX mtxWorld, mtxScl, mtxTranslate;

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

		mtxScl = XMMatrixScaling(itr->vScl.x, itr->vScl.y, 1.0f);
		mtxWorld = XMMatrixMultiply(mtxScl, mtxWorld);

		mtxTranslate = XMMatrixTranslation(itr->vPos.x, itr->vPos.y, itr->vPos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		XMStoreFloat4x4(pWorld, mtxWorld);

		CMesh::SetWorld(pWorld);
		CMesh::Draw();

		++itr;
	}
	
	SetFrameSize(1.0f, 1.0f);
	SetUV(0.0f, 0.0f);

	// ここでオンにする
	m_pScene->GetLight()->SetEnable(true);

	SetBlendState(BS_NONE);		// アルファブレンド無効
}

void CEffectManager::StartEffect(EffectType EffectType, XMFLOAT3 pos, XMFLOAT2 scl, bool bLoop)
{
	EFFECT _Effect;
	_Effect.bLoop = bLoop;
	_Effect.fAlpha = 1.0f;
	_Effect.fAngle = 0.0f;
	_Effect.fZoom = 0.0f;
	_Effect.nAnim = 0;
	_Effect.nCount = g_effectType[EffectType].nCount;
	_Effect.nType = EffectType;
	_Effect.vPos = pos;
	_Effect.vScl = scl;
	

	m_Effect.push_back(_Effect);
}
