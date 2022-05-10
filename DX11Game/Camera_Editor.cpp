#include"Camera_Editor.h"
#include"Input.h"
#include"Scene.h"
#include"CameraManager.h"
#include"GameObj.h"
#include"Editor_Manager.h"
#include"Text.h"
namespace {
	const XMFLOAT3	CAM_POS = XMFLOAT3(0, 300.0f, -500.0f);		        // �v���C���[����Ƃ���TPS�J�����̑��Ύ��_�ʒu
	const float CAM_MOVE_SPEED = 10.0f;

	const XMFLOAT2 RETICLE_SCALE = XMFLOAT2(20.0f, 40.0f);
	const XMFLOAT4 RETICLE_COLOR = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
}

// �ÓI�����o
bool CCamera_Editor::m_bMove = false;

CCamera_Editor::CCamera_Editor() : CCamera()
{
	// �ړ��X�s�[�h
	m_fSpeed = CAM_MOVE_SPEED;
	// ���_���璍���_�܂ł̒���
	XMStoreFloat(&m_fPosToTargetLength, XMVector3Length(
		XMVectorSet(m_vStartTarget.x - m_vStartPos.x,
			m_vStartTarget.y - m_vStartPos.y,
			m_vStartTarget.z - m_vStartPos.z, 1.0f)));
	m_fAspectRatio = VIEW_ASPECT;		   	  // �c����
	m_fFovY = VIEW_ANGLE;					  // ����p(Degree)
	Init();                                   // ������
}

void CCamera_Editor::Init()
{
	//m_nEditorReticleIndex = -1;
	// �}�g���b�N�X�X�V
	UpdateMatrix();
	// ������X�V
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
	// �G�f�B�^�[���[�h�I�𒆂͍X�V���Ȃ� 
	if (CEditor_Manager::GetStateSelectFlag())return;
	// �E�N���b�N�z�[���h���ȊO�͍X�V���Ȃ�
	if (!CInput::GetKeyPress(VK_RBUTTON))
	{
		if (CInput::GetKeyRelease(VK_RBUTTON)) {
			// �J�����ړ��t���O
			m_bMove = false;
			// �}�E�X�J�[�\����Œ�
			CInput::SetRockCursorFlag(false);
			// �}�E�X�J�[�\���̕\��
			ShowCursor(TRUE);
		}
		return;
	}
	if (CInput::GetKeyTrigger(VK_RBUTTON))
	{
		// �J�����ړ��t���O
		m_bMove = true;
		// �}�E�X�J�[�\���Œ�
		CInput::SetRockCursorFlag(true);
		// �}�E�X�J�[�\���̔�\��
		ShowCursor(FALSE);
	}
	 
	XMMATRIX mtxWorld = XMMatrixIdentity();
	// ********��]***********
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
	// �J�����̉�]�ʂ����]�s����쐬
	// ��]�s��쐬
	XMMATRIX mR = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_vAngle.x),
		XMConvertToRadians(m_vAngle.y),
		XMConvertToRadians(m_vAngle.z));
	// ���[���h�s��ɉ�]�𔽉f
	mtxWorld = XMMatrixMultiply(mtxWorld, mR);
	XMStoreFloat4x4(&m_mtxWorld, mtxWorld);
	// **********�ړ�**********
	if (CInput::GetKeyPress(VK_LSHIFT))
	{
		if (CInput::GetKeyPress(VK_W))
		{
			// ��i
			m_vPos.x += m_mtxWorld._21 * m_fSpeed;
			m_vPos.y += m_mtxWorld._22 * m_fSpeed;
			m_vPos.z += m_mtxWorld._23 * m_fSpeed;
		}
		if (CInput::GetKeyPress(VK_S))
		{
			// ���i
			m_vPos.x -= m_mtxWorld._21 * m_fSpeed;
			m_vPos.y -= m_mtxWorld._22 * m_fSpeed;
			m_vPos.z -= m_mtxWorld._23 * m_fSpeed;
		}

	}
	else {
		if (CInput::GetKeyPress(VK_W))
		{
			// �O�i
			m_vPos.x += m_mtxWorld._31 * m_fSpeed;
			m_vPos.y += m_mtxWorld._32 * m_fSpeed;
			m_vPos.z += m_mtxWorld._33 * m_fSpeed;
		}
		if (CInput::GetKeyPress(VK_S))
		{
			// ��i
			m_vPos.x -= m_mtxWorld._31 * m_fSpeed;
			m_vPos.y -= m_mtxWorld._32 * m_fSpeed;
			m_vPos.z -= m_mtxWorld._33 * m_fSpeed;
		}
	}
	if (CInput::GetKeyPress(VK_D))
	{
		// ���i
		m_vPos.x += m_mtxWorld._11 * m_fSpeed;
		m_vPos.y += m_mtxWorld._12 * m_fSpeed;
		m_vPos.z += m_mtxWorld._13 * m_fSpeed;
	}
	if (CInput::GetKeyPress(VK_A))
	{
		// �E�i
		m_vPos.x -= m_mtxWorld._11 * m_fSpeed;
		m_vPos.y -= m_mtxWorld._12 * m_fSpeed;
		m_vPos.z -= m_mtxWorld._13 * m_fSpeed;
	}
	
	// ���_�E�����_����
	m_mtxWorld._41 = m_vPos.x;
	m_mtxWorld._42 = m_vPos.y;
	m_mtxWorld._43 = m_vPos.z;
	m_vTarget.x = m_mtxWorld._41 + m_mtxWorld._31 * m_fPosToTargetLength;
	m_vTarget.y = m_mtxWorld._42 + m_mtxWorld._32 * m_fPosToTargetLength;
	m_vTarget.z = m_mtxWorld._43 + m_mtxWorld._33 * m_fPosToTargetLength;
	
	// �}�g���b�N�X�X�V
	UpdateMatrix();
	// ������X�V
	UpdateViewFrustum();

	// ���e�B�N���ꏊ�X�V
	//CText::DeleteText(m_nEditorReticleIndex);
	//m_nEditorReticleIndex = CText::OutPutText(std::string("+"), m_vTarget, RETICLE_SCALE);
}