#include "AttackMethod.h"
#include "SwordAttackManager.h"
#include "MGManager.h"
#include "Scene.h"

namespace{
	// �Z���Ƃ̃_���[�W�{��
	const float TECRATE_SLASH = 1.5f;
	const float TECRATE_MGMAGIC = 0.7f;
	// ���U�����[�V��������
	const int ALLMOTION_ATTACK = 28;
	// �U������o������(�����J�n�ォ��̌o�ߎ���)
	const int START_TIME_ATTACK = 18;
}

CAttackMethod::CAttackMethod(CGameObj* pParent)
{
	m_pParent = pParent;
	Init();
}

CAttackMethod::~CAttackMethod()
{

}

void CAttackMethod::Init()
{
	m_pCamera = CScene::GetCurrent()->GetCameraManager()->GetCamera();
	// �ߋ�������
	if (m_pHandRangeWeaponManager) {
		// ���������
		m_pHandRangeWeaponManager->AllDelete();
		m_pHandRangeWeaponManager.reset();
	}
	// ����
	m_pHandRangeWeaponManager = std::make_shared<CSwordAttackManager>(CScene::GetCurrent());
	m_pHandRangeWeaponManager->SetParent(m_pParent);
	// ������
	m_pHandRangeWeaponManager->Init(&m_pParent->GetTransform()->GetWorld());
	m_pHandRangeWeaponManager->Create(GOT_PLAYER);


	// ����������
	if (m_pLongRangeWeaponManager) {
		// ���������
		m_pLongRangeWeaponManager->AllDelete();
		m_pLongRangeWeaponManager.reset();
	}
	// ����
	m_pLongRangeWeaponManager = std::make_shared<CMGManager>(CScene::GetCurrent());
	m_pLongRangeWeaponManager->SetParent(m_pParent);
	// ������
	m_pLongRangeWeaponManager->Init(&m_pParent->GetTransform()->GetWorld());
	m_pLongRangeWeaponManager->Create(GOT_PLAYER);
}

void CAttackMethod::Fin()
{
	// �I������
	m_pHandRangeWeaponManager->Fin();
	m_pLongRangeWeaponManager->Fin();
}

void CAttackMethod::Update()
{
	// �X�V����
	m_pHandRangeWeaponManager->Update();
	m_pLongRangeWeaponManager->Update();
}

void CAttackMethod::AttackMethod()
{
	// ����ύX
	//ChangeWeapon();
	// ���푕�U
	if (CInput::GetKeyTrigger(VK_R)) {
		m_pLongRangeWeaponManager->Create(GOT_PLAYER);
		CSound::Play(SE_DEFEND);
		m_pParent->SetState(Reload);
	}
	// ���픭��
	if (CInput::GetMouseButton(VK_RBUTTON)) {
		if (m_pCamera) {
			m_pParent->GetTransform()->RotationModel(XMFLOAT3(m_pCamera->GetWorldMatrix()._31, 0.0f, m_pCamera->GetWorldMatrix()._33));
		}

		// �������U��
		if (CInput::GetMouseButton(VK_LBUTTON)) {
			switch (m_pParent->GetState())
			{
			case CharaState::Wait:
				// �U������
				m_pLongRangeWeaponManager->Fire((int)(m_pParent->GetStatus().nINT * TECRATE_MGMAGIC));
				// �X�e�[�g�ݒ�
				m_pParent->SetState(CharaState::LongRangeAttack);
				break;
			case CharaState::Walk:
				// �U������
				m_pLongRangeWeaponManager->Fire((int)(m_pParent->GetStatus().nINT * TECRATE_MGMAGIC));
				// �X�e�[�g�ݒ�
				m_pParent->SetState(CharaState::LongRangeAttack);
				break;
			case CharaState::Dash:
				// �������Ȃ�
				break;
			case CharaState::LongRangeAttack:
				// �U������
				m_pLongRangeWeaponManager->Fire((int)(m_pParent->GetStatus().nINT * TECRATE_MGMAGIC));
				break;
			default:
				break;
			}
		}
	}
	else if (CInput::GetMouseButton(VK_LBUTTON))
	{
		if (m_pParent->GetOnGround())
		{
			// �ߋ����U��
			switch (m_pParent->GetState())
			{
			case CharaState::Wait:
				// �U��
				m_pHandRangeWeaponManager->Fire((int)(m_pParent->GetStatus().nATK * TECRATE_SLASH), ALLMOTION_ATTACK, START_TIME_ATTACK);
				// �X�e�[�g�ݒ�
				m_pParent->SetState(CharaState::HandRangeAttack);
				break;
			case CharaState::Walk:
				// �U��
				m_pHandRangeWeaponManager->Fire((int)(m_pParent->GetStatus().nATK * TECRATE_SLASH), ALLMOTION_ATTACK, START_TIME_ATTACK);
				// �X�e�[�g�ݒ�
				m_pParent->SetState(CharaState::HandRangeAttack);
				break;
			default:
				break;
			}
		}
	}
}

CWeaponManager* CAttackMethod::GetHandWeapon()
{
	return m_pHandRangeWeaponManager.get();
}

CWeaponManager* CAttackMethod::GetLongWeapon()
{
	return m_pLongRangeWeaponManager.get();
}

//std::weak_ptr<CWeaponManager> CAttackMethod::GetHandWeapon()
//{
//	std::weak_ptr<CWeaponManager> wpHandRangeWeaponManager(m_pHandRangeWeaponManager);
//	return wpHandRangeWeaponManager;
//}
//
//std::weak_ptr<CWeaponManager> CAttackMethod::GetLongWeapon()
//{
//	std::weak_ptr<CWeaponManager> wpLongRangeWeaponManager(m_pLongRangeWeaponManager);
//	return wpLongRangeWeaponManager;
//}
