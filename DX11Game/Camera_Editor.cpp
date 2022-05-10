#include"Camera_Editor.h"
#include"Input.h"
#include"Scene.h"
#include"CameraManager.h"
#include"GameObj.h"
#include"Editor_Manager.h"
#include"Text.h"
namespace {
	const XMFLOAT3	CAM_POS = XMFLOAT3(0, 300.0f, -500.0f);		        // プレイヤーを基準とするTPSカメラの相対視点位置
	const float CAM_MOVE_SPEED = 10.0f;

	const XMFLOAT2 RETICLE_SCALE = XMFLOAT2(20.0f, 40.0f);
	const XMFLOAT4 RETICLE_COLOR = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
}

// 静的メンバ
bool CCamera_Editor::m_bMove = false;

CCamera_Editor::CCamera_Editor() : CCamera()
{
	// 移動スピード
	m_fSpeed = CAM_MOVE_SPEED;
	// 視点から注視点までの長さ
	XMStoreFloat(&m_fPosToTargetLength, XMVector3Length(
		XMVectorSet(m_vStartTarget.x - m_vStartPos.x,
			m_vStartTarget.y - m_vStartPos.y,
			m_vStartTarget.z - m_vStartPos.z, 1.0f)));
	m_fAspectRatio = VIEW_ASPECT;		   	  // 縦横比
	m_fFovY = VIEW_ANGLE;					  // 視野角(Degree)
	Init();                                   // 初期化
}

void CCamera_Editor::Init()
{
	//m_nEditorReticleIndex = -1;
	// マトリックス更新
	UpdateMatrix();
	// 視錐台更新
	UpdateViewFrustum();
	CCamera::Init();
}

void CCamera_Editor::Fin()
{
	//CText::DeleteText(m_nEditorReticleIndex);
	CCamera::Fin();
}

void CCamera_Editor::Update()
{	
	// エディターモード選択中は更新しない 
	if (CEditor_Manager::GetStateSelectFlag())return;
	// 右クリックホールド中以外は更新しない
	if (!CInput::GetKeyPress(VK_RBUTTON))
	{
		if (CInput::GetKeyRelease(VK_RBUTTON)) {
			// カメラ移動フラグ
			m_bMove = false;
			// マウスカーソル非固定
			CInput::SetRockCursorFlag(false);
			// マウスカーソルの表示
			ShowCursor(TRUE);
		}
		return;
	}
	if (CInput::GetKeyTrigger(VK_RBUTTON))
	{
		// カメラ移動フラグ
		m_bMove = true;
		// マウスカーソル固定
		CInput::SetRockCursorFlag(true);
		// マウスカーソルの非表示
		ShowCursor(FALSE);
	}
	 
	XMMATRIX mtxWorld = XMMatrixIdentity();
	// ********回転***********
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
	// カメラの回転量から回転行列を作成
	// 回転行列作成
	XMMATRIX mR = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_vAngle.x),
		XMConvertToRadians(m_vAngle.y),
		XMConvertToRadians(m_vAngle.z));
	// ワールド行列に回転を反映
	mtxWorld = XMMatrixMultiply(mtxWorld, mR);
	XMStoreFloat4x4(&m_mtxWorld, mtxWorld);
	// **********移動**********
	if (CInput::GetKeyPress(VK_LSHIFT))
	{
		if (CInput::GetKeyPress(VK_W))
		{
			// 上進
			m_vPos.x += m_mtxWorld._21 * m_fSpeed;
			m_vPos.y += m_mtxWorld._22 * m_fSpeed;
			m_vPos.z += m_mtxWorld._23 * m_fSpeed;
		}
		if (CInput::GetKeyPress(VK_S))
		{
			// 下進
			m_vPos.x -= m_mtxWorld._21 * m_fSpeed;
			m_vPos.y -= m_mtxWorld._22 * m_fSpeed;
			m_vPos.z -= m_mtxWorld._23 * m_fSpeed;
		}

	}
	else {
		if (CInput::GetKeyPress(VK_W))
		{
			// 前進
			m_vPos.x += m_mtxWorld._31 * m_fSpeed;
			m_vPos.y += m_mtxWorld._32 * m_fSpeed;
			m_vPos.z += m_mtxWorld._33 * m_fSpeed;
		}
		if (CInput::GetKeyPress(VK_S))
		{
			// 後進
			m_vPos.x -= m_mtxWorld._31 * m_fSpeed;
			m_vPos.y -= m_mtxWorld._32 * m_fSpeed;
			m_vPos.z -= m_mtxWorld._33 * m_fSpeed;
		}
	}
	if (CInput::GetKeyPress(VK_D))
	{
		// 左進
		m_vPos.x += m_mtxWorld._11 * m_fSpeed;
		m_vPos.y += m_mtxWorld._12 * m_fSpeed;
		m_vPos.z += m_mtxWorld._13 * m_fSpeed;
	}
	if (CInput::GetKeyPress(VK_A))
	{
		// 右進
		m_vPos.x -= m_mtxWorld._11 * m_fSpeed;
		m_vPos.y -= m_mtxWorld._12 * m_fSpeed;
		m_vPos.z -= m_mtxWorld._13 * m_fSpeed;
	}
	
	// 視点・注視点決定
	m_mtxWorld._41 = m_vPos.x;
	m_mtxWorld._42 = m_vPos.y;
	m_mtxWorld._43 = m_vPos.z;
	m_vTarget.x = m_mtxWorld._41 + m_mtxWorld._31 * m_fPosToTargetLength;
	m_vTarget.y = m_mtxWorld._42 + m_mtxWorld._32 * m_fPosToTargetLength;
	m_vTarget.z = m_mtxWorld._43 + m_mtxWorld._33 * m_fPosToTargetLength;
	
	// マトリックス更新
	UpdateMatrix();
	// 視錐台更新
	UpdateViewFrustum();

	// レティクル場所更新
	//CText::DeleteText(m_nEditorReticleIndex);
	//m_nEditorReticleIndex = CText::OutPutText(std::string("+"), m_vTarget, RETICLE_SCALE);
}