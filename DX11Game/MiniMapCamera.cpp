//=============================================================================
//
// カメラ クラス [Camera.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "MiniMapCamera.h"
#include "Player.h"
#include "Scene.h"
#include "Polygon.h"
#include "Text.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

namespace {

	const XMFLOAT3 CAM_POS_MINIMAP = XMFLOAT3(0, 3000, 0);      // カメラの初期座標
	const XMFLOAT3 CAM_TARGETPOS_MINIMAP = XMFLOAT3(0, 0, 0);   // カメラの注視点座標

}

// コンストラクタ
CMiniMapCamera::CMiniMapCamera()
{
	Init();
}

// 初期化
void CMiniMapCamera::Init()
{
	// プレイヤーポインタ
	m_pTargetObj = nullptr;
	//// カメラモード決定
	//m_nMode = STATIC_CAMERAMODE;

	// 初期位置決定
	m_vPos = CAM_POS_MINIMAP;		// 視点
	m_vTarget = CAM_TARGETPOS_MINIMAP;	// 注視点
	m_vUp = XMFLOAT3(0.0f, 0.0f, 1.0f);		// 上方ベクトル


	// 初期回転量設定
	m_vAngle = XMFLOAT3(0, 0, 0);

	m_fAspectRatio = 1/*VIEW_ASPECT*/;			// 縦横比
	m_fFovY = VIEW_ANGLE;					// 視野角(Degree)
	m_fNearZ = VIEW_NEAR_Z;					// 前方クリップ距離
	m_fFarZ = VIEW_FAR_Z;					// 後方クリップ距離

	// プロジェクション変換パラメータから視錐台生成
	CreateViewFrustum();

	// 視点/注視点/上方ベクトルからワールド マトリックス算出
	CalcWorldMatrix();

	// カメラのワールドマトリックスから視錐台を移動
	UpdateViewFrustum();

	// 方角テキスト
	CText::OutPutText(std::string("N"), XMFLOAT2(500, 345), XMFLOAT2(15, 30), XMFLOAT4(1, 0, 0, 0.5f));
	CText::OutPutText(std::string("S"), XMFLOAT2(500, 205), XMFLOAT2(15, 30), XMFLOAT4(0, 0, 1, 0.5f));
	CText::OutPutText(std::string("W"), XMFLOAT2(425, 275), XMFLOAT2(15, 30), XMFLOAT4(0, 1, 0, 0.5f));
	CText::OutPutText(std::string("E"), XMFLOAT2(575, 275), XMFLOAT2(15, 30), XMFLOAT4(0, 1, 1, 0.5f));

}

// 更新
void CMiniMapCamera::Update()
{
	
	if (m_pTargetObj) {
		m_vTarget = m_pTargetObj->GetTransform()->GetPos();
		m_vPos = XMFLOAT3(m_vTarget.x, m_vTarget.y + 6000.0f, m_vTarget.z);
		/*XMFLOAT4X4 mW = m_pTargetObj->GetTransform()->GetWorld();
		XMFLOAT3 vZ;
		XMStoreFloat3(&vZ, XMVector3Normalize(XMVectorSet(mW._31, mW._32, mW._33, 1.0f)));
		m_vUp = XMFLOAT3(vZ.x, vZ.y, vZ.z);*/


		// マトリックス更新
		UpdateMatrix();

		// ワールドマトリックス更新、視錐台更新
		CalcWorldMatrix();
		UpdateViewFrustum();
	}
}

void CMiniMapCamera::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	SetBlendState(BS_ALPHABLEND);
	SetZWrite(false);				// Zバッファ更新しない
	extern ID3D11ShaderResourceView* g_pMiniMapRSRV;
	CPolygon::SetTexture(g_pMiniMapRSRV);
	CPolygon::SetPos(500.0f, 275.0f);
	CPolygon::SetSize(150.0f, 150.0f);
	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(0.3f, 0.3f, 0.3f);
	CPolygon::SetAlpha(0.5f);
	CPolygon::Draw(pDC);
	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(0.0f, 0.0f, 0.0f);
	SetZWrite(true);							// Zバッファ更新する
	SetBlendState(BS_NONE);		                // アルファブレンド無効


}

// 画面クリア
void CMiniMapCamera::Clear()
{
	// main.cppのリソース参照
	extern ID3D11RenderTargetView* g_pMiniMapRTV;
	extern ID3D11DepthStencilView* g_pMiniMapDSV;

	float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	ID3D11DeviceContext* pDC = GetDeviceContext();
	pDC->ClearRenderTargetView(g_pMiniMapRTV, ClearColor);
	pDC->ClearDepthStencilView(g_pMiniMapDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}



// ビュー/プロジェクション マトリックス更新
void CMiniMapCamera::UpdateMatrix()
{
	XMStoreFloat4x4(&m_mtxView, XMMatrixLookAtLH(
		XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp)));
	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovY), m_fAspectRatio, m_fNearZ, m_fFarZ));
}

// ワールド マトリックス設定
void CMiniMapCamera::SetWorldMatrix(XMFLOAT4X4& mtxWorld)
{
	m_mtxWorld = mtxWorld;
	m_vPos = XMFLOAT3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
	m_vTarget = XMFLOAT3(mtxWorld._41 + mtxWorld._31, mtxWorld._42 + mtxWorld._32, mtxWorld._43 + mtxWorld._33);
	m_vUp = XMFLOAT3(mtxWorld._21, mtxWorld._22, mtxWorld._23);
}

// 視点/注視点/上方ベクトルからワールド マトリックス算出
XMFLOAT4X4& CMiniMapCamera::CalcWorldMatrix()
{
	XMVECTOR vecZ = XMVectorSet(m_vTarget.x - m_vPos.x, m_vTarget.y - m_vPos.y, m_vTarget.z - m_vPos.z, 0.0f);
	XMFLOAT3 vZ;
	XMStoreFloat3(&vZ, XMVector3Normalize(vecZ));
	XMVECTOR vecY = XMLoadFloat3(&m_vUp);
	XMVECTOR vecX = XMVector3Normalize(XMVector3Cross(vecY, vecZ));
	XMFLOAT3 vX;
	XMStoreFloat3(&vX, vecX);
	vecY = XMVector3Normalize(XMVector3Cross(vecZ, vecX));
	XMFLOAT3 vY;
	XMStoreFloat3(&vY, vecY);

	m_mtxWorld._11 = vX.x;
	m_mtxWorld._12 = vX.y;
	m_mtxWorld._13 = vX.z;
	m_mtxWorld._14 = 0.0f;
	m_mtxWorld._21 = vY.x;
	m_mtxWorld._22 = vY.y;
	m_mtxWorld._23 = vY.z;
	m_mtxWorld._24 = 0.0f;
	m_mtxWorld._31 = vZ.x;
	m_mtxWorld._32 = vZ.y;
	m_mtxWorld._33 = vZ.z;
	m_mtxWorld._34 = 0.0f;
	m_mtxWorld._41 = m_vPos.x;
	m_mtxWorld._42 = m_vPos.y;
	m_mtxWorld._43 = m_vPos.z;
	m_mtxWorld._44 = 1.0f;

	return m_mtxWorld;
}

// ターゲットオブジェクトをセット
void CMiniMapCamera::SetTargetObj(CGameObj* pObj)
{
	m_pTargetObj = pObj;
}

// 可視判定(0:非表示, -1:部分, 1:表示)
int CMiniMapCamera::CollisionViewFrustum(XMFLOAT3 vCenter,
	float fRadius)
{
	bool bHit = false;
	XMVECTOR frusw, center, dot;
	float fDot;
	center = XMLoadFloat3(&vCenter);
	for (int i = 0; i < 6; ++i) {
		frusw = XMLoadFloat4(&m_frusw[i]);
		dot = XMPlaneDotCoord(frusw, center);
		XMStoreFloat(&fDot, dot);
		if (fDot < -fRadius)
			return 0;	// 完全に外側
		if (fDot <= fRadius)
			bHit = true;
	}
	if (bHit)
		return -1;	// 部分的に内側
	return 1;	// 完全に内側
}

// プロジェクション変換パラメータから視錐台生成
void CMiniMapCamera::CreateViewFrustum()
{
	// 0:上 1:下 2:左 3:右 4:前 5:後ろ
	float fTan =
		tanf(XMConvertToRadians(m_fFovY) * 0.5f);
	m_frus[0] = XMFLOAT4(0.0f, -1.0f, fTan, 0.0f);
	m_frus[1] = XMFLOAT4(0.0f, 1.0f, fTan, 0.0f);
	fTan *= m_fAspectRatio;
	m_frus[2] = XMFLOAT4(1.0f, 0.0f, fTan, 0.0f);
	m_frus[3] = XMFLOAT4(-1.0f, 0.0f, fTan, 0.0f);
	m_frus[4] = XMFLOAT4(0.0f, 0.0f, 1.0f, -m_fNearZ);
	m_frus[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, m_fFarZ);
	// 0～3を正規化
	for (int i = 0; i < 4; ++i) {
		XMStoreFloat4(&m_frus[i],
			XMPlaneNormalize(
				XMLoadFloat4(&m_frus[i])));
	}
}

// カメラのワールドマトリックスから視錐台を移動
void CMiniMapCamera::UpdateViewFrustum()
{
	// CalcWorldMatrixでm_mtxWorldが更新済を前提
	XMMATRIX mW = XMLoadFloat4x4(&m_mtxWorld);
	mW = XMMatrixInverse(nullptr, mW);//逆行列
	mW = XMMatrixTranspose(mW);//転置行列
	// ※逆行列の転置行列を求めるのは
	//   XMPlaneTransformの仕様!!
	for (int i = 0; i < 6; ++i) {
		XMStoreFloat4(&m_frusw[i],
			XMPlaneTransform(
				XMLoadFloat4(&m_frus[i]), mW));
	}
}

// 視野角設定
void CMiniMapCamera::SetFovY(float fFovY)
{
	m_fFovY = fFovY;
	CreateViewFrustum();
}

// アスペクト比設定
void CMiniMapCamera::SetAspectRatio(float fAspect)
{
	m_fAspectRatio = fAspect;
	CreateViewFrustum();
}

// 前後クリップ距離設定
void CMiniMapCamera::SetRangeZ(float fNearZ, float fFarZ)
{
	m_fNearZ = fNearZ, m_fFarZ = fFarZ;
	CreateViewFrustum();
}

