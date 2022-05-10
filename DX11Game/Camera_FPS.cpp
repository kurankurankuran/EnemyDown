#include"Camera_FPS.h"
#include"Input.h"
#include"Scene.h"
#include"CameraManager.h"
#include"Text.h"
#include"GameObj.h"
namespace {
	const XMFLOAT3 CAM_POS = XMFLOAT3(75.0f, 150.0f, -150.0f);           // プレイヤーを基準とするFPSカメラの相対視点位置

}

CCamera_FPS::CCamera_FPS() : CCamera()
{
	m_fAspectRatio = VIEW_ASPECT;		   	  // 縦横比
	m_fFovY = VIEW_ANGLE;					  // 視野角(Degree)
	Init();                                   // 初期化
}

void CCamera_FPS::Init()
{
	CCamera::Init();
}

void CCamera_FPS::Update()
{
	// ポーズ状態なら更新しない
	if (CScene::GetPause()->GetPauseFlg())return;
	if (!m_pTargetObj)return;

	if (CInput::GetMouseRelease(VK_RBUTTON))
	{
		CScene::GetCurrent()->GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::TPS_CAMERAMODE);
		m_nReeticleTextIndex = CText::DeleteText(m_nReeticleTextIndex);
	}

	XMFLOAT4X4 mtxW = m_pTargetObj->GetTransform()->GetWorld();
	// プレイヤーのワールド行列の回転成分を無効にした行列をかけ合わせる
	mtxW._11 = 1.0f;
	mtxW._12 = 0.0f;
	mtxW._13 = 0.0f;
	mtxW._21 = 0.0f;
	mtxW._22 = 1.0f;
	mtxW._23 = 0.0f;
	mtxW._31 = 0.0f;
	mtxW._32 = 0.0f;
	mtxW._33 = 1.0f;

	XMMATRIX mW = XMLoadFloat4x4(&mtxW);

	// **********カメラ注視点**********
	XMFLOAT3 vPlayerPos = m_pTargetObj->GetTransform()->GetPos();
	// 注視点はプレイヤーの視線の先
	XMStoreFloat3(&m_vTarget, XMVector3TransformCoord(XMVectorSet(m_vStartTarget.x, m_vStartTarget.y, m_vStartTarget.z, 1.0f), mW));
	// プレイヤー座標から注視点座標へのベクトル
	XMVECTOR vecPlayerToTarget = XMVectorAdd(XMLoadFloat3(&m_vTarget), -XMLoadFloat3(&vPlayerPos));
	// マウス操作
	// マウスでプレイヤーを軸にカメラ座標回転
	/*
	１ピクセル辺りの回転量を計算
	横方向は画面の半分の移動量で180°回転、
	*/
	m_vAngle.y += XMConvertToDegrees((CInput::GetMouseMoveX() * XM_PI / (SCREEN_CENTER_X)) * CInput::GetMouseSensitivityX());
	m_vAngle.x += XMConvertToDegrees((CInput::GetMouseMoveY() * XM_PI / (SCREEN_CENTER_Y)) * CInput::GetMouseSensitivityY());
	if (m_vAngle.x < -180 / 3.0f)
	{
		m_vAngle.x = -180 / 3.0f;
	}
	else if (180 / 4.0f < m_vAngle.x)
	{
		m_vAngle.x = 180 / 4.0f;
	}
	// 回転行列作成
	XMMATRIX mR = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_vAngle.x),
		XMConvertToRadians(m_vAngle.y),
		XMConvertToRadians(m_vAngle.z));


	// 作成した回転行列をベクトルにかけ合わせる
	vecPlayerToTarget = XMVector3Transform(vecPlayerToTarget, mR);
	// 作成したベクトルをプレイヤー座標に加算したものが新しい注視点座標
	m_vTarget.x = vPlayerPos.x + XMVectorGetX(vecPlayerToTarget);
	m_vTarget.y = vPlayerPos.y + XMVectorGetY(vecPlayerToTarget);
	m_vTarget.z = vPlayerPos.z + XMVectorGetZ(vecPlayerToTarget);


	// 上方向は常に(0,1,0)ベクトル
	m_vUp = XMFLOAT3(0, 1, 0);

	// **********カメラ座標**********
	// カメラ座標はプレイヤーの肩越しの位置の座標
	XMVECTOR vecPlayerToCamera;
	XMVECTOR vecCameraX = XMVector3Normalize(XMVectorSet(m_mtxWorld._11, m_mtxWorld._12, m_mtxWorld._13, 1.0f));
	XMVECTOR vecCameraY = XMVector3Normalize(XMVectorSet(m_mtxWorld._21, m_mtxWorld._22, m_mtxWorld._23, 1.0f));
	XMVECTOR vecCameraZ = XMVector3Normalize(XMVectorSet(m_mtxWorld._31, m_mtxWorld._32, m_mtxWorld._33, 1.0f));
	vecPlayerToCamera = vecCameraX * CAM_POS.x + vecCameraY * CAM_POS.y + vecCameraZ * CAM_POS.z;
	m_vPos.x = vPlayerPos.x + XMVectorGetX(vecPlayerToCamera);
	m_vPos.y = vPlayerPos.y + XMVectorGetY(vecPlayerToCamera);
	m_vPos.z = vPlayerPos.z + XMVectorGetZ(vecPlayerToCamera);

	CCamera::Update();
}