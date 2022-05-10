// 敵魔法使い [Enemy_Wizard.cpp]
#include "Enemy_Wizard.h"
#include "Sound.h"
#include "EffekseerManager.h"
#include "MGManager.h"
#include "SwordAttackManager.h"
#include "DebugProc.h"
// 定数
namespace {
	// ********ステータス********
	const int STATUS_MAX_LIFE = 1000;   // 最大ライフ
	const int STATUS_ATK = 100;			// 物理攻撃力
	const int STATUS_DEF = 50;			// 物理防御力
	const int STATUS_INT = 300;			// 魔法攻撃力
	const int STATUS_RES = 300;			// 魔法防御力

	// *********移動用変数*********
	const float MOVE_ACCEL = 0.5f;      // 加速度
	const float VEL_MAX = 1.0f;         // 最高速度
	const float ROLLSPEED = 12.0f;      // 振り向き速度　単位:度
	const float SEACH_RAD = 5000.0f;    // 索敵範囲
	
	// *********当たり判定*********
	const float COL_SPHERE_RAD = 50.0f;
	const XMFLOAT3 COL_BBOX = XMFLOAT3(50.0f, 120.0f, 60.0f);

	// *********攻撃用定数*********
	const float ATTACK_BS_RAD = 3000.0f;    // 攻撃範囲
	const int ALLMOTION_ATTACK = 30;        // 総攻撃モーション時間
	const int START_TIME_ATTACK = 18;       // 攻撃判定出現時間(処理開始後からの経過時間)
	const float TECRATE_NAILATK = 1.3f;     // 技ごとのダメージ倍率
	const float TECRATE_TRIPLEMAGIC = 1.2f; // 技ごとのダメージ倍率

	// *********アニメーション用定数*********
	const double ANIM_SPEED = 0.015;    // アニメーション再生速度
	const int ANIM_IDLE     = 0;        // 待機
	const int ANIM_WALK     = 1;        // 歩行
	const int ANIM_ATTACK   = 2;        // 攻撃
	const int ANIM_DYING    = 3;        // 死
	const int ANIM_SERCH    = 4;        // 索敵
	const int ANIM_HITBACK  = 5;        // ヒットバック
}

// コンストラクタ
CEnemy_Wizard::CEnemy_Wizard(CScene* pScene) : CEnemy(pScene)
{
	m_pWeaponManager = nullptr;
	SetObjID(EObjID::OI_ENEMY_WIZARD);
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
CEnemy_Wizard::~CEnemy_Wizard()
{
}

// 初期化
HRESULT CEnemy_Wizard::Init()
{
	HRESULT hr = CEnemy::Init();
	// 攻撃カウント
	m_nAttackCnt = 0;
	// ステータス
	m_Status.nMaxHP = STATUS_MAX_LIFE;
	m_Status.nHP = m_Status.nMaxHP;
	m_Status.nATK = STATUS_ATK;
	m_Status.nDEF = STATUS_DEF;
	m_Status.nINT = STATUS_INT;
	m_Status.nRES = STATUS_RES;
	// モデル設定
	SetModel(MT_WIZARD);
	m_fRadius = 50.0f;
	m_vBBox = XMFLOAT3(50.0f, m_vCenter.y, 60.0f);
	m_vCenter.x = 0;
	m_vCenter.z = 0;
	// 武器処理
	if (m_pWeaponManager) {
		m_pWeaponManager->AllDelete();
		delete m_pWeaponManager;
	}
	// 武器設定
	m_pWeaponManager = new CMGManager(m_pScene);
	m_pWeaponManager->SetParent(this);

	return hr;
}

// 終了処理
void CEnemy_Wizard::Fin()
{
	SAFE_DELETE(m_pWeaponManager);
	CEnemy::Fin();
}

// 更新
void CEnemy_Wizard::Update()
{
	CEnemy::Update();
	//// のけぞり処理
	//if (CharaState::HitBack == m_State)
	//{
	//	m_nNextAnimStack = ANIM_HITBACK;

	//	// 死亡判定
	//	if (m_Status.nHP <= 0)
	//	{
	//		// ライフ0
	//		m_Status.nHP = 0;
	//		// アニメスタック設定
	//		m_nNextAnimStack = ANIM_DYING;
	//		// ステート変更
	//		SetState(CharaState::Dying);
	//		// 武器終了処理
	//		m_pWeaponManager->Fin();
	//		// 死亡音
	//		CSound::Play(SE_ENEMY_DIE);
	//	}
	//}
	//

	//do {
	//	if (!m_bVisible)
	//		break;

	//	if (Dying == m_State)
	//		break;

	//	if (CharaState::HitBack == m_State)
	//		break;

	//	// ワールド変換行列取得
	//	XMFLOAT4X4 mW = m_Transform.GetWorld();
	//	// モデル座標軸取得
	//	XMFLOAT3 vX = XMFLOAT3(mW._11, mW._12, mW._13);
	//	XMFLOAT3 vY = XMFLOAT3(mW._21, mW._22, mW._23);
	//	XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
	//	// 座標取得、クリア
	//	m_Transform.SetPos(XMFLOAT3(mW._41, mW._42, mW._43));
	//	mW._41 = mW._42 = mW._43 = 0.0f;

	//	m_Transform.SetVel(XMFLOAT3(0, 0, 0));

	//	if (CharaState::HandRangeAttack != m_State)
	//	{
	//		if (m_pTarget) {
	//			// 追尾範囲内にプレイヤーがいるか
	//			if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), SERCH_BS_RAD, m_pTarget->GetRadius())) {
	//				// プレイヤーは可視状態か
	//				if (m_pTarget->GetVisible()) {
	//					// プレイヤーへのベクトルを取得
	//					XMFLOAT3& vPlayerPos = m_pTarget->GetTransform()->GetPos();
	//					XMFLOAT3 vPlayer;
	//					vPlayer.x = vPlayerPos.x - m_Transform.GetPos().x;
	//					vPlayer.y = vPlayerPos.y - m_Transform.GetPos().y;
	//					vPlayer.z = vPlayerPos.z - m_Transform.GetPos().z;
	//					XMVECTOR vecPlayer;
	//					vecPlayer = XMVector3Normalize(XMLoadFloat3(&vPlayer));

	//					// ターゲットが左右どちらか
	//					XMMATRIX mR = XMMatrixIdentity();

	//					float fRight = vX.x * vPlayer.x + vX.y * vPlayer.y + vX.z * vPlayer.z;	// 内積
	//					XMStoreFloat(&fRight, XMVector3Dot(XMLoadFloat3(&vX), vecPlayer));

	//					if (fRight > 0.0f)		// 右
	//						mR *= XMMatrixRotationAxis(XMLoadFloat3(&vY), XMConvertToRadians(ROLLSPEED));
	//					else if (fRight < 0.0f)	// 左
	//						mR *= XMMatrixRotationAxis(XMLoadFloat3(&vY), XMConvertToRadians(-ROLLSPEED));

	//					XMStoreFloat4x4(&mW, XMMatrixMultiply(XMLoadFloat4x4(&mW), mR));


	//					//********移動処理******
	//						// 向いている方向に移動		
	//					m_Transform.SetVel(
	//						XMFLOAT3(
	//							m_Transform.GetVel().x + mW._31 * MOVE_SPEED,
	//							m_Transform.GetVel().y + mW._32 * MOVE_SPEED,
	//							m_Transform.GetVel().z + mW._33 * MOVE_SPEED
	//						));

	//					// アニメーション変更
	//					m_nNextAnimStack = ANIM_WALK;
	//					// ステート変更
	//					SetState(CharaState::Walk);

	//					//********攻撃処理******
	//					// 攻撃範囲内にプレイヤーがいるか
	//					if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), ATTACK_BS_RAD, m_pTarget->GetRadius())) {

	//						// 自分の攻撃射角の範囲内にいると攻撃
	//						float fAttackAngle;

	//						XMStoreFloat(&fAttackAngle, XMVector3AngleBetweenNormals(XMVector3Normalize(XMLoadFloat3(&vZ)), XMVector3Normalize(XMLoadFloat3(&vPlayer))));
	//						if (fAttackAngle < XMConvertToRadians(5.0f))
	//						{
	//							
	//							// ステート変更
	//							SetState(CharaState::HandRangeAttack);
	//							// アニメーション変更
	//							m_nNextAnimStack = ANIM_ATTACK;
	//						}
	//					}
	//				}
	//				else
	//				{
	//					// ステート変更
	//					SetState(CharaState::Wait);
	//					m_nNextAnimStack = ANIM_SERCH;
	//				}
	//			}
	//			else
	//			{
	//				// ステート変更
	//				SetState(CharaState::Wait);
	//				m_nNextAnimStack = ANIM_SERCH;
	//			}
	//		}
	//		else
	//		{
	//		    // ステート変更
	//		    SetState(CharaState::Wait);
	//		    m_nNextAnimStack = ANIM_SERCH;
	//		}
	//	}
	//	
	//	
	//	if (!m_bGround)
	//	{
	//		// 上下方向の加速度更新
	//		// 重力をかける
	//		m_Transform.SetCurAccel(
	//			XMFLOAT3(
	//				m_Transform.GetCurAccel().x + -GRAVITY_SCALE,
	//				m_Transform.GetCurAccel().y + -GRAVITY_SCALE,
	//				m_Transform.GetCurAccel().z + -GRAVITY_SCALE
	//			));
	//	}
	//	
	//	// 移動ベクトルに加算
	//	m_Transform.SetVel(
	//		XMFLOAT3(
	//			m_Transform.GetVel().x,
	//			m_Transform.GetVel().y + m_Transform.GetCurAccel().y,
	//			m_Transform.GetVel().z
	//		));
	//	if (m_Transform.GetVel().y < -LIMIT_VEL)
	//	{
	//		m_Transform.SetVel(
	//			XMFLOAT3(
	//				m_Transform.GetVel().x,
	//				m_Transform.GetVel().y + -LIMIT_VEL,
	//				m_Transform.GetVel().z
	//			));
	//	}
	//	// 移動ベクトル加算
	//	m_Transform.SetPos(
	//		XMFLOAT3(
	//			m_Transform.GetPos().x + m_Transform.GetVel().x,
	//			m_Transform.GetPos().y + m_Transform.GetVel().y,
	//			m_Transform.GetPos().z + m_Transform.GetVel().z
	//		));

	//	// ワールド変換行列に反映
	//	mW._41 = m_Transform.GetPos().x;
	//	mW._42 = m_Transform.GetPos().y;
	//	mW._43 = m_Transform.GetPos().z;
	//	m_Transform.SetWorld(mW);

	//	// 登録ノード更新
	//	Octree* pOctree = CScene::GetOctree();
	//	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);
	//	
	//	// 当たり判定
	//	Collision();

	//	m_pWeaponManager->Update();



	//	// 死亡判定
	//	if (m_Status.nHP <= 0)
	//	{
	//		// ライフ0
	//		m_Status.nHP = 0;
	//		// アニメスタック設定
	//		m_nNextAnimStack = ANIM_DYING;
	//		// ステート変更
	//		SetState(CharaState::Dying);
	//		// 武器終了処理
	//		m_pWeaponManager->Fin();
	//		// 死亡音
	//		CSound::Play(SE_ENEMY_DIE);
	//	}



	//} while (0);


	//// アニメーション更新
	//AnimeUpdate();
}


void CEnemy_Wizard::Draw()
{
	// アニメーション設定
	SetAnimIndex(m_nAnimStack);
	SetAnimTime(m_dAnimTime);

	CEnemy::Draw();
}

void CEnemy_Wizard::NearAttack()
{
	// 攻撃範囲内にプレイヤーがいるか
	if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), ATTACK_BS_RAD, m_pTarget->GetRadius())) 
	{
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
		if (fAttackAngle < XMConvertToRadians(5.0f))
		{
			// ステート変更
			SetState(CharaState::HandRangeAttack);
			// アニメーション変更
			m_nNextAnimStack = ANIM_ATTACK;
		}
	}
}

void CEnemy_Wizard::AnimeUpdate()
{
	

	// アニメーション番号更新
	switch (m_nAnimStack)
	{
	case ANIM_IDLE:
	case ANIM_SERCH:
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

	// アニメーション時間中に行う処理
	switch (m_nAnimStack)
	{
	case ANIM_ATTACK:
		m_nAttackCnt++;
		if (m_nAttackCnt == 25 )
		{
			// 攻撃開始
			m_pWeaponManager->Fire((int)(m_Status.nINT * TECRATE_TRIPLEMAGIC));
		}
		if (m_nAttackCnt == 30)
		{
			// 攻撃開始
			m_pWeaponManager->Fire((int)(m_Status.nINT * TECRATE_TRIPLEMAGIC));
		}
		if (m_nAttackCnt == 35)
		{
			// 攻撃開始
			m_pWeaponManager->Fire((int)(m_Status.nINT * TECRATE_TRIPLEMAGIC));
		}
		break;
	default:
		break;
	}
	
	// アニメーション開始してからの時間が総アニメーション時間を超えていた時の処理
	//double Double = GetAnimDuration(m_nAnimStack);
	if (m_dAnimTime >= GetAnimDuration(m_nAnimStack))
	{
		switch (m_nAnimStack) {
		case ANIM_IDLE:
		case ANIM_SERCH:
		case ANIM_WALK:
			// アニメーション番号を待機アニメーション番号に
			m_dAnimTime = 0.0;
			break;
		case ANIM_ATTACK:
		case ANIM_HITBACK:
			// アニメーション番号を待機アニメーション番号に
			m_dAnimTime = 0.0;
			// 再攻撃可能に
			m_pWeaponManager->Create(GOT_ENEMY);
			m_nAttackCnt = 0;
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

