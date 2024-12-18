//=============================================================================
//
// �J���� �N���X [Camera.cpp]
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
// �}�N����`
//*****************************************************************************
// const�ɂ��ăX�e�[�W���ƂɌ��߂�ׂ��H
#define	CAM_POS_P_X			(20000.0f)					// �J�����̎��_�����ʒu(X���W)
#define	CAM_POS_P_Y			(20000.0f)				    // �J�����̎��_�����ʒu(Y���W)
#define	CAM_POS_P_Z			(20000.0f)				// �J�����̎��_�����ʒu(Z���W)

#define	CAM_TARGET_TITLE_X	(  0.0f)			    // �^�C�g���J�����̎��_�����ʒu(X���W)
#define	CAM_TARGET_TITLE_Y	(100.0f)		        // �^�C�g���J�����̎��_�����ʒu(Y���W)
#define	CAM_TARGET_TITLE_Z	(  0.0f)			    // �^�C�g���J�����̎��_�����ʒu(Z���W)

#define CAM_ROTANGLE_ROL        (20.0f)                 // ���[���s�b�`���̃J�����̃x�N�g����]��
#define CAM_ROTANGLE_PIT        (10.0f)                 // ���[���s�b�`���̃J�����̃x�N�g����]��
#define CAM_ROTCONFICIENT_ROL   (1.0f / CAM_ROTANGLE_ROL)   // ��]�W��
#define CAM_ROTCONFICIENT_PIT   (1.0f / CAM_ROTANGLE_PIT)   // ��]�W��

#define	CAM_POS_Y_X			(0.0f)					        // �J�����̎��_�����ʒu(X���W)
#define	CAM_POS_Y_Y			(35000.0f)				        // �J�����̎��_�����ʒu(Y���W)
#define	CAM_POS_Y_Z			(0.0f)				            // �J�����̎��_�����ʒu(Z���W)

namespace {
	const XMFLOAT3	CAM_POS = XMFLOAT3(0, 300.0f, -500.0f);		    // ���_�����ʒu
	const XMFLOAT3	CAM_TARGET_POS = XMFLOAT3(0, 0, 400.0f);		// �����_�����ʒu
	const XMFLOAT3	CAM_ROT = XMFLOAT3(0, 0, 0);					// �J�����̏�����]��(X��)
}

// �ÓI�ϐ�
DirectX::XMFLOAT3 CCamera::m_vStartPos = CAM_POS;		    // �������_
DirectX::XMFLOAT3 CCamera::m_vPos = CAM_POS;		        // ���_
DirectX::XMFLOAT3 CCamera::m_vAngle = CAM_ROT;		        // ��]��
DirectX::XMFLOAT3 CCamera::m_vStartTarget = CAM_TARGET_POS;	// ���������_
DirectX::XMFLOAT3 CCamera::m_vTarget = CAM_TARGET_POS;	    // �����_
DirectX::XMFLOAT3 CCamera::m_vUp;		                    // ����x�N�g��
DirectX::XMFLOAT4X4 CCamera::m_mtxWorld;	                // ���[���h �}�g���b�N�X
DirectX::XMFLOAT4X4 CCamera::m_mtxView;	                    // �r���[ �}�g���b�N�X
DirectX::XMFLOAT4X4 CCamera::m_mtxProj;	                    // �v���W�F�N�V���� �}�g���b�N�X
float CCamera::m_fNearZ = VIEW_NEAR_Z;					    // �O���N���b�v����
float CCamera::m_fFarZ = VIEW_FAR_Z;					    // ����N���b�v����
CAssimpModel* CCamera::m_pSky = nullptr;
CGameObj* CCamera::m_pTargetObj = nullptr;
int CCamera::m_nReeticleTextIndex = -1;

// �R���X�g���N�^
CCamera::CCamera()
{
	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);		  // ����x�N�g��
}

// ������
void CCamera::Init()
{
	m_fRadius = 300.0f;

	// �o���b�g�^�C�����J�����̍ی��݂̉�]�ʂ�ۑ�
	m_fAngle = 0.0f;
	// �o���b�g�^�C�����J�����̍ۃJ�������W���Z�o����ׂɎg�p
	m_fFreeBulletMode = XMFLOAT2(2000.0f, 200.0f);
	
	// �v���W�F�N�V�����ϊ��p�����[�^���王���䐶��
	CreateViewFrustum();

	// ���_/�����_/����x�N�g�����烏�[���h �}�g���b�N�X�Z�o
	CalcWorldMatrix();

	// �J�����̃��[���h�}�g���b�N�X���王������ړ�
	UpdateViewFrustum();

}

// �X�V
void CCamera::Update()
{
	//// �v���C���[���Z�b�g����Ă�����v���C���[��J����
	//switch (m_nMode) {
	//	//****************************************
	//	// ��_�J����
	//	//****************************************
	//case STATIC_CAMERAMODE:
	//	if (m_pTargetObj) {
	//		m_vTarget = m_pTargetObj->GetTransform()->GetPos();
	//	}
	//	m_vPos = XMFLOAT3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);
	//	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

	//	break;
	//	//****************************************
	//	// ��_�J����(�^�C�g�����)
	//	//****************************************
	//case STATIC_TITLE_CAMERAMODE:
	//	m_vTarget = XMFLOAT3(CAM_TARGET_TITLE_X, CAM_TARGET_TITLE_Y, CAM_TARGET_TITLE_Z);
	//	m_vPos = XMFLOAT3(CAM_POS_TITLE_X, CAM_POS_TITLE_Y, CAM_POS_TITLE_Z);
	//	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//	break;
	//	//****************************************
	//	// ��l�̃J����
	//	//****************************************
	//case FPS_CAMERAMODE:
	//{
	//	
	//}

	//break;
	////****************************************
	//// �o���b�g�^�C�����J����
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
	//	// �O�l�̃J����
	//	//****************************************
	//case TPS_CAMERAMODE:
	//{
	//	

	//	// *******************************************************************************************************
	//	break;
	//}
	////****************************************
	//// �G�f�B�^�[�J����
	////****************************************
	//case EDITOR_CAMERAMODE:
	//{


	//	break; 
	//}
	////****************************************
	//// �o���b�g�^�C�����J����(���R�ӏ�)
	////****************************************
	//case BULLET_FREE_CAMERAMODE:
	//	m_fAngle += 0.5f;

	//	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//	m_vPos.x = m_vTarget.x + sinf(XMConvertToRadians(m_fAngle)) * (m_fFreeBulletMode.x);
	//	m_vPos.y = m_vTarget.y + m_fFreeBulletMode.y;
	//	m_vPos.z = m_vTarget.z + cosf(XMConvertToRadians(m_fAngle)) * (m_fFreeBulletMode.x);
	//	break;
	//}
		
	
	// �}�g���b�N�X�X�V
	UpdateMatrix();

	// ���[���h�}�g���b�N�X�X�V�A������X�V
	CalcWorldMatrix();
	UpdateViewFrustum();

	// �f�o�b�O�\��
	CDebugProc::Print("*** ��� ��� ***\n");
	CDebugProc::Print("FP View:[1]\nBullet-time like:[2]\nTP View:[3]\n:TPBack View[4]\nFixed point:[0]\n");
	CDebugProc::Print("[��� �� :\n(%f, %f, %f)]\n", m_vPos.x, m_vPos.y, m_vPos.z);
	CDebugProc::Print("[������ :\n(%f, %f, %f)]\n", m_vTarget.x, m_vTarget.y, m_vTarget.z);
	CDebugProc::Print("\n");
}

// ��ʃN���A
void CCamera::Clear()
{
	// main.cpp�̃��\�[�X�Q��
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

// �r���[/�v���W�F�N�V���� �}�g���b�N�X�X�V
void CCamera::UpdateMatrix()
{
	XMStoreFloat4x4(&m_mtxView, XMMatrixLookAtLH(
		XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp)));
	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovY), m_fAspectRatio, m_fNearZ, m_fFarZ));
}

// ���[���h �}�g���b�N�X�ݒ�
void CCamera::SetWorldMatrix(XMFLOAT4X4& mtxWorld)
{
	m_mtxWorld = mtxWorld;
	m_vPos = XMFLOAT3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
	m_vTarget = XMFLOAT3(mtxWorld._41 + mtxWorld._31, mtxWorld._42 + mtxWorld._32, mtxWorld._43 + mtxWorld._33);
	m_vUp = XMFLOAT3(mtxWorld._21, mtxWorld._22, mtxWorld._23);
}

// ���_/�����_/����x�N�g�����烏�[���h �}�g���b�N�X�Z�o
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

//// �v���C���[���Z�b�g
//void CCamera::SetPlayer(CPlayer* pPlayer)
//{
//	m_pPlayer = pPlayer;
//	//m_fRadius = (pPlayer) ? pPlayer->GetRadius() : 0.5f;
//}

// �^�[�Q�b�g�I�u�W�F�N�g���Z�b�g
void CCamera::SetTargetObj(CGameObj* pObj)
{
	m_pTargetObj = pObj;
}

// ������(0:��\��, -1:����, 1:�\��)
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
			return 0;	// ���S�ɊO��
		if (fDot <= fRadius)
			bHit = true;
	}
	if (bHit)
		return -1;	// �����I�ɓ���
	return 1;	// ���S�ɓ���
}

// �v���W�F�N�V�����ϊ��p�����[�^���王���䐶��
void CCamera::CreateViewFrustum()
{
	// 0:�� 1:�� 2:�� 3:�E 4:�O 5:���
	float fTan =
		tanf(XMConvertToRadians(m_fFovY) * 0.5f);
	m_frus[0] = XMFLOAT4(0.0f, -1.0f, fTan, 0.0f);
	m_frus[1] = XMFLOAT4(0.0f, 1.0f, fTan, 0.0f);
	fTan *= m_fAspectRatio;
	m_frus[2] = XMFLOAT4(1.0f, 0.0f, fTan, 0.0f);
	m_frus[3] = XMFLOAT4(-1.0f, 0.0f, fTan, 0.0f);
	m_frus[4] = XMFLOAT4(0.0f, 0.0f, 1.0f, -m_fNearZ);
	m_frus[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, m_fFarZ);
	// 0�`3�𐳋K��
	for (int i = 0; i < 4; ++i) {
		XMStoreFloat4(&m_frus[i],
			XMPlaneNormalize(
				XMLoadFloat4(&m_frus[i])));
	}
}

// �J�����̃��[���h�}�g���b�N�X���王������ړ�
void CCamera::UpdateViewFrustum()
{
	// CalcWorldMatrix��m_mtxWorld���X�V�ς�O��
	XMMATRIX mW = XMLoadFloat4x4(&m_mtxWorld);
	mW = XMMatrixInverse(nullptr, mW);//�t�s��
	mW = XMMatrixTranspose(mW);//�]�u�s��
	// ���t�s��̓]�u�s������߂�̂�
	//   XMPlaneTransform�̎d�l!!
	for (int i = 0; i < 6; ++i) {
		XMStoreFloat4(&m_frusw[i],
			XMPlaneTransform(
				XMLoadFloat4(&m_frus[i]), mW));
	}
}

// ����p�ݒ�
void CCamera::SetFovY(float fFovY)
{
	m_fFovY = fFovY;
	CreateViewFrustum();
}

// �A�X�y�N�g��ݒ�
void CCamera::SetAspectRatio(float fAspect)
{
	m_fAspectRatio = fAspect;
	CreateViewFrustum();
}

// �O��N���b�v�����ݒ�
void CCamera::SetRangeZ(float fNearZ, float fFarZ)
{
	m_fNearZ = fNearZ, m_fFarZ = fFarZ;
	CreateViewFrustum();
}
