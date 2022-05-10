#include "TitleUI.h"
#include "Text.h"
#include "Scene.h"
#include "EffekseerManager.h"

// �ÓI�����o
std::vector<int>  CTitleUI::m_DrawTextIndex;
int CTitleUI::m_nEfkHandle;

void CTitleUI::Init()
{
	// �e�L�X�g�\��
	SetText();
	// �I�𒆌�������
	m_nEfkHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_PLAYER_EFK), true, TIMER_MACHINGUN_GENERATER_PLAYER_EFK);
	// �����ʒu�ύX
	MoveToTextPosEfk();
}

void CTitleUI::Fin()
{
	// �����폜
	CEffekseerManager::Stop(m_nEfkHandle, true);
	// ���݂̃e�L�X�g�폜
	DeleteText();
}

void CTitleUI::MoveSelect()
{
	// �I�����ύX
	if (CInput::GetKeyTrigger(VK_W))
	{
		// �J�[�\���ړ��T�E���h
		CSound::Play(SE_MENUE);
		// �J�[�\���ӏ��ύX
		m_nCurSelect = (int)(m_nCurSelect - 1);
		if (m_nCurSelect < 0)
		{
			m_nCurSelect = 0;
		}
		// �����ʒu�ύX
		MoveToTextPosEfk();
	}
	else if (CInput::GetKeyTrigger(VK_S))
	{
		// �J�[�\���ړ��T�E���h
		CSound::Play(SE_MENUE);
		// �J�[�\���ӏ��ύX
		m_nCurSelect = (int)(m_nCurSelect + 1);
		if (m_nMaxSelect <= m_nCurSelect)
		{
			m_nCurSelect = m_nMaxSelect - 1;
		}
		// �����ʒu�ύX
		MoveToTextPosEfk();
	}
}

void CTitleUI::DeleteText()
{
	for (auto itr = m_DrawTextIndex.begin(); itr != m_DrawTextIndex.end();)
	{
		CText::DeleteText(*itr);
		++itr;
	}
	m_DrawTextIndex.clear();
}

void CTitleUI::SetEfkPos(XMFLOAT2 vPos)
{
	// �����ʒu
	XMFLOAT2 EfkScreenPos(vPos.x, vPos.y/*850.0f, 625.0f*/);
	XMFLOAT3 EfkPos;
	CalcScreenToWorld(
		&EfkPos,
		EfkScreenPos.x, EfkScreenPos.y, 0.997f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		&CScene::GetCurrent()->GetCameraManager()->GetCamera()->GetViewMatrix(),
		&CScene::GetCurrent()->GetCameraManager()->GetCamera()->GetProjMatrix());
	XMFLOAT4X4 EfkMat;
	XMStoreFloat4x4(&EfkMat, XMMatrixIdentity());

	XMMATRIX mtxScl;
	mtxScl = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	XMStoreFloat4x4(&EfkMat, XMMatrixMultiply(XMLoadFloat4x4(&EfkMat), mtxScl));

	EfkMat._41 = EfkPos.x;
	EfkMat._42 = EfkPos.y;
	EfkMat._43 = EfkPos.z;

	CEffekseerManager::SetMatrix(m_nEfkHandle, true, EfkMat);
}


// �X�N���[�����W�����[���h���W�ɕϊ�
XMFLOAT3* CTitleUI::CalcScreenToWorld(
	XMFLOAT3* pout,
	float Sx,  // �X�N���[��X���W
	float Sy,  // �X�N���[��Y���W
	float fZ,  // �ˉe��Ԃł�Z�l�i0�`1�j
	float Screen_w,
	float Screen_h,
	XMFLOAT4X4* View,
	XMFLOAT4X4* Prj
) {
	// �e�s��̋t�s����Z�o
	XMMATRIX InvView, InvPrj, InvViewport;
	InvView = XMMatrixInverse(nullptr, XMLoadFloat4x4(View));
	InvPrj = XMMatrixInverse(nullptr, XMLoadFloat4x4(Prj));
	XMFLOAT4X4 VP;
	XMStoreFloat4x4(&VP, XMMatrixIdentity());
	VP._11 = Screen_w / 2.0f; VP._22 = -Screen_h / 2.0f;
	VP._41 = Screen_w / 2.0f; VP._42 = Screen_h / 2.0f;
	InvViewport = XMMatrixInverse(nullptr, XMLoadFloat4x4(&VP));

	// �t�ϊ�
	XMMATRIX tmp = InvViewport * InvPrj * InvView;
	XMStoreFloat3(pout, XMVector3TransformCoord(XMVectorSet(Sx, Sy, fZ, 1.0f), tmp));

	return pout;
}
