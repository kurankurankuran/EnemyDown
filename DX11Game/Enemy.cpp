// 敵機 [Enemy.cpp]
#include "Enemy.h"
#include "Input.h"
#include "Player.h"
#include "Sound.h"
//#include "EffectManager.h"
#include "EffekseerManager.h"
#include "Polygon.h"
//#include "Texture.h"
#include "TextureData.h"
#include "EnemyManager.h"

// 定数
namespace {
	// ********ステータス********
	const int STATUS_MAX_LIFE = 2500;   // 最大ライフ
	const int STATUS_ATK = 500;		    // 物理攻撃力
	const int STATUS_DEF = 500;		    // 物理防御力
	const int STATUS_INT = 50;		    // 魔法攻撃力
	const int STATUS_RES = 25;		    // 魔法防御力

	// *********移動用変数*********
	const float MOVE_ACCEL = 0.2f;      // 加速度
	const float VEL_MAX = 2.5f;         // 最高速度
	const float ROLLSPEED = 0.5f;       // 振り向き速度　単位:度
	const float SEACH_RAD = 5000.0f;    // 索敵範囲

	// *********ライフUI用*********
	const XMFLOAT2 POS_LIFE_OFFSET = XMFLOAT2(0.0f, 15.0f);  // ライフゲージの座標(敵基準)
	const XMFLOAT2 SIZE_LIFEWAKU = XMFLOAT2(40.0f, 7.5f);    // ライフゲージの枠表示サイズ
	const XMFLOAT2 WAKUTHICKNESS = XMFLOAT2(2.5f, 2.5f);     // 枠の厚み
	const XMFLOAT2 LIFESIZEMAX =                             // 残りゲージの横と縦の大きさ
		XMFLOAT2(SIZE_LIFEWAKU.x - WAKUTHICKNESS.x, SIZE_LIFEWAKU.y - WAKUTHICKNESS.y);


	// *********攻撃用定数*********
	const float ATTACK_BS_RAD = 20.0f;  // 攻撃範囲
	const int ALLMOTION_ATTACK = 30;    // 総攻撃モーション時間
	const int START_TIME_ATTACK = 18;   // 攻撃判定出現時間(処理開始後からの経過時間)
	const float TECRATE_DEFAULT = 1.0f; // 技ごとのダメージ倍率

	// *********アニメーション用定数*********
	const double ANIM_SPEED = 0.015;    // アニメーション再生速度
	// アニメーションの種類
	const int ANIM_IDLE     = 0;		// 待機
	const int ANIM_WALK     = 1;		// 歩行
	const int ANIM_DYING    = 2;		// 死
	const int ANIM_ATTACK   = 4;		// 攻撃
	const int ANIM_HITBACK  = 5;        // 被ダメージ
										
}

// コンストラクタ
CEnemy::CEnemy(CScene* pScene) : CModel(pScene)
{
	SetTag(GOT_ENEMY);
	m_pTarget = nullptr;
	m_pLand = nullptr;
	m_Transform.SetMoveAccel(MOVE_ACCEL);
	m_Transform.SetVelMax(VEL_MAX);
	m_fAtkRad = ATTACK_BS_RAD;
	m_fSeachRad = SEACH_RAD;
	m_empEBaseAnimationToInt[IDLE_BA] = ANIM_IDLE;
	m_empEBaseAnimationToInt[WALK_BA] = ANIM_WALK;
	m_empEBaseAnimationToInt[DYING_BA] = ANIM_DYING;
	m_empEBaseAnimationToInt[HITBACK_BA] = ANIM_HITBACK;

}

// デストラクタ
CEnemy::~CEnemy()
{
	

}

// 初期化
HRESULT CEnemy::Init()
{
	HRESULT hr = CModel::Init();
	// 不可視
	m_bVisible = false;
	// 生成エフェクト開始
	m_nCreateEffectTimer = TIMER_ENEMY_CREATE_EFK;
	m_nEfkID = CEffekseerManager::Play(CEffekseerManager::GetPath(ENEMY_CREATE_EFK), false, TIMER_ENEMY_CREATE_EFK);
	CEffekseerManager::SetMatrix(m_nEfkID, false, m_Transform.GetWorld());
	CEffekseerManager::SetScale(m_nEfkID, false, XMFLOAT3(
		m_Transform.GetScale().x * SCALE_ENEMY_CREATE_EFK,
		m_Transform.GetScale().y * SCALE_ENEMY_CREATE_EFK,
		m_Transform.GetScale().z * SCALE_ENEMY_CREATE_EFK));
	// **********アニメーション用設定**********
	// アニメーション内部時間
	m_dAnimTime = 0.0;
	// 現在のアニメーション番号
	m_nAnimStack = m_empEBaseAnimationToInt[IDLE_BA];
	// 次アニメーションの番号
	m_nNextAnimStack = m_nAnimStack;
	// **********アニメーション用設定ここまで**********
	return hr;
}

// 終了処理
void CEnemy::Fin()
{
	CEnemyManager::DelEnemyCnt();
	CModel::Fin();
}

// 更新
void CEnemy::Update()
{
	// 生成時エフェクトタイマー更新
	if (m_nCreateEffectTimer)
	{
		--m_nCreateEffectTimer;
		CEffekseerManager::SetMatrix(m_nEfkID, false, m_Transform.GetWorld());
		CEffekseerManager::SetScale(m_nEfkID, false, XMFLOAT3(
			m_Transform.GetScale().x * SCALE_ENEMY_CREATE_EFK,
			m_Transform.GetScale().y * SCALE_ENEMY_CREATE_EFK,
			m_Transform.GetScale().z * SCALE_ENEMY_CREATE_EFK));
		if (!m_bVisible && m_nCreateEffectTimer < TIMER_ENEMY_CREATE_EFK * 0.25f)
		{
			m_bVisible = true;
			m_nCreateEffectTimer = 0;
			// 武器初期化
			m_pWeaponManager->Init(&m_Transform.GetWorld());
			m_pWeaponManager->Create(m_Tag);
		}
	}

	// 死亡時の処理
	if (m_nDethEffectTimer) {
		if (m_nDethEffectTimer) {
			--m_nDethEffectTimer;
			if (!m_nDethEffectTimer) {
				Fin();
				delete(this);
				return;
			}
		}
	}

	// 無敵時間更新
	if (0 < m_nInvTime)
	{
		--m_nInvTime;
		if (m_nInvTime < 0)
		{
			m_nInvTime = 0;
		}
	}

	
	// 加速度リセット
	m_Transform.SetCurAccel(XMFLOAT3(0, 0, 0));
	
	if (!m_bVisible)return;
		
	// 向き修正のためワールド行列から座標情報を一時抜き出し保存
	// ワールド変換行列取得
	XMFLOAT4X4 mW = m_Transform.GetWorld();
	// モデル座標軸取得
	XMFLOAT3 vX = XMFLOAT3(mW._11, mW._12, mW._13);
	XMFLOAT3 vY = XMFLOAT3(mW._21, mW._22, mW._23);
	XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
	// 座標取得、クリア
	m_Transform.SetPos(XMFLOAT3(mW._41, mW._42, mW._43));
	mW._41 = mW._42 = mW._43 = 0.0f;


	// 範囲内のプレイヤー方向に向きを変える
	if (CharaState::HandRangeAttack != GetState() &&
		CharaState::HitBack != GetState() &&
		CharaState::Dying != GetState())
	{
		if (m_pTarget)
		{
			if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), m_fSeachRad, m_pTarget->GetRadius())) 
			{
				if (m_pTarget->GetVisible()) 
				{
					 //プレイヤーへのベクトルを取得
					XMFLOAT3& vPlayerPos = m_pTarget->GetTransform()->GetPos();
					XMFLOAT3 vPlayer;
					vPlayer.x = vPlayerPos.x - m_Transform.GetPos().x;
					vPlayer.y = vPlayerPos.y - m_Transform.GetPos().y;
					vPlayer.z = vPlayerPos.z - m_Transform.GetPos().z;
					XMVECTOR vecPlayer;
					vecPlayer = XMVector3Normalize(XMLoadFloat3(&vPlayer));

					// ターゲットが左右どちらか
					XMMATRIX mR = XMMatrixIdentity();

					float fRight = vX.x * vPlayer.x + vX.y * vPlayer.y + vX.z * vPlayer.z;	// 内積
					XMStoreFloat(&fRight, XMVector3Dot(XMLoadFloat3(&vX), vecPlayer));

					if (fRight > 0.2f)		// 右
						mR *= XMMatrixRotationAxis(XMLoadFloat3(&vY), XMConvertToRadians(m_Transform.GetRollSpeed()));
					else if (fRight < -0.2f)	// 左
						mR *= XMMatrixRotationAxis(XMLoadFloat3(&vY), XMConvertToRadians(-m_Transform.GetRollSpeed()));

					XMStoreFloat4x4(&mW, XMMatrixMultiply(XMLoadFloat4x4(&mW), mR));

					// 加速度設定
					m_Transform.SetCurAccel(
						XMFLOAT3(
							mW._31 * m_Transform.GetMoveAccel(),
							mW._32 * m_Transform.GetMoveAccel(),
							mW._33 * m_Transform.GetMoveAccel()));

					XMMATRIX worldTestWorld = m_Transform.ExtractRotation(XMLoadFloat4x4(&m_Transform.GetWorld()));

					XMFLOAT4X4 mTestWorld;
					XMStoreFloat4x4(&mTestWorld, worldTestWorld);

					// アニメーション変更
					m_nNextAnimStack = m_empEBaseAnimationToInt[WALK_BA];
					// ステート変更
					SetState(CharaState::Walk);

					//********攻撃処理******
					if (m_pWeaponManager)
					{
						NearAttack();
					}
				}
				else
				{
					// ステート変更
					SetState(CharaState::Wait);
					m_nNextAnimStack = m_empEBaseAnimationToInt[IDLE_BA];
				}
			}
			else
			{
				// ステート変更
				SetState(CharaState::Wait);
				m_nNextAnimStack = m_empEBaseAnimationToInt[IDLE_BA];
			}
		}
		else
		{
			// ステート変更
			SetState(CharaState::Wait);
			m_nNextAnimStack = m_empEBaseAnimationToInt[IDLE_BA];
		}
	}

	if (!m_bGround)
	{
		// 上下方向の加速度更新
		// 重力をかける
		m_Transform.SetCurAccel(
			XMFLOAT3(
				m_Transform.GetCurAccel().x,
				m_Transform.GetCurAccel().y + -GRAVITY_SCALE * m_Transform.GetGravityScale(),
				m_Transform.GetCurAccel().z
			));
	}
	

	mW._41 = m_Transform.GetPos().x;
	mW._42 = m_Transform.GetPos().y;
	mW._43 = m_Transform.GetPos().z;
	m_Transform.SetWorld(mW);


	CModel::Update();

	// 登録ノード更新
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

	// 当たり判定
	Collision();

	m_pWeaponManager->Update();


	// のけぞり処理
	if (CharaState::HitBack == GetState())
	{
		m_nNextAnimStack = m_empEBaseAnimationToInt[HITBACK_BA];
	}
	// 死亡判定
	if (m_Status.nHP <= 0 && GetState() != CharaState::Dying)
	{
		// ライフ0
		m_Status.nHP = 0;
		// アニメスタック設定
		m_nNextAnimStack = m_empEBaseAnimationToInt[DYING_BA];
		// ステート変更
		SetState(CharaState::Dying);
		// 武器終了処理
		m_pWeaponManager->Fin();
		// 死亡音
		CSound::Play(SE_ENEMY_DIE);
	}

	AnimeUpdate();
}


void CEnemy::Draw()
{
	if (!m_bVisible)return;

	SetShadowRender();
	m_pBBox->SetWorld(&m_Transform.GetWorld());
	m_pBBox->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_pBBox->DepthDraw();
	SetDefaultRender();

	CModel::Draw();
	
}
//********************
// UI描画
//********************
void CEnemy::DrawUI()
{
	if (!m_bVisible)return;
	ID3D11DeviceContext* pDC = GetDeviceContext();
	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
	SetZWrite(false);				// Zバッファ更新しない

	// ライフバー
	// ライフ枠
	// 3D→2D変換
	// 表示位置はOBBの上辺の位置
	XMFLOAT3 vDrawPos = m_Transform.GetPos();
	vDrawPos.x += m_vCenter.x + m_vBBox.x;
	vDrawPos.y += m_vCenter.y + m_vBBox.y;
	vDrawPos.z += m_vCenter.z + m_vBBox.z;

	CCamera* pCamera = m_pScene->GetCameraManager()->GetCamera();
	XMMATRIX mProj = XMLoadFloat4x4(&pCamera->GetProjMatrix());
	XMMATRIX mView = XMLoadFloat4x4(&pCamera->GetViewMatrix());
	XMMATRIX mWorld = XMMatrixIdentity();
	XMVECTOR vecPos = XMVector3Project(
		XMLoadFloat3(&vDrawPos),
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		0.0f, 1.0f,
		mProj,
		mView,
		mWorld);

	XMFLOAT3 vScreenPos;
	XMStoreFloat3(&vScreenPos, vecPos);
	vScreenPos.x -= SCREEN_CENTER_X;
	vScreenPos.y -= SCREEN_CENTER_Y;

	if (vScreenPos.z <= 1.0f)
	{
		// ライフ枠
		CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_WAKU)/*m_pTextureUI[UITEX_GAGE_WAKU]*/);
		CPolygon::SetPos(vScreenPos.x + POS_LIFE_OFFSET.x, -vScreenPos.y + POS_LIFE_OFFSET.y);
		CPolygon::SetSize(SIZE_LIFEWAKU.x, SIZE_LIFEWAKU.y);
		CPolygon::SetUV(1.0f, 1.0f);
		CPolygon::SetColor(1.0f, 1.0f, 1.0f);
		CPolygon::SetAlpha(1.0f);
		CPolygon::Draw(pDC);


		// ライフ背景
		CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_HAIKEI)/*m_pTextureUI[UITEX_GAGE_HAIKEI]*/);
		CPolygon::SetPos(vScreenPos.x + POS_LIFE_OFFSET.x, -vScreenPos.y + POS_LIFE_OFFSET.y);
		CPolygon::SetSize(LIFESIZEMAX.x, LIFESIZEMAX.y);
		CPolygon::SetColor(1.0f, 1.0f, 1.0f);
		CPolygon::SetAlpha(1.0f);
		CPolygon::Draw(pDC);

		// 残りライフ
		float fRateWork; // ライフと武器それぞれの現在値/最大値割合
		fRateWork = (float)m_Status.nHP / (float)m_Status.nMaxHP;
		if (m_Status.nHP < m_Status.nMaxHP / 3)
		{
			CPolygon::SetTexture(CTextureData::GetTextureResource(LIFEGAGE_RED));
		}
		else
		{
			CPolygon::SetTexture(CTextureData::GetTextureResource(LIFEGAGE_GREEN));
		}
		CPolygon::SetPos((vScreenPos.x + POS_LIFE_OFFSET.x) - (LIFESIZEMAX.x / 2.0f) * (1.0f - fRateWork), -vScreenPos.y + POS_LIFE_OFFSET.y);
		CPolygon::SetSize(LIFESIZEMAX.x * fRateWork, LIFESIZEMAX.y);
		CPolygon::SetUV(1.0f/*SIZE_LIFEWAKU.x * fRateWork*/, 1.0f);
		CPolygon::SetColor(1.0f, 1.0f, 1.0f);
		CPolygon::SetAlpha(1.0f);
		CPolygon::Draw(pDC);
	}

	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(0.0f, 0.0f, 0.0f);

	SetZWrite(true);							// Zバッファ更新する
	SetBlendState(BS_NONE);		                // アルファブレンド無効
}

void CEnemy::NearAttack()  // 近距離攻撃
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
			m_pWeaponManager->Fire((int)(m_Status.nATK * TECRATE_DEFAULT), ALLMOTION_ATTACK, START_TIME_ATTACK);
			// ステート変更
			SetState(CharaState::HandRangeAttack);
			// アニメーション変更
			m_nNextAnimStack = ANIM_ATTACK;
		}
	}
}


// バウンディボックス
bool CEnemy::BS(XMFLOAT3 vA, XMFLOAT3 vB, float RadA, float RadB)
{
	// 衝突判定
	float dx = vA.x - vB.x;
	float dy = vA.y - vB.y;
	float dz = vA.z - vB.z;
	float dr = RadA + RadB;
	return dx * dx + dy * dy + dz * dz <= dr * dr;
}
