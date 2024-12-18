#include"Camera_TPS.h"
#include"Input.h"
#include"Scene.h"
#include"CameraManager.h"
#include"Text.h"
#include"GameObj.h"
namespace {
	const XMFLOAT3	CAM_POS = XMFLOAT3(0, 300.0f, -500.0f);		        // �v���C���[����Ƃ���TPS�J�����̑��Ύ��_�ʒu

	// ���e�B�N���`��p
	const std::string RETICLE = std::string("+");
	const XMFLOAT2 RETICLE_POS = XMFLOAT2(0.0f, 0.0f);
	const XMFLOAT2 RETICLE_SCALE = XMFLOAT2(20.0f, 40.0f);
	const XMFLOAT4 RETICLE_COLOR = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);
}

CCamera_TPS::CCamera_TPS() : CCamera()
{
	m_fAspectRatio = VIEW_ASPECT;		   	  // �c����
	m_fFovY = VIEW_ANGLE;					  // ����p(Degree)
	Init();                                   // ������
}

void CCamera_TPS::Init()
{
	CCamera::Init();
}

void CCamera_TPS::Update()
{
	// �|�[�Y��ԂȂ�X�V���Ȃ�
	if (CScene::GetPause()->GetPauseFlg())return;
	if (!m_pTargetObj)return;

	// �E�N���b�N�ɂ��J�������[�h�ύX
	if (CInput::GetMouseButton(VK_RBUTTON))
	{
		CScene::GetCurrent()->GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::FPS_CAMERAMODE);
		// ���e�B�N���\��
		m_nReeticleTextIndex = CText::OutPutText(RETICLE, RETICLE_POS, RETICLE_SCALE, RETICLE_COLOR);
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
	// ������͏��(0,1,0)�x�N�g��
	m_vUp = XMFLOAT3(0, 1, 0);

	// �����_�̓v���C���[�̎����̐�
	XMStoreFloat3(&m_vTarget, XMVector3TransformCoord(XMVectorSet(m_vStartTarget.x, m_vStartTarget.y, m_vStartTarget.z, 1.0f), mW));

	// �J�������W�A�萔�Ƀv���C���[�̉�]�������[���h�s����������킹��
	XMStoreFloat3(&m_vPos, XMVector3TransformCoord(XMVectorSet(CAM_POS.x, CAM_POS.y, CAM_POS.z, 1.0f), mW));


	/*
	�}�E�X�Ńv���C���[�����ɃJ�������W��]
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

	XMFLOAT3 vPlayerPos = m_pTargetObj->GetTransform()->GetPos();
	// **********�J�������W**********
	// �v���C���[���W����J�������W�ւ̃x�N�g��
	XMVECTOR vecPlayerToPos = XMVectorAdd(XMLoadFloat3(&m_vPos), -XMLoadFloat3(&vPlayerPos));
	// �J�����̉�]�ʂ����]�s����쐬
	// �}�E�X����
	// ��]�s��쐬
	XMMATRIX mR = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_vAngle.x),
		XMConvertToRadians(m_vAngle.y),
		XMConvertToRadians(m_vAngle.z));
	// �쐬������]�s����x�N�g���ɂ������킹��
	vecPlayerToPos = XMVector3Transform(vecPlayerToPos, mR);
	// �쐬�����x�N�g�����v���C���[���W�ɉ��Z�������̂��V�����J�������W
	m_vPos.x = vPlayerPos.x + XMVectorGetX(vecPlayerToPos);
	m_vPos.y = vPlayerPos.y + XMVectorGetY(vecPlayerToPos);
	m_vPos.z = vPlayerPos.z + XMVectorGetZ(vecPlayerToPos);


	// **********�J���������_**********
	// �v���C���[���W���璍���_���W�ւ̃x�N�g��
	XMVECTOR vecPlayerToTarget = XMVectorAdd(XMLoadFloat3(&m_vTarget), -XMLoadFloat3(&vPlayerPos));
	// �쐬������]�s����x�N�g���ɂ������킹��
	vecPlayerToTarget = XMVector3Transform(vecPlayerToTarget, mR);
	// �쐬�����x�N�g�����v���C���[���W�ɉ��Z�������̂��V���������_���W
	m_vTarget.x = vPlayerPos.x + XMVectorGetX(vecPlayerToTarget);
	m_vTarget.y = vPlayerPos.y + XMVectorGetY(vecPlayerToTarget);
	m_vTarget.z = vPlayerPos.z + XMVectorGetZ(vecPlayerToTarget);


	CCamera::Update();
}