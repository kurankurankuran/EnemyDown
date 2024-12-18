//=============================================================================
//
// カメラ クラス [Camera.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "Camera.h"
#include "Input.h"
#include "DebugProc.h"
#include "AssimpModel.h"
#include "Player.h"
#include "Scene.h"
#include "Pause.h"
#include "Text.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// constにしてステージごとに決めるべき？
#define	CAM_POS_P_X			(20000.0f)					// カメラの視点初期位置(X座標)
#define	CAM_POS_P_Y			(20000.0f)				    // カメラの視点初期位置(Y座標)
#define	CAM_POS_P_Z			(20000.0f)				// カメラの視点初期位置(Z座標)

#define	CAM_TARGET_TITLE_X	(  0.0f)			    // タイトルカメラの視点初期位置(X座標)
#define	CAM_TARGET_TITLE_Y	(100.0f)		        // タイトルカメラの視点初期位置(Y座標)
#define	CAM_TARGET_TITLE_Z	(  0.0f)			    // タイトルカメラの視点初期位置(Z座標)

#define CAM_ROTANGLE_ROL        (20.0f)                 // ロールピッチ時のカメラのベクトル回転量
#define CAM_ROTANGLE_PIT        (10.0f)                 // ロールピッチ時のカメラのベクトル回転量
#define CAM_ROTCONFICIENT_ROL   (1.0f / CAM_ROTANGLE_ROL)   // 回転係数
#define CAM_ROTCONFICIENT_PIT   (1.0f / CAM_ROTANGLE_PIT)   // 回転係数

#define	CAM_POS_Y_X			(0.0f)					        // カメラの視点初期位置(X座標)
#define	CAM_POS_Y_Y			(35000.0f)				        // カメラの視点初期位置(Y座標)
#define	CAM_POS_Y_Z			(0.0f)				            // カメラの視点初期位置(Z座標)

namespace {
	const XMFLOAT3	CAM_POS = XMFLOAT3(0, 300.0f, -500.0f);		    // 視点初期位置
	const XMFLOAT3	CAM_TARGET_POS = XMFLOAT3(0, 0, 400.0f);		// 注視点初期位置
	const XMFLOAT3	CAM_ROT = XMFLOAT3(0, 0, 0);					// カメラの初期回転量(X軸)
}

// 静的変数
DirectX::XMFLOAT3 CCamera::m_vStartPos = CAM_POS;		    // 初期視点
DirectX::XMFLOAT3 CCamera::m_vPos = CAM_POS;		        // 視点
DirectX::XMFLOAT3 CCamera::m_vAngle = CAM_ROT;		        // 回転量
DirectX::XMFLOAT3 CCamera::m_vStartTarget = CAM_TARGET_POS;	// 初期注視点
DirectX::XMFLOAT3 CCamera::m_vTarget = CAM_TARGET_POS;	    // 注視点
DirectX::XMFLOAT3 CCamera::m_vUp;		                    // 上方ベクトル
DirectX::XMFLOAT4X4 CCamera::m_mtxWorld;	                // ワールド マトリックス
DirectX::XMFLOAT4X4 CCamera::m_mtxView;	                    // ビュー マトリックス
DirectX::XMFLOAT4X4 CCamera::m_mtxProj;	                    // プロジェクション マトリックス
float CCamera::m_fNearZ = VIEW_NEAR_Z;					    // 前方クリップ距離
float CCamera::m_fFarZ = VIEW_FAR_Z;					    // 後方クリップ距離
CAssimpModel* CCamera::m_pSky = nullptr;
CGameObj* CCamera::m_pTargetObj = nullptr;
int CCamera::m_nReeticleTextIndex = -1;

// コンストラクタ
CCamera::CCamera()
{
	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);		  // 上方ベクトル
}

// 初期化
void CCamera::Init()
{
	m_fRadius = 300.0f;

	// バレットタイム風カメラの際現在の回転量を保存
	m_fAngle = 0.0f;
	// バレットタイム風カメラの際カメラ座標を算出する為に使用
	m_fFreeBulletMode = XMFLOAT2(2000.0f, 200.0f);
	
	// プロジェクション変換パラメータから視錐台生成
	CreateViewFrustum();

	// 視点/注視点/上方ベクトルからワールド マトリックス算出
	CalcWorldMatrix();

	// カメラのワールドマトリックスから視錐台を移動
	UpdateViewFrustum();

}

// 更新
void CCamera::Update()
{
	//// プレイヤーがセットされていたらプレイヤー基準カメラ
	//switch (m_nMode) {
	//	//****************************************
	//	// 定点カメラ
	//	//****************************************
	//case STATIC_CAMERAMODE:
	//	if (m_pTargetObj) {
	//		m_vTarget = m_pTargetObj->GetTransform()->GetPos();
	//	}
	//	m_vPos = XMFLOAT3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);
	//	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

	//	break;
	//	//****************************************
	//	// 定点カメラ(タイトル画面)
	//	//****************************************
	//case STATIC_TITLE_CAMERAMODE:
	//	m_vTarget = XMFLOAT3(CAM_TARGET_TITLE_X, CAM_TARGET_TITLE_Y, CAM_TARGET_TITLE_Z);
	//	m_vPos = XMFLOAT3(CAM_POS_TITLE_X, CAM_POS_TITLE_Y, CAM_POS_TITLE_Z);
	//	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//	break;
	//	//****************************************
	//	// 一人称カメラ
	//	//****************************************
	//case FPS_CAMERAMODE:
	//{
	//	
	//}

	//break;
	////****************************************
	//// バレットタイム風カメラ
	////****************************************
	//case BULLET_CAMERAMODO:
	//	m_fAngle += 0.1f;
	//	if (!m_pTargetObj)break;
	//	m_vTarget = m_pTargetObj->GetTransform()->GetPos();
	//	m_vTarget.y += 50.0f;

	//	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//	m_vPos.x = m_vTarget.x + sinf(XMConvertToRadians(m_fAngle)) * (m_fRadius /** 20.0f*/);
	//	m_vPos.y = m_vTarget.y;
	//	m_vPos.z = m_vTarget.z + cosf(XMConvertToRadians(m_fAngle)) * (m_fRadius /** 20.0f*/);
	//	break;
	//	//****************************************
	//	// 三人称カメラ
	//	//****************************************
	//case TPS_CAMERAMODE:
	//{
	//	

	//	// *******************************************************************************************************
	//	break;
	//}
	////****************************************
	//// エディターカメラ
	////****************************************
	//case EDITOR_CAMERAMODE:
	//{


	//	break; 
	//}
	////****************************************
	//// バレットタイム風カメラ(自由箇所)
	////****************************************
	//case BULLET_FREE_CAMERAMODE:
	//	m_fAngle += 0.5f;

	//	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//	m_vPos.x = m_vTarget.x + sinf(XMConvertToRadians(m_fAngle)) * (m_fFreeBulletMode.x);
	//	m_vPos.y = m_vTarget.y + m_fFreeBulletMode.y;
	//	m_vPos.z = m_vTarget.z + cosf(XMConvertToRadians(m_fAngle)) * (m_fFreeBulletMode.x);
	//	break;
	//}
		
	
	// マトリックス更新
	UpdateMatrix();

	// ワールドマトリックス更新、視錐台更新
	CalcWorldMatrix();
	UpdateViewFrustum();

	// デバッグ表示
	CDebugProc::Print("*** ｶﾒﾗ ｿｳｻ ***\n");
	CDebugProc::Print("FP View:[1]\nBullet-time like:[2]\nTP View:[3]\n:TPBack View[4]\nFixed point:[0]\n");
	CDebugProc::Print("[ｶﾒﾗ ｲﾁ :\n(%f, %f, %f)]\n", m_vPos.x, m_vPos.y, m_vPos.z);
	CDebugProc::Print("[ﾁｭｳｼﾃﾝ :\n(%f, %f, %f)]\n", m_vTarget.x, m_vTarget.y, m_vTarget.z);
	CDebugProc::Print("\n");
}

// 画面クリア
void CCamera::Clear()
{
	// main.cppのリソース参照
	extern ID3D11RenderTargetView* g_pRenderTargetView;
	extern ID3D11DepthStencilView* g_pDepthStencilView;

	float ClearColor[4] = { 0.117647f, 0.254902f, 0.352941f, 1.0f };
	ID3D11DeviceContext* pDC = GetDeviceContext();
	pDC->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	pDC->ClearDepthStencilView(g_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (m_pSky) {
		SetZBuffer(false);
		m_pSky->SetCamera(this);
		CLight lightOff;
		lightOff.SetDir(XMFLOAT3(0, 0, 0));
		m_pSky->SetLight(&lightOff);
		XMFLOAT4X4 mWorld;
		XMStoreFloat4x4(&mWorld, XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z));
		m_pSky->Draw(pDC, mWorld);
	}

	SetZBuffer(true);
	SetBlendState(BS_NONE);
}

// ビュー/プロジェクション マトリックス更新
void CCamera::UpdateMatrix()
{
	XMStoreFloat4x4(&m_mtxView, XMMatrixLookAtLH(
		XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp)));
	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovY), m_fAspectRatio, m_fNearZ, m_fFarZ));
}

// ワールド マトリックス設定
void CCamera::SetWorldMatrix(XMFLOAT4X4& mtxWorld)
{
	m_mtxWorld = mtxWorld;
	m_vPos = XMFLOAT3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
	m_vTarget = XMFLOAT3(mtxWorld._41 + mtxWorld._31, mtxWorld._42 + mtxWorld._32, mtxWorld._43 + mtxWorld._33);
	m_vUp = XMFLOAT3(mtxWorld._21, mtxWorld._22, mtxWorld._23);
}

// 視点/注視点/上方ベクトルからワールド マトリックス算出
XMFLOAT4X4& CCamera::CalcWorldMatrix()
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

//// プレイヤーをセット
//void CCamera::SetPlayer(CPlayer* pPlayer)
//{
//	m_pPlayer = pPlayer;
//	//m_fRadius = (pPlayer) ? pPlayer->GetRadius() : 0.5f;
//}

// ターゲットオブジェクトをセット
void CCamera::SetTargetObj(CGameObj* pObj)
{
	m_pTargetObj = pObj;
}

// 可視判定(0:非表示, -1:部分, 1:表示)
int CCamera::CollisionViewFrustum(XMFLOAT3 vCenter,
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
void CCamera::CreateViewFrustum()
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
void CCamera::UpdateViewFrustum()
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
void CCamera::SetFovY(float fFovY)
{
	m_fFovY = fFovY;
	CreateViewFrustum();
}

// アスペクト比設定
void CCamera::SetAspectRatio(float fAspect)
{
	m_fAspectRatio = fAspect;
	CreateViewFrustum();
}

// 前後クリップ距離設定
void CCamera::SetRangeZ(float fNearZ, float fFarZ)
{
	m_fNearZ = fNearZ, m_fFarZ = fFarZ;
	CreateViewFrustum();
}
