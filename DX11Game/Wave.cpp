//=============================================================================
//
// 各種エフェクト処理 [EffectManager.cpp]
// Author : KAZUKI KURA
//
//=============================================================================



#include "Wave.h"
#include "polygon.h"
#include "Texture.h"

//*****************************************************************************
// 定数定義
//*****************************************************************************
namespace {
	
	//LPCWSTR g_pszPathTex = L"data/texture/suimen.jpg";
	LPCWSTR g_pszPathTex = L"data/texture/BG00a0_80.jpg";

	
	

	
	

	
	const float WAVE_TIMER      =       (0.05f)     ;   // ウェーブポイントの時間速度
    const int   WAVE_PITCH	    =		(40)		;	// ウェーブポイントの間隔
    const int   WAVE_QTY		=		(120)		;	// ウェーブポイントの総数
	const float WAVE_AMPLITUDE	=       (20.0f)     ;	// ウェーブポイントの振幅
	const float WAVE_LENGTH     =       (140.0f)    ;	// ウェーブポイントの波長
	const float WAVE_CYCLE      =       (7.0f)      ;	// ウェーブポイントの周期
	const float WAVE_DEL        =       (0.8f)      ;   // 振幅の減衰値
	const float AXIS_X_OFFSET   =       (0)         ;	// X座標オフセット
	const float AXIS_Y_OFFSET   =       (-100.0f)   ;	// Y座標オフセット
	const float AXIS_Z_OFFSET   =       (2000)      ;	// Z座標オフセット
	const float TEXTURE_WIDTH   =       (500.0f)    ;   // 仮想テクスチャの横幅
	const float TEXTURE_HEIGHT  =       (500.0f)    ;   // 仮想テクスチャの縦幅
	const float TEXTURE_POS_Y   =       (-500.0f)   ;   // 仮想テクスチャのY座標
}


CWave::CWave(CScene* pScene)
{
	m_pScene = pScene;
	m_pTexture = nullptr;
	m_pVertex = nullptr;
}

CWave::~CWave()
{

}


//=============================================================================
// エフェクトの初期化処理
//=============================================================================
HRESULT CWave::Init()
{
	HRESULT hr = S_OK;

	m_pWaveParam = new CWaveParam[WAVE_QTY * WAVE_QTY];		// ウェーブ情報

	const static unsigned int vtxXNum = WAVE_QTY * 3;
	const static unsigned int vtxYNum = WAVE_QTY;
	float vtx[vtxYNum][vtxXNum];
	float nor[vtxYNum][vtxXNum];


	const static unsigned int indexXNum = (WAVE_QTY - 1) * 4;
	const static unsigned int indexYNum = WAVE_QTY - 1;
	unsigned int index[indexYNum + 1][indexXNum + 1];

	const static unsigned int uvXNum = WAVE_QTY * 2;
	const static unsigned int uvYNum = WAVE_QTY;
	float uv[uvYNum][uvXNum];

	float posX  = -1.0f / WAVE_QTY;
	float posZ  = -1.0f / WAVE_QTY;
	float pitch = WAVE_PITCH ;
	
	CWaveParam* pWaveWk = m_pWaveParam;
	for (int j = 0; j < WAVE_QTY; ++j)
	{
		posX = -1.0f / WAVE_QTY;
		posZ = -1.0f / WAVE_QTY;
		for (int i = 0; i < WAVE_QTY; ++i)
		{
			XMFLOAT2 vPos = XMFLOAT2(posX + i * pitch, posZ + j * pitch);
			
			 pWaveWk->position.x = (float)((i - WAVE_QTY / 2) * WAVE_PITCH);	// Ｘ座標
			 pWaveWk->position.y = 0.0f;										// Ｙ座標
			 pWaveWk->position.z = (float)((j - WAVE_QTY / 2) * WAVE_PITCH);	// Ｚ座標
			  	   		
			 pWaveWk->amplitude = WAVE_AMPLITUDE;	// 振幅
			 pWaveWk->time = 0.0f;
			++ pWaveWk;

			vtx[j][i * 3 + 0] = vPos.x;
			vtx[j][i * 3 + 1] = 0.0f;
			vtx[j][i * 3 + 2] = vPos.y;

			nor[j][i * 3 + 0] = 0.0f;
			nor[j][i * 3 + 1] = 1.0f;
			nor[j][i * 3 + 2] = 0.0f;

			uv[j][i * 2 + 0] = (1.0f / (WAVE_QTY - 1)) * i;
			uv[j][i * 2 + 1] = (1.0f / (WAVE_QTY - 1)) * j;

			if (j < WAVE_QTY - 1)
			{
				index[j][i * 4 + 0] = i + WAVE_QTY * j;
				index[j][i * 4 + 1] = i + WAVE_QTY * (j + 1);
				index[j][i * 4 + 2] = i + 1 + WAVE_QTY * j;
				index[j][i * 4 + 3] = i + 1 + WAVE_QTY * (j + 1);
			}
		}
	}
	

	int nVertexNum = WAVE_QTY * WAVE_QTY;
	int nIndexNum = indexXNum * indexYNum;

	//VERTEX_3D* pVertexWk = new VERTEX_3D[nVertexNum];
	m_pVertex = new VERTEX_3D[nVertexNum];

	m_pIndex = new int[nIndexNum + indexYNum];
	VERTEX_3D* pVtx = m_pVertex;
	int nIdx = 0;

	for (int j = 0; j < WAVE_QTY; ++j) {
		for (int i = 0; i < WAVE_QTY; ++i)
		{
			pVtx->vtx.x = vtx[j][i * 3 + 0];
			pVtx->vtx.y = vtx[j][i * 3 + 1];
			pVtx->vtx.z = vtx[j][i * 3 + 2];
			pVtx->nor.x = nor[j][i * 3 + 0];
			pVtx->nor.y = nor[j][i * 3 + 1];
			pVtx->nor.z = nor[j][i * 3 + 2];
			pVtx->tex.x =  uv[j][i * 2 + 0];
			pVtx->tex.y =  uv[j][i * 2 + 1];
			pVtx->diffuse = XMFLOAT4(1, 1, 1, 1);
			++pVtx;
		}
	}

	int* saveIndex = m_pIndex;
	for (unsigned int j = 0; j < indexYNum; ++j) {
		for (unsigned int i = 0; i < indexXNum; ++i) {
			*m_pIndex = index[j][i];
			m_pIndex++;
		}
	}
	m_pIndex = saveIndex;
	hr = CMesh::Init(m_pVertex, nVertexNum, m_pIndex, nIndexNum);
	//delete[] pIndexWk;
	//delete[] pVertexWk;




	ID3D11Device* pDevice = GetDevice();
	// テクスチャ読み込み
	hr = CreateTextureFromFile(
		pDevice,					// デバイスへのポインタ
		g_pszPathTex,		// ファイルの名前
		&m_pTexture);			// 読み込むメモリー
	if (FAILED(hr)) {
		return hr;
	}

	SetCamera(m_pScene->GetCameraManager()->GetCamera());
	SetLight(m_pScene->GetLight());


	return hr;
}

//=============================================================================
// エフェクトの終了処理
//=============================================================================
void CWave::Fin()
{
	// テクスチャ開放
	SAFE_RELEASE(m_pTexture);

	// インデックスデータ削除
	SAFE_DELETE_ARRAY(m_pIndex);
	// 頂点データ削除
	SAFE_DELETE_ARRAY(m_pVertex);
	SAFE_DELETE_ARRAY(m_pWaveParam);

}

//=============================================================================
// エフェクトの更新処理
//=============================================================================
void CWave::Update()
{

	VERTEX_3D* pVtx = m_pVertex;

	float dx, dz, length, n;
	float fDel;
	CWaveParam* pWaveWk = m_pWaveParam;
	CWaveParam* pWaveCenter = m_pWaveParam + (WAVE_QTY / 2) * WAVE_QTY + WAVE_QTY / 2;

	CCamera* pCamera = m_pScene->GetCameraManager()->GetCamera();
	XMFLOAT3 vCameraPos = pCamera->GetPos();
	XMFLOAT3 vView;
	XMVECTOR vecRef;					// 屈折ベクトル
	XMVECTOR vecTmp;					// テンポラリ
	XMVECTOR vecBGNor;				// ＢＧ面の法線ベクトル
	XMVECTOR vecPos;					// 屈折ベクトルとテクスチャ面との交点
	float		ref_rate_v, ref_rate_t;	// 屈折ベクトル計算用一時変数
	float		rrv_rrt, rrv2_rrt2;
	float		ans_InnerProduct;
	float		tmp0;
	for (int j = 0; j < WAVE_QTY; ++j)
	{
		for (int i = 0; i < WAVE_QTY; ++i)
		{
			dx = pWaveWk->position.x - pWaveCenter->position.x;// xの差分
			dz = pWaveWk->position.z - pWaveCenter->position.z;// zの差分
			length = sqrtf(dx * dx + dz * dz);

			// 減衰
			dx = (float)(WAVE_QTY / 2 * WAVE_PITCH);
			dz = (float)(WAVE_QTY / 2 * WAVE_PITCH);// yの差分
			fDel = 1.0f - length / sqrtf(dx * dx + dz * dz);

			// 距離を元に振動の式から変位を求める
			pWaveWk->position.y = pWaveWk->amplitude * sinf(2.0f * XM_PI * (length / WAVE_LENGTH - pWaveWk->time / WAVE_CYCLE)) * fDel * WAVE_DEL;

			// 時間を進める
			pWaveWk->time += WAVE_TIMER;

			pVtx->vtx.x = pWaveWk->position.x;
			pVtx->vtx.y = pWaveWk->position.y;
			pVtx->vtx.z = pWaveWk->position.z;

			// 法線を計算
			// 頂点から波動の中心方向ベクトルとY軸正方向ベクトルの外積で法線の回転軸を求める
			XMVECTOR vecAxis = XMVector3Cross(
				XMVectorSet(
					pWaveCenter->position.x - pVtx->vtx.x,
					pWaveCenter->position.y - pVtx->vtx.y,
					pWaveCenter->position.z - pVtx->vtx.z, 1.0f),
				XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)
			);

			if (!(XMVectorGetX(vecAxis) == 0 && XMVectorGetY(vecAxis) == 0 && XMVectorGetZ(vecAxis) == 0))
			{
				vecAxis = XMVector3Normalize(vecAxis);

				// 頂点における法線の傾き度を求める
				float fRotValue = (2.0f * XM_PI) / WAVE_CYCLE * cosf(2.0f * XM_PI * (length / WAVE_LENGTH - pWaveWk->time / WAVE_CYCLE)) * fDel;

				// 法線を傾ける
				//自由軸回転マトリクス
				XMMATRIX mtxRot = XMMatrixRotationAxis(vecAxis, fRotValue);
				XMStoreFloat3(&pVtx->nor, XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), mtxRot));
			}

			// UVを設定
			vView.x = vCameraPos.x - pVtx->vtx.x;
			vView.y = vCameraPos.y - pVtx->vtx.y;
			vView.z = vCameraPos.z - pVtx->vtx.z;
			XMStoreFloat3(&vView, XMVector3Normalize(XMLoadFloat3(&vView)));

			ref_rate_v = 1.0f;
			ref_rate_t = 1.2f;

			rrv_rrt = ref_rate_v / ref_rate_t;//nL/nT
			rrv2_rrt2 = (ref_rate_v * ref_rate_v) / (ref_rate_t * ref_rate_t); //(nL^2/nT^2)

			XMStoreFloat(&ans_InnerProduct, XMVector3Dot(XMLoadFloat3(&pVtx->nor), XMLoadFloat3(&vView))); // N・L

			tmp0 = rrv_rrt * ans_InnerProduct -
				sqrtf(1.0f - rrv2_rrt2 * (1.0f - ans_InnerProduct * ans_InnerProduct));// 最終式の()の中

			vecRef = XMVectorSet(
				tmp0 * pVtx->nor.x - rrv_rrt * vView.x,
				tmp0 * pVtx->nor.y - rrv_rrt * vView.y,
				tmp0 * pVtx->nor.z - rrv_rrt * vView.z,
				1.0f);

			// 屈折ベクトルの線上とテクスチャ面との交点を求める
			vecTmp = XMVectorSet(
				0.0f          - pVtx->vtx.x,
				TEXTURE_POS_Y - pVtx->vtx.y,
				0.0f          - pVtx->vtx.z,
				1.0f);

			// ＢＧのテクスチャ面の法線ベクトル
			vecBGNor = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
			XMStoreFloat(&n, XMVector3Dot(vecTmp, vecBGNor) / XMVector3Dot(vecRef, vecBGNor));
			vecPos = XMLoadFloat3(&(pVtx->vtx)) + n * vecRef;

			// 交点座標を実際のテクスチャ座標に変換
			pVtx->tex.x = 0.5f + (XMVectorGetX(vecPos)) / TEXTURE_WIDTH;
			pVtx->tex.y = 0.5f + (XMVectorGetZ(vecPos)) / TEXTURE_HEIGHT;


			float TestU = pVtx->tex.x;
			float TestV = pVtx->tex.y;

			int test;
			test = 0;


			++pWaveWk;
			++pVtx;
		}
	}


	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapResource;

	// データコピー
	hr = pContext->Map(GetVertexBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
	if (SUCCEEDED(hr))
	{
		rsize_t size = (WAVE_QTY * WAVE_QTY) * sizeof(VERTEX_3D);
		memcpy_s(mapResource.pData, size, m_pVertex, size);
		pContext->Unmap(GetVertexBuffer(), 0);
	}

}

//=============================================================================
// エフェクトの描画処理
//=============================================================================
void CWave::Draw()
{
	// 加算合成
	//SetBlendState(BS_ADDITIVE);

	// ここでオフにする
	m_pScene->GetLight()->SetEnable(false);

	// エフェクト描画
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	SetFrameSize(1.0f, 1.0f);

	SetUV(1.0f, 1.0f);

	SetDiffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetTexture(m_pTexture);

	XMFLOAT4X4* pWorld = CMesh::GetWorld();
	XMMATRIX mtxWorld, mtxScl, mtxTranslate;


	mtxWorld = XMMatrixIdentity();
	XMStoreFloat4x4(pWorld, mtxWorld);

	// パラメータ設定
	mtxWorld = XMLoadFloat4x4(pWorld);

	mtxScl = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	mtxWorld = XMMatrixMultiply(mtxScl, mtxWorld);

	mtxTranslate = XMMatrixTranslation(
		AXIS_X_OFFSET,
		AXIS_Y_OFFSET,
		AXIS_Z_OFFSET);


	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	XMStoreFloat4x4(pWorld, mtxWorld);

	CMesh::SetWorld(pWorld);
	CMesh::Draw();

	
		

	SetFrameSize(1.0f, 1.0f);
	SetUV(0.0f, 0.0f);

	// ここでオンにする
	m_pScene->GetLight()->SetEnable(true);

	SetBlendState(BS_NONE);		// アルファブレンド無効
}
//
//void CWave::StartEffect(EffectType EffectType, XMFLOAT3 pos, XMFLOAT2 scl, bool bLoop)
//{
//	EFFECT _Effect;
//	_Effect.bLoop = bLoop;
//	_Effect.fAlpha = 1.0f;
//	_Effect.fAngle = 0.0f;
//	_Effect.fZoom = 0.0f;
//	_Effect.nAnim = 0;
//	_Effect.nCount = g_effectType[EffectType].nCount;
//	_Effect.nType = EffectType;
//	_Effect.vPos = pos;
//	_Effect.vScl = scl;
//
//
//	m_Effect.push_back(_Effect);
//}
