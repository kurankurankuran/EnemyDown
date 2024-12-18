//敵（ﾐｭｰﾀﾝﾄ） [Enemy_Mutant.cpp]
#include "Enemy_Mutant.h"
#include "Sound.h"
#include "EffekseerManager.h"
#include "Explosion.h"
#include "MGManager.h"
#include "SwordAttackManager.h"
#include "DebugProc.h"
// 定数
namespace {
	
	// ********ステータス********
	const int STATUS_MAX_LIFE = 2500;   // 最大ライフ
	const int STATUS_ATK = 500;         // 物理攻撃力
	const int STATUS_DEF = 500;         // 物理防御力
	const int STATUS_INT =  50;         // 魔法攻撃力
	const int STATUS_RES =  25;         // 魔法防御力

	// *********移動用変数*********
	const float MOVE_ACCEL = 0.2f;      // 加速度
	const float VEL_MAX = 2.5f;         // 最高速度
	const float ROLLSPEED = 0.5f;       // 振り向き速度　単位:度
	const float SEACH_RAD = 5000.0f;    // 索敵範囲

	// *********当たり判定*********
	const float COL_SPHERE_RAD = 50.0f;
	const XMFLOAT3 COL_BBOX = XMFLOAT3(50.0f, 120.0f, 60.0f);

	// *********攻撃用定数*********
	const float ATTACK_BS_RAD = 20.0f;  // 攻撃範囲
	const int ALLMOTION_ATTACK = 30;    // 総攻撃モーション時間
	const int START_TIME_ATTACK = 18;   // 攻撃判定出現時間(処理開始後からの経過時間)
	const float TECRATE_NAILATK = 1.3f; // 技ごとのダメージ倍率

	// *********アニメーション用定数*********
	const double ANIM_SPEED = 0.015;    // アニメーション再生速度
	// アニメーションの種類
	const int ANIM_IDLE       = 0;      // 待機
	const int ANIM_WALK       = 1;      // 歩行
	const int ANIM_DYING      = 2;      // 死
	const int ANIM_WAIT_HIMA  = 3;      // 待機２
	const int ANIM_ATTACK     = 4;      // 攻撃
	const int ANIM_HITBACK    = 5;      // 被ダメージ
}

// コンストラクタ
CEnemy_Mutant::CEnemy_Mutant(CScene* pScene) : CEnemy(pScene)
{
	m_pWeaponManager = nullptr;
	SetObjID(EObjID::OI_ENEMY_MUTANT);
	m_Transform.SetMoveAccel(MOVE_ACCEL);
	m_Transform.SetVelMax(VEL_MAX);
	m_Transform.SetRollSpeed(ROLLSPEED);
	m_fAtkRad = ATTACK_BS_RAD;
	m_fSeachRad = SEACH_RAD;
	m_empEBaseAnimationToInt[IDLE_BA] = ANIM_IDLE;
	m_empEBaseAnimationToInt[WALK_BA] = ANIM_WALK;
	m_empEBaseAnimationToInt[DYING_BA] = ANIM_DYING;
	m_empEBaseAnimationToInt[HITBACK_BA] = ANIM_HITBACK;

	
}

// デストラクタ
CEnemy_Mutant::~CEnemy_Mutant()
{
}

// 初期化
HRESULT CEnemy_Mutant::Init()
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
	SetModel(MT_MUTANT);
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
void CEnemy_Mutant::Fin()
{
	SAFE_DELETE(m_pWeaponManager);
	CEnemy::Fin();
}

// 更新
void CEnemy_Mutant::Update()
{
	CEnemy::Update();	
}


void CEnemy_Mutant::Draw()
{
	// アニメーション設定
	SetAnimIndex(m_nAnimStack);
	SetAnimTime(m_dAnimTime);

	CEnemy::Draw();
}

void CEnemy_Mutant::NearAttack()
{
	// 攻撃範囲内にプレイヤーがいるか
	if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), m_fAtkRad, m_pTarget->GetRadius())) {

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
			m_pWeaponManager->Fire((int)(m_Status.nATK * TECRATE_NAILATK), ALLMOTION_ATTACK, START_TIME_ATTACK);
			// ステート変更
			SetState(CharaState::HandRangeAttack);
			// アニメーション変更
			m_nNextAnimStack = ANIM_ATTACK;
		}
	}
}


void CEnemy_Mutant::AnimeUpdate()
{
	

	// アニメーション番号更新
	switch (m_nAnimStack)
	{
	case ANIM_WAIT_HIMA:
	case ANIM_IDLE:
	case ANIM_WALK:
	case ANIM_DYING:
	case ANIM_ATTACK:
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
	case ANIM_DYING:
		m_dAnimTime += ANIM_SPEED * 1.5;
		break;
	default:
		m_dAnimTime += ANIM_SPEED;
		break;
	}
	
	// アニメーション終了時の処理
	if (m_dAnimTime >= GetAnimDuration(m_nAnimStack))
	{
		switch (m_nAnimStack) {
		case ANIM_WALK:
		case ANIM_WAIT_HIMA:
		case ANIM_IDLE:
			// アニメーション番号を繰り返す
			m_dAnimTime = 0.0;
			break;
		case ANIM_ATTACK:
		case ANIM_HITBACK:
			// アニメーション番号を待機アニメーション番号に
			m_dAnimTime = 0.0;
			// 再攻撃可能に
			m_pWeaponManager->Create(GOT_ENEMY);
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

