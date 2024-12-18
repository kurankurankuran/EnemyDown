#include"Camera_FPS.h"
#include"Input.h"
#include"Scene.h"
#include"CameraManager.h"
#include"Text.h"
#include"GameObj.h"
namespace {
	const XMFLOAT3 CAM_POS = XMFLOAT3(75.0f, 150.0f, -150.0f);           // �v���C���[����Ƃ���FPS�J�����̑��Ύ��_�ʒu

}

CCamera_FPS::CCamera_FPS() : CCamera()
{
	m_fAspectRatio = VIEW_ASPECT;		   	  // �c����
	m_fFovY = VIEW_ANGLE;					  // ����p(Degree)
	Init();                                   // ������
}

void CCamera_FPS::Init()
{
	CCamera::Init();
}

void CCamera_FPS::Update()
{
	// �|�[�Y��ԂȂ�X�V���Ȃ�
	if (CScene::GetPause()->GetPauseFlg())return;
	if (!m_pTargetObj)return;

	if (CInput::GetMouseRelease(VK_RBUTTON))
	{
		CScene::GetCurrent()->GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::TPS_CAMERAMODE);
		m_nReeticleTextIndex = CText::DeleteText(m_nReeticleTextIndex);
	}

	XMFLOAT4X4 mtxW = m_pTargetObj->GetTransform()->GetWorld();
	// �v���C���[�̃��[���h�s��̉�]�����𖳌��ɂ����s����������킹��
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

	// **********�J���������_**********
	XMFLOAT3 vPlayerPos = m_pTargetObj->GetTransform()->GetPos();
	// �����_�̓v���C���[�̎����̐�
	XMStoreFloat3(&m_vTarget, XMVector3TransformCoord(XMVectorSet(m_vStartTarget.x, m_vStartTarget.y, m_vStartTarget.z, 1.0f), mW));
	// �v���C���[���W���璍���_���W�ւ̃x�N�g��
	XMVECTOR vecPlayerToTarget = XMVectorAdd(XMLoadFloat3(&m_vTarget), -XMLoadFloat3(&vPlayerPos));
	// �}�E�X����
	// �}�E�X�Ńv���C���[�����ɃJ�������W��]
	/*
	�P�s�N�Z���ӂ�̉�]�ʂ��v�Z
	�������͉�ʂ̔����̈ړ��ʂ�180����]�A
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
	// ��]�s��쐬
	XMMATRIX mR = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_vAngle.x),
		XMConvertToRadians(m_vAngle.y),
		XMConvertToRadians(m_vAngle.z));


	// �쐬������]�s����x�N�g���ɂ������킹��
	vecPlayerToTarget = XMVector3Transform(vecPlayerToTarget, mR);
	// �쐬�����x�N�g�����v���C���[���W�ɉ��Z�������̂��V���������_���W
	m_vTarget.x = vPlayerPos.x + XMVectorGetX(vecPlayerToTarget);
	m_vTarget.y = vPlayerPos.y + XMVectorGetY(vecPlayerToTarget);
	m_vTarget.z = vPlayerPos.z + XMVectorGetZ(vecPlayerToTarget);


	// ������͏��(0,1,0)�x�N�g��
	m_vUp = XMFLOAT3(0, 1, 0);

	// **********�J�������W**********
	// �J�������W�̓v���C���[�̌��z���̈ʒu�̍��W
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