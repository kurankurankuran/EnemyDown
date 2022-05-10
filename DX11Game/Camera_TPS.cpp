#include"Camera_TPS.h"
#include"Input.h"
#include"Scene.h"
#include"CameraManager.h"
#include"Text.h"
#include"GameObj.h"
namespace {
	const XMFLOAT3	CAM_POS = XMFLOAT3(0, 300.0f, -500.0f);		        // プレイヤーを基準とするTPSカメラの相対視点位置

	// レティクル描画用
	const std::string RETICLE = std::string("+");
	const XMFLOAT2 RETICLE_POS = XMFLOAT2(0.0f, 0.0f);
	const XMFLOAT2 RETICLE_SCALE = XMFLOAT2(20.0f, 40.0f);
	const XMFLOAT4 RETICLE_COLOR = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);
}

CCamera_TPS::CCamera_TPS() : CCamera()
{
	m_fAspectRatio = VIEW_ASPECT;		   	  // 縦横比
	m_fFovY = VIEW_ANGLE;					  // 視野角(Degree)
	Init();                                   // 初期化
}

void CCamera_TPS::Init()
{
	CCamera::Init();
}

void CCamera_TPS::Update()
{
	// ポーズ状態なら更新しない
	if (CScene::GetPause()->GetPauseFlg())return;
	if (!m_pTargetObj)return;

	// 右クリックによるカメラモード変更
	if (CInput::GetMouseButton(VK_RBUTTON))
	{
		CScene::GetCurrent()->GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::FPS_CAMERAMODE);
		// レティクル表示
		m_nReeticleTextIndex = CText::OutPutText(RETICLE, RETICLE_POS, RETICLE_SCALE, RETICLE_COLOR);
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
	// 上方向は常に(0,1,0)ベクトル
	m_vUp = XMFLOAT3(0, 1, 0);

	// 注視点はプレイヤーの視線の先
	XMStoreFloat3(&m_vTarget, XMVector3TransformCoord(XMVectorSet(m_vStartTarget.x, m_vStartTarget.y, m_vStartTarget.z, 1.0f), mW));

	// カメラ座標、定数にプレイヤーの回転無しワールド行列をかけ合わせる
	XMStoreFloat3(&m_vPos, XMVector3TransformCoord(XMVectorSet(CAM_POS.x, CAM_POS.y, CAM_POS.z, 1.0f), mW));


	/*
	マウスでプレイヤーを軸にカメラ座標回転
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

	XMFLOAT3 vPlayerPos = m_pTargetObj->GetTransform()->GetPos();
	// **********カメラ座標**********
	// プレイヤー座標からカメラ座標へのベクトル
	XMVECTOR vecPlayerToPos = XMVectorAdd(XMLoadFloat3(&m_vPos), -XMLoadFloat3(&vPlayerPos));
	// カメラの回転量から回転行列を作成
	// マウス操作
	// 回転行列作成
	XMMATRIX mR = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_vAngle.x),
		XMConvertToRadians(m_vAngle.y),
		XMConvertToRadians(m_vAngle.z));
	// 作成した回転行列をベクトルにかけ合わせる
	vecPlayerToPos = XMVector3Transform(vecPlayerToPos, mR);
	// 作成したベクトルをプレイヤー座標に加算したものが新しいカメラ座標
	m_vPos.x = vPlayerPos.x + XMVectorGetX(vecPlayerToPos);
	m_vPos.y = vPlayerPos.y + XMVectorGetY(vecPlayerToPos);
	m_vPos.z = vPlayerPos.z + XMVectorGetZ(vecPlayerToPos);


	// **********カメラ注視点**********
	// プレイヤー座標から注視点座標へのベクトル
	XMVECTOR vecPlayerToTarget = XMVectorAdd(XMLoadFloat3(&m_vTarget), -XMLoadFloat3(&vPlayerPos));
	// 作成した回転行列をベクトルにかけ合わせる
	vecPlayerToTarget = XMVector3Transform(vecPlayerToTarget, mR);
	// 作成したベクトルをプレイヤー座標に加算したものが新しい注視点座標
	m_vTarget.x = vPlayerPos.x + XMVectorGetX(vecPlayerToTarget);
	m_vTarget.y = vPlayerPos.y + XMVectorGetY(vecPlayerToTarget);
	m_vTarget.z = vPlayerPos.z + XMVectorGetZ(vecPlayerToTarget);


	CCamera::Update();
}