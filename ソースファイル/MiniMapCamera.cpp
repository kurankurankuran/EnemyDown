//=============================================================================
//
// �J���� �N���X [Camera.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "MiniMapCamera.h"
#include "Player.h"
#include "Scene.h"
#include "Polygon.h"
#include "Text.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

namespace {

	const XMFLOAT3 CAM_POS_MINIMAP = XMFLOAT3(0, 3000, 0);      // �J�����̏������W
	const XMFLOAT3 CAM_TARGETPOS_MINIMAP = XMFLOAT3(0, 0, 0);   // �J�����̒����_���W

}

// �R���X�g���N�^
CMiniMapCamera::CMiniMapCamera()
{
	Init();
}

// ������
void CMiniMapCamera::Init()
{
	// �v���C���[�|�C���^
	m_pTargetObj = nullptr;
	//// �J�������[�h����
	//m_nMode = STATIC_CAMERAMODE;

	// �����ʒu����
	m_vPos = CAM_POS_MINIMAP;		// ���_
	m_vTarget = CAM_TARGETPOS_MINIMAP;	// �����_
	m_vUp = XMFLOAT3(0.0f, 0.0f, 1.0f);		// ����x�N�g��


	// ������]�ʐݒ�
	m_vAngle = XMFLOAT3(0, 0, 0);

	m_fAspectRatio = 1/*VIEW_ASPECT*/;			// �c����
	m_fFovY = VIEW_ANGLE;					// ����p(Degree)
	m_fNearZ = VIEW_NEAR_Z;					// �O���N���b�v����
	m_fFarZ = VIEW_FAR_Z;					// ����N���b�v����

	// �v���W�F�N�V�����ϊ��p�����[�^���王���䐶��
	CreateViewFrustum();

	// ���_/�����_/����x�N�g�����烏�[���h �}�g���b�N�X�Z�o
	CalcWorldMatrix();

	// �J�����̃��[���h�}�g���b�N�X���王������ړ�
	UpdateViewFrustum();

	// ���p�e�L�X�g
	CText::OutPutText(std::string("N"), XMFLOAT2(500, 345), XMFLOAT2(15, 30), XMFLOAT4(1, 0, 0, 0.5f));
	CText::OutPutText(std::string("S"), XMFLOAT2(500, 205), XMFLOAT2(15, 30), XMFLOAT4(0, 0, 1, 0.5f));
	CText::OutPutText(std::string("W"), XMFLOAT2(425, 275), XMFLOAT2(15, 30), XMFLOAT4(0, 1, 0, 0.5f));
	CText::OutPutText(std::string("E"), XMFLOAT2(575, 275), XMFLOAT2(15, 30), XMFLOAT4(0, 1, 1, 0.5f));

}

// �X�V
void CMiniMapCamera::Update()
{
	
	if (m_pTargetObj) {
		m_vTarget = m_pTargetObj->GetTransform()->GetPos();
		m_vPos = XMFLOAT3(m_vTarget.x, m_vTarget.y + 6000.0f, m_vTarget.z);
		/*XMFLOAT4X4 mW = m_pTargetObj->GetTransform()->GetWorld();
		XMFLOAT3 vZ;
		XMStoreFloat3(&vZ, XMVector3Normalize(XMVectorSet(mW._31, mW._32, mW._33, 1.0f)));
		m_vUp = XMFLOAT3(vZ.x, vZ.y, vZ.z);*/


		// �}�g���b�N�X�X�V
		UpdateMatrix();

		// ���[���h�}�g���b�N�X�X�V�A������X�V
		CalcWorldMatrix();
		UpdateViewFrustum();
	}
}

void CMiniMapCamera::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	SetBlendState(BS_ALPHABLEND);
	SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�
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
	SetZWrite(true);							// Z�o�b�t�@�X�V����
	SetBlendState(BS_NONE);		                // �A���t�@�u�����h����


}

// ��ʃN���A
void CMiniMapCamera::Clear()
{
	// main.cpp�̃��\�[�X�Q��
	extern ID3D11RenderTargetView* g_pMiniMapRTV;
	extern ID3D11DepthStencilView* g_pMiniMapDSV;

	float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	ID3D11DeviceContext* pDC = GetDeviceContext();
	pDC->ClearRenderTargetView(g_pMiniMapRTV, ClearColor);
	pDC->ClearDepthStencilView(g_pMiniMapDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}



// �r���[/�v���W�F�N�V���� �}�g���b�N�X�X�V
void CMiniMapCamera::UpdateMatrix()
{
	XMStoreFloat4x4(&m_mtxView, XMMatrixLookAtLH(
		XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp)));
	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovY), m_fAspectRatio, m_fNearZ, m_fFarZ));
}

// ���[���h �}�g���b�N�X�ݒ�
void CMiniMapCamera::SetWorldMatrix(XMFLOAT4X4& mtxWorld)
{
	m_mtxWorld = mtxWorld;
	m_vPos = XMFLOAT3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
	m_vTarget = XMFLOAT3(mtxWorld._41 + mtxWorld._31, mtxWorld._42 + mtxWorld._32, mtxWorld._43 + mtxWorld._33);
	m_vUp = XMFLOAT3(mtxWorld._21, mtxWorld._22, mtxWorld._23);
}

// ���_/�����_/����x�N�g�����烏�[���h �}�g���b�N�X�Z�o
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

// �^�[�Q�b�g�I�u�W�F�N�g���Z�b�g
void CMiniMapCamera::SetTargetObj(CGameObj* pObj)
{
	m_pTargetObj = pObj;
}

// ������(0:��\��, -1:����, 1:�\��)
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
			return 0;	// ���S�ɊO��
		if (fDot <= fRadius)
			bHit = true;
	}
	if (bHit)
		return -1;	// �����I�ɓ���
	return 1;	// ���S�ɓ���
}

// �v���W�F�N�V�����ϊ��p�����[�^���王���䐶��
void CMiniMapCamera::CreateViewFrustum()
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
void CMiniMapCamera::UpdateViewFrustum()
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
void CMiniMapCamera::SetFovY(float fFovY)
{
	m_fFovY = fFovY;
	CreateViewFrustum();
}

// �A�X�y�N�g��ݒ�
void CMiniMapCamera::SetAspectRatio(float fAspect)
{
	m_fAspectRatio = fAspect;
	CreateViewFrustum();
}

// �O��N���b�v�����ݒ�
void CMiniMapCamera::SetRangeZ(float fNearZ, float fFarZ)
{
	m_fNearZ = fNearZ, m_fFarZ = fFarZ;
	CreateViewFrustum();
}

