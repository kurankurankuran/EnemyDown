#include "AttackMethod.h"
#include "SwordAttackManager.h"
#include "MGManager.h"
#include "Scene.h"

namespace{
	// 技ごとのダメージ倍率
	const float TECRATE_SLASH = 1.5f;
	const float TECRATE_MGMAGIC = 0.7f;
	// 総攻撃モーション時間
	const int ALLMOTION_ATTACK = 28;
	// 攻撃判定出現時間(処理開始後からの経過時間)
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
	// 近距離武器
	if (m_pHandRangeWeaponManager) {
		// メモリ解放
		m_pHandRangeWeaponManager->AllDelete();
		m_pHandRangeWeaponManager.reset();
	}
	// 生成
	m_pHandRangeWeaponManager = std::make_shared<CSwordAttackManager>(CScene::GetCurrent());
	m_pHandRangeWeaponManager->SetParent(m_pParent);
	// 初期化
	m_pHandRangeWeaponManager->Init(&m_pParent->GetTransform()->GetWorld());
	m_pHandRangeWeaponManager->Create(GOT_PLAYER);


	// 遠距離武器
	if (m_pLongRangeWeaponManager) {
		// メモリ解放
		m_pLongRangeWeaponManager->AllDelete();
		m_pLongRangeWeaponManager.reset();
	}
	// 生成
	m_pLongRangeWeaponManager = std::make_shared<CMGManager>(CScene::GetCurrent());
	m_pLongRangeWeaponManager->SetParent(m_pParent);
	// 初期化
	m_pLongRangeWeaponManager->Init(&m_pParent->GetTransform()->GetWorld());
	m_pLongRangeWeaponManager->Create(GOT_PLAYER);
}

void CAttackMethod::Fin()
{
	// 終了処理
	m_pHandRangeWeaponManager->Fin();
	m_pLongRangeWeaponManager->Fin();
}

void CAttackMethod::Update()
{
	// 更新処理
	m_pHandRangeWeaponManager->Update();
	m_pLongRangeWeaponManager->Update();
}

void CAttackMethod::AttackMethod()
{
	// 武器変更
	//ChangeWeapon();
	// 武器装填
	if (CInput::GetKeyTrigger(VK_R)) {
		m_pLongRangeWeaponManager->Create(GOT_PLAYER);
		CSound::Play(SE_DEFEND);
		m_pParent->SetState(Reload);
	}
	// 武器発射
	if (CInput::GetMouseButton(VK_RBUTTON)) {
		if (m_pCamera) {
			m_pParent->GetTransform()->RotationModel(XMFLOAT3(m_pCamera->GetWorldMatrix()._31, 0.0f, m_pCamera->GetWorldMatrix()._33));
		}

		// 遠距離攻撃
		if (CInput::GetMouseButton(VK_LBUTTON)) {
			switch (m_pParent->GetState())
			{
			case CharaState::Wait:
				// 攻撃発射
				m_pLongRangeWeaponManager->Fire((int)(m_pParent->GetStatus().nINT * TECRATE_MGMAGIC));
				// ステート設定
				m_pParent->SetState(CharaState::LongRangeAttack);
				break;
			case CharaState::Walk:
				// 攻撃発射
				m_pLongRangeWeaponManager->Fire((int)(m_pParent->GetStatus().nINT * TECRATE_MGMAGIC));
				// ステート設定
				m_pParent->SetState(CharaState::LongRangeAttack);
				break;
			case CharaState::Dash:
				// 何もしない
				break;
			case CharaState::LongRangeAttack:
				// 攻撃発射
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
			// 近距離攻撃
			switch (m_pParent->GetState())
			{
			case CharaState::Wait:
				// 攻撃
				m_pHandRangeWeaponManager->Fire((int)(m_pParent->GetStatus().nATK * TECRATE_SLASH), ALLMOTION_ATTACK, START_TIME_ATTACK);
				// ステート設定
				m_pParent->SetState(CharaState::HandRangeAttack);
				break;
			case CharaState::Walk:
				// 攻撃
				m_pHandRangeWeaponManager->Fire((int)(m_pParent->GetStatus().nATK * TECRATE_SLASH), ALLMOTION_ATTACK, START_TIME_ATTACK);
				// ステート設定
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
