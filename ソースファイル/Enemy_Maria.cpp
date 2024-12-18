// マリア（ボス）[Enemy_Maria.cpp]
#include "Enemy_Maria.h"
#include "Sound.h"
#include "EffekseerManager.h"
#include "Explosion.h"
#include "MGManager.h"
#include "SwordAttackManager.h"
#include "DebugProc.h"
// 定数
namespace {
	// ********ステータス********
	const int STATUS_MAX_LIFE = 10000;  // 最大ライフ
	const int STATUS_ATK = 1000;		// 物理攻撃力
	const int STATUS_DEF = 400;			// 物理防御力
	const int STATUS_INT = 200;			// 魔法攻撃力
	const int STATUS_RES = 100;			// 魔法防御力

	// *********移動用変数*********
	const float MOVE_ACCEL = 0.2f;      // 加速度
	const float VEL_MAX = 2.5f;         // 最高速度
	const float ROLLSPEED = 2.0f;       // 振り向き速度　単位:度
	const float SEACH_RAD = 5000.0f;    // 索敵範囲

	// *********当たり判定*********
	const float COL_SPHERE_RAD = 50.0f;
	const XMFLOAT3 COL_BBOX = XMFLOAT3(50.0f, 120.0f, 60.0f);

	// *********攻撃用定数*********
	const float ATTACK_BS_RAD = 20.0f;          // 攻撃範囲
	const int ALLMOTION_HIGHSPINATTACK = 60;    // 総攻撃モーション時間
	const int ALLMOTION_GRATESWORDSLASH = 190;  // 総攻撃モーション時間
	const int START_TIME_HIGHSPINATTACK = 10;   // 攻撃判定出現時間(処理開始後からの経過時間)
	const int START_TIME_GRATESWORDSLASH = 90;  // 攻撃判定出現時間(処理開始後からの経過時間)
	const float TECRATE_HIGHSPINATTACK = 0.7f;  // 技ごとのダメージ倍率
	const float TECRATE_GRATESWORDSLASH = 0.9f;	// 技ごとのダメージ倍率


	// *********アニメーション用定数*********
	const double ANIM_SPEED = 0.015;       // アニメーション再生速度
	// アニメーションの種類
	const int ANIM_IDLE            = 0;    // 待機
	const int ANIM_SERCH           = 1;    // 索敵
	const int ANIM_RUN             = 2;    // 走る
	const int ANIM_HIGHSPINATTACK  = 3;    // ハイスラッシュ
	const int ANIM_GRATESWORDSLASH = 4;    // グレートスラッシュ
	const int ANIM_DYING           = 5;    // 死
	const int ANIM_HITBACK         = 6;    // ヒットバック

	// *********その他定数***********
	// 死んだときの時間をゆっくりにする時間？（カウント）
	const int TIME_SLOW_CNT = 30;
}

// コンストラクタ
CEnemy_Maria::CEnemy_Maria(CScene* pScene) : CEnemy(pScene)
{
	m_pWeaponManager = nullptr;
	SetTag(GOT_BOSS);
	SetObjID(EObjID::OI_ENEMY_MARIA);
	m_Transform.SetMoveAccel(MOVE_ACCEL);
	m_Transform.SetVelMax(VEL_MAX);
	m_Transform.SetRollSpeed(ROLLSPEED);
	m_fAtkRad = ATTACK_BS_RAD;
	m_fSeachRad = SEACH_RAD;
	m_empEBaseAnimationToInt[IDLE_BA] = ANIM_IDLE;
	m_empEBaseAnimationToInt[WALK_BA] = ANIM_RUN;
	m_empEBaseAnimationToInt[DYING_BA] = ANIM_DYING;
	m_empEBaseAnimationToInt[HITBACK_BA] = ANIM_HITBACK;
}

// デストラクタ
CEnemy_Maria::~CEnemy_Maria()
{
}

// 初期化
HRESULT CEnemy_Maria::Init()
{
	HRESULT hr = CEnemy::Init();

	// ステータス
	m_Status.nMaxHP = STATUS_MAX_LIFE;
	m_Status.nHP = m_Status.nMaxHP;
	m_Status.nATK = STATUS_ATK;
	m_Status.nDEF = STATUS_DEF;
	m_Status.nINT = STATUS_INT;
	m_Status.nRES = STATUS_RES;
	// モデル設定
	SetModel(MT_MARIA);
	m_fRadius = COL_SPHERE_RAD;
	m_vBBox = COL_BBOX;
	m_vCenter.x = 0;
	m_vCenter.z = 0;

	// 武器処理
	if (m_pWeaponManager) {
		m_pWeaponManager->AllDelete();
		delete m_pWeaponManager;
	}
	// 装備武器
	m_pWeaponManager = new CSwordAttackManager(m_pScene);
	m_pWeaponManager->SetParent(this);

	return hr;
}

// 終了処理
void CEnemy_Maria::Fin()
{
	SAFE_DELETE(m_pWeaponManager);
	CEnemy::Fin();
}

// 更新
void CEnemy_Maria::Update()
{
	CEnemy::Update();
}


void CEnemy_Maria::Draw()
{
	// アニメーション設定
	SetAnimIndex(m_nAnimStack);
	SetAnimTime(m_dAnimTime);

	CEnemy::Draw();
}

void CEnemy_Maria::NearAttack()
{
	// 攻撃範囲内にプレイヤーがいるか
	if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), ATTACK_BS_RAD, m_pTarget->GetRadius())) {
		// プレイヤーへのベクトルを取得
		XMFLOAT3& vPlayerPos = m_pTarget->GetTransform()->GetPos();
		XMFLOAT3 vPlayer;
		vPlayer.x = vPlayerPos.x - m_Transform.GetPos().x;
		vPlayer.y = vPlayerPos.y - m_Transform.GetPos().y;
		vPlayer.z = vPlayerPos.z - m_Transform.GetPos().z;
		// 前方向ベクトル
		XMFLOAT4X4 mW = m_Transform.GetWorld();
		XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);		
		// 自分の攻撃射角の範囲内にいると攻撃
		float fAttackAngle;
		XMStoreFloat(&fAttackAngle, XMVector3AngleBetweenNormals(XMVector3Normalize(XMLoadFloat3(&vZ)), XMVector3Normalize(XMLoadFloat3(&vPlayer))));
		if (fAttackAngle < XMConvertToRadians(45.0f))
		{
			// 攻撃開始
			m_pWeaponManager->Fire((int)(m_Status.nATK * TECRATE_GRATESWORDSLASH), ALLMOTION_GRATESWORDSLASH, START_TIME_GRATESWORDSLASH);
			// ステート変更
			SetState(CharaState::HandRangeAttack);
			// アニメーション変更
			m_nNextAnimStack = ANIM_GRATESWORDSLASH;
		}
		else
		{
			// 攻撃開始
			m_pWeaponManager->Fire((int)(m_Status.nATK * TECRATE_HIGHSPINATTACK), ALLMOTION_HIGHSPINATTACK, START_TIME_HIGHSPINATTACK);
			// ステート変更
			SetState(CharaState::HandRangeAttack);
			// アニメーション変更
			m_nNextAnimStack = ANIM_HIGHSPINATTACK;
		}
	}
}

void CEnemy_Maria::AnimeUpdate()
{


	// アニメーション番号更新
	switch (m_nAnimStack)
	{
	case ANIM_IDLE:
	case ANIM_SERCH:
	case ANIM_RUN:
	case ANIM_DYING:
	case ANIM_HIGHSPINATTACK:
	case ANIM_GRATESWORDSLASH:
	case ANIM_HITBACK:
		if (m_nAnimStack != m_nNextAnimStack) {
			m_nAnimStack = m_nNextAnimStack;
			m_dAnimTime = 0.0;
		}
		break;
	default:
		break;
	}

	// アニメーション時間更新
	switch (m_nAnimStack)
	{
	case ANIM_HIGHSPINATTACK:
	case ANIM_GRATESWORDSLASH:
	case ANIM_DYING:
		m_dAnimTime += ANIM_SPEED * 1.5;
		break;
	case ANIM_HITBACK:
		m_dAnimTime += ANIM_SPEED * 2.0;
		break;
	default:
		m_dAnimTime += ANIM_SPEED;
		break;
	}

	// アニメーション開始してからの時間が総アニメーション時間を超えていた時の処理
	if (m_dAnimTime >= GetAnimDuration(m_nAnimStack))
	{
		switch (m_nAnimStack) {
		case ANIM_IDLE:
		case ANIM_SERCH:
		case ANIM_RUN:
			// アニメーション番号を繰り返す
			m_dAnimTime = 0.0;
			break;
		case ANIM_HIGHSPINATTACK:
		case ANIM_GRATESWORDSLASH:
		case ANIM_HITBACK:
			// アニメーション番号を待機アニメーション番号に
			m_dAnimTime = 0.0;
			// 再攻撃可能に
			m_pWeaponManager->Create(GOT_BOSS);
			// ステート変更
			SetState(CharaState::Wait);
			break;
		case ANIM_DYING:
			// この後の爆発ですぐ不可視になる
			if (m_bVisible) {
				// 不可視
				m_bVisible = false;
				// エフェクシア
				StartDeath();
			}
		}
	}
}

