// プレイヤー [Player.cpp]
#include "Player.h"
#include "Input.h"
#include "DebugProc.h"
#include "Scene.h"
#include "MGManager.h"
#include "SwordAttackManager.h"
#include "Land.h"
#include "Sound.h"
#include "Fade.h"
#include "Polygon.h"
#include "Texture.h"
#include "EffekseerManager.h"
#include "Text.h"
#include "TextureData.h"

//**************************************************************************************************************
// 定数
//**************************************************************************************************************
namespace {
	// ********ステータス********
	const int STATUS_MAX_LIFE = 1000;
	const int STATUS_ATK = 1000;
	const int STATUS_DEF =  500;
	const int STATUS_INT =  250;
	const int STATUS_RES =  250;

	//  *********ライフ用定数*********
	// ライフゲージの枠表示サイズ
	const XMFLOAT2 SIZE_LIFEWAKU = XMFLOAT2(300.0f, 20.0f);
	// ライフゲージの枠表示位置(画面左上から)
	const XMFLOAT2 POS_LIFEWAKU_OFFSET = XMFLOAT2(50.0f, 40.0f);
	// ライフゲージの枠と武器ゲージの枠の間隔
	const float LIFETOWEAPONDISTANCE = 40.0f;
	// 枠の厚み
	const XMFLOAT2 WAKUTHICKNESS = XMFLOAT2(10.0f, 5.0f);
	// 残りゲージの横と縦の大きさ
	const XMFLOAT2 LIFESIZEMAX = XMFLOAT2(SIZE_LIFEWAKU.x - WAKUTHICKNESS.x, SIZE_LIFEWAKU.y - WAKUTHICKNESS.y);
	
	
	//  *********移動用定数*********
	// 移動速度限界
	const float LIMIT_VEL = 50.0f;
	// 歩く速度
	const float WALKSPEED = 20.0f;
	// 走る速度
	const float DASHSPEED = 40.0f;
	// ダッシュ可能回数
	const int DASH_NUM = 1;
	// ダッシュ移動量
	const float DASHVOL = 50.0f;
	// ダッシュ移動量減衰係数
	const float DASHVOLDEL = 0.95f;
	// ダッシュ移動量減衰限界
	const float DASHVOLDELLIMIT = 10.0f;
	// ジャンプ力
	const float JAUMPPOWER = 55.0f;

	// *********アニメーション用定数*********
	// アニメーション再生速度
	const double ANIM_SPEED = 0.03;
	// アニメーションの種類
	const int ANIM_IDLE = 0;
	const int ANIM_WALK = 1;
	const int ANIM_RUN = 2;
	const int ANIM_JUMP = 3;
	const int ANIM_SLASH = 4;
	const int ANIM_HITBACK= 5;
	const int ANIM_DYING = 6;
	
	// *********その他定数***********
	// 死んだときの時間をゆっくりにする時間？（カウント）
	const int TIME_SLOW_CNT = 30;

	// テキストのサイズ
	const XMFLOAT2 TEXT_SIZE = XMFLOAT2(20.0f, 40.0f);
	// ライフゲージ位置計算用
	const XMFLOAT2 POS_LIFEWAKU_CALCULAT = XMFLOAT2(-SCREEN_CENTER_X + SIZE_LIFEWAKU.x / 2 + POS_LIFEWAKU_OFFSET.x/*画面左端からの距離*/, SCREEN_CENTER_Y - POS_LIFEWAKU_OFFSET.y);
	const std::string TEXT_HP = std::string("HP");
	const XMFLOAT2 TEXT_HP_POS = XMFLOAT2(-SCREEN_WIDTH / 2 + TEXT_SIZE.x, POS_LIFEWAKU_CALCULAT.y + 5);
	const XMFLOAT2 TEXT_HP_SIZE = TEXT_SIZE;
	const XMFLOAT4 TEXT_HP_COLOR = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	// MPゲージ
	const std::string TEXT_MP = std::string("MP");
	const XMFLOAT2 TEXT_MP_POS = XMFLOAT2(-SCREEN_WIDTH / 2 + TEXT_SIZE.x, POS_LIFEWAKU_CALCULAT.y - TEXT_HP_SIZE.y + 5);
	const XMFLOAT2 TEXT_MP_SIZE = TEXT_SIZE;
	const XMFLOAT4 TEXT_MP_COLOR = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	const int TIME_CNT = 100; // 画面枠赤くするようカウント（仮）
}


//**************************************************************************************************************
// コンストラクタ
//**************************************************************************************************************
CPlayer::CPlayer(CScene* pScene) : CModel(pScene)
{
	SetTag(GOT_PLAYER);
	SetObjID(EObjID::OI_PLAYER);
	m_pLand = nullptr;
	m_pCamera = nullptr;
	m_nAnimStack = ANIM_IDLE;
	m_nNextAnimStack = m_nAnimStack;
	m_dAnimTime = 0.0;
	
}
//**************************************************************************************************************
// デストラクタ
//**************************************************************************************************************
CPlayer::~CPlayer()
{
}
//**************************************************************************************************************
// 初期化
//**************************************************************************************************************
HRESULT CPlayer::Init()
{
	m_nTimeCnt = 0;   // 画面枠を赤くする用（仮）
	m_nPlusCnt = 1;   // 画面枠を赤くする用（仮）
	// **********ステータス関係**********
	// モデルと当たり判定設定
	SetModel(MT_PALADIN);
	m_fRadius = PLAYER_BS_RAD;
	m_vBBox = XMFLOAT3(50.f, m_vCenter.y, 30.0f);
	
	m_vCenter.x = 0;
	m_vCenter.z = 0;

	// 現在のステート
	SetState(Wait);
	// 地面設置フラグ
	m_bGround = false;
	// ダッシュ可能回数
	m_nDashNum = DASH_NUM;
	// ダッシュ移動量
	m_fDashVol = 0.0f;
	
	// ステータス
	m_Status.nMaxHP = STATUS_MAX_LIFE;
	m_Status.nHP = m_Status.nMaxHP;
	m_Status.nATK = STATUS_ATK;
	m_Status.nDEF = STATUS_DEF;
	m_Status.nINT = STATUS_INT;
	m_Status.nRES = STATUS_RES;
	
	// ノードに登録
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

	// **********ステータス関係ここまで**********
	// **********アニメーション用設定**********
	// アニメーション内部時間
	m_dAnimTime = 0.0;
	// 現在のアニメーション番号
	m_nAnimStack = ANIM_IDLE;
	// 次アニメーションの番号
	m_nNextAnimStack = m_nAnimStack;
	// **********アニメーション用設定ここまで**********

	// **********参照用ポインタ設定**********
	// カメラポインタ
	m_pCamera = m_pScene->GetCameraManager()->GetCamera();
	// 地形モデルポインタ
	m_pLand = (CLand*)m_pScene->Find(GOT_LAND);
	// **********参照用ポインタ設定ここまで**********
	
	CText::OutPutText(TEXT_HP, TEXT_HP_POS, TEXT_HP_SIZE, TEXT_HP_COLOR);
	CText::OutPutText(TEXT_MP, TEXT_MP_POS, TEXT_MP_SIZE, TEXT_MP_COLOR);
	
	HRESULT hr = CModel::Init();

	// 子の初期化
	// 武器
	m_uqpAttackMethod = std::make_unique<CAttackMethod>(this);

	return hr;
}

// 終了処理
void CPlayer::Fin()
{
	// 武器終了処理
	m_uqpAttackMethod->Fin();

	CModel::Fin();
}
//**************************************************************************************************************
// 更新
//**************************************************************************************************************
void CPlayer::Update()
{
	// 死亡演出
	if (m_nDethEffectTimer) {
		if (m_nDethEffectTimer) {
			--m_nDethEffectTimer;
			if (!m_nDethEffectTimer) {
				// シーン遷移
				CFade::Out(SCENE_GAMEOVER);
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

	// のけぞり処理
	if (CharaState::HitBack == m_State)
	{
		m_nNextAnimStack = ANIM_HITBACK;

		// 死亡判定
		if (m_Status.nHP <= 0)
		{
			// ライフ0
			m_Status.nHP = 0;
			// アニメスタック設定
			m_nNextAnimStack = ANIM_DYING;
			// ステート変更
			SetState(CharaState::Dying);
			// 武器終了処理
			m_uqpAttackMethod->Fin();
			// 時止め演出
			m_pScene->SetTimeSlowCnt(TIME_SLOW_CNT);
			// 死亡音
			CSound::Play(SE_PLAYER_DIE);
		}
	}



	do {
		if (!m_bVisible)
			break;

		if (Dying == m_State)
			break;

		if (CharaState::HitBack == m_State)
			break;

		// ワールド変換行列取得
		XMFLOAT4X4& mW = m_Transform.GetWorld();
		// モデル座標軸取得
		XMFLOAT3 vX = XMFLOAT3(mW._11, mW._12, mW._13);
		XMFLOAT3 vY = XMFLOAT3(mW._21, mW._22, mW._23);
		XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
		// 座標取得、クリア
		m_Transform.SetPos(XMFLOAT3(mW._41, mW._42, mW._43));
		mW._41 = mW._42 = mW._43 = 0.0f;

		// 移動速度ベクトル(XMFLOAT3)
		m_Transform.SetVel(XMFLOAT3(0.0f, 0.0f, 0.0f));
		
		// デフォルトステートの設定
		// 攻撃中でない
		if (CharaState::HandRangeAttack != m_State) {

			// リロード中でない
			if (CharaState::Reload != m_State &&
				CharaState::ReloadWalk != m_State) {

				// ダッシュ中でない
				if (CharaState::Dash != m_State && CharaState::DashJump != m_State) {

					// ジャンプ中でない
					if (Jump != m_State) {
						// デフォルトステート設定
						SetState(Wait);
						// デフォルトアニメーション（待機）
						m_nNextAnimStack = ANIM_IDLE;
					}
				}
			}
		    // ********移動********
			MouseMove(m_Transform.GetVel(), mW);
		}


		// ジャンプ処理
		//接地しているならジャンプ可能
		if (m_bGround) {
			// 待機、歩き、ダッシュ状態から遷移可能
			if ((
				CharaState::Wait == m_State ||
				CharaState::Walk == m_State ||
				CharaState::Dash == m_State ||
				CharaState::HandRangeAttack != m_State 
				) &&
				CInput::GetKeyTrigger(VK_SPACE))
			{
				// Y軸方向の加速度
				m_Transform.SetCurAccel(
					XMFLOAT3(
						m_Transform.GetCurAccel().x,
						m_Transform.GetCurAccel().y + JAUMPPOWER,
						m_Transform.GetCurAccel().z));

				// ダッシュ中ならダッシュジャンプ状態に遷移
				if (CharaState::Dash != m_State) {
					// ステート設定
					SetState(CharaState::Jump);
				}
				else {
					SetState(CharaState::DashJump);
				}
				// アニメーション番号設定
				m_nNextAnimStack = ANIM_JUMP;// ジャンプ
			}
		}
		
		if (!m_bGround)
		{
			// 上下方向の加速度更新
			// 重力をかける
			m_Transform.SetCurAccel(
				XMFLOAT3(
					m_Transform.GetCurAccel().x,
					m_Transform.GetCurAccel().y + -GRAVITY_SCALE,
					m_Transform.GetCurAccel().z));
		}


		// 移動ベクトルに加算
		m_Transform.SetVel(
			XMFLOAT3(
				m_Transform.GetVel().x + m_Transform.GetCurAccel().x, 
				m_Transform.GetVel().y + m_Transform.GetCurAccel().y, 
				m_Transform.GetVel().z + m_Transform.GetCurAccel().z));
		if (m_Transform.GetVel().y < -LIMIT_VEL)
		{
			m_Transform.SetVel(
				XMFLOAT3(
					m_Transform.GetVel().x + m_Transform.GetCurAccel().x,
					-LIMIT_VEL,
					m_Transform.GetVel().z + m_Transform.GetCurAccel().z));
		}

		//**********移動ここまで**********

		//**********攻撃処理**********
		m_uqpAttackMethod->AttackMethod();
		if (CharaState::HandRangeAttack == m_State)
		{
			// アニメーション番号設定
			m_nNextAnimStack = ANIM_SLASH;// 立ち攻撃
		}
		DrawCurrentState(m_State);
		

		// 移動ベクトル加算
		m_Transform.SetPos(
			XMFLOAT3(
				m_Transform.GetPos().x + m_Transform.GetVel().x,
				m_Transform.GetPos().y + m_Transform.GetVel().y,
				m_Transform.GetPos().z + m_Transform.GetVel().z
			));
		
		// ワールド変換行列に反映
		mW._41 = m_Transform.GetPos().x;
		mW._42 = m_Transform.GetPos().y;
		mW._43 = m_Transform.GetPos().z;

		// ワールド行列設定
		//SetWorld(mW);

		// 登録ノード更新
		Octree* pOctree = CScene::GetOctree();
		pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

		// **********当たり判定**********
		Collision();
		//**********当たり判定ここまで**********
		if (m_bGround)
		{
			if (CharaState::DashJump == m_State)
			{
				m_fDashVol = 0;
			}

			// ダッシュ可能数回復
			if (m_nDashNum <= 0)
			{
				m_nDashNum = DASH_NUM;
			}
		}

		
		// TODO ライフ関係の処理も全てのキャラクタ共通なので基底クラスなどで行うべき
		if (m_Status.nHP <= 0)
		{
			// ステート更新
			SetState(Dying);
			// アニメスタック設定
			m_nNextAnimStack = ANIM_DYING;
			// 武器終了処理
			m_uqpAttackMethod->Fin();
			// HPは０
			m_Status.nHP = 0;

			// 時止め演出
			CScene::SetTimeSlowCnt(TIME_SLOW_CNT);
			// 死亡音
			CSound::Play(SE_PLAYER_DIE);
		}

		// ********** 武器更新 **********
		m_uqpAttackMethod->Update();
	
	} while (0);

	// **********アニメーション更新**********
	AnimationUpdate();
	
	// 画面枠用カウント更新（仮）
	if (m_Status.nHP < m_Status.nMaxHP / 3)
	{
		m_nTimeCnt += m_nPlusCnt;
		if (m_nTimeCnt < 0 || TIME_CNT <= m_nTimeCnt)
		{
			m_nPlusCnt *= -1;
		}
	}
	else
	{
		m_nTimeCnt = 0;
	}

	// デバッグ表示
	CDebugProc::Print("*** ﾌﾟﾚｲﾔｰ ***\n");
	CDebugProc::Print("Dash:[SPACE]\n");
	CDebugProc::Print("Pitch:[W][S]\n");
	CDebugProc::Print("Roll:[A][D]\n");
	CDebugProc::Print("Fire  Missile:[L]\n");
	CDebugProc::Print("Reload  Missile:[J]\n");
	CDebugProc::Print("AngleToF Missile:[I]\n");
	CDebugProc::Print("AngleToB Missile:[K]\n");
	CDebugProc::Print("Pos :X[%f] Y[%f] Z[%f]\n", m_Transform.GetPos().x, m_Transform.GetPos().y, m_Transform.GetPos().z);
	CDebugProc::Print("Accel :X[%f] Y[%f] Z[%f]\n", m_Transform.GetCurAccel().x, m_Transform.GetCurAccel().y, m_Transform.GetCurAccel().z);
	CDebugProc::Print("\n");
}


//**************************************************************************************************************
// 描画
//**************************************************************************************************************
void CPlayer::Draw()
{
	if (!m_bVisible) return;
	
	SetAnimIndex(m_nAnimStack);
	SetAnimTime(m_dAnimTime);
	CModel::Draw();

	SetShadowRender();
	m_pBBox->SetWorld(&m_Transform.GetWorld());
	m_pBBox->SetColor(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pBBox->DepthDraw();
	SetDefaultRender();

}


//***********
// UI描画
//*********** 
void CPlayer::DrawUI()
{
	if (!m_bVisible) return;

	ID3D11DeviceContext* pDC = GetDeviceContext();

	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
	SetZWrite(false);				// Zバッファ更新しない

	// ライフバー
	// ライフ枠
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_WAKU));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x, POS_LIFEWAKU_CALCULAT.y);
	CPolygon::SetSize(SIZE_LIFEWAKU.x, SIZE_LIFEWAKU.y);
	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);
	// ライフ背景
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_HAIKEI));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x, POS_LIFEWAKU_CALCULAT.y);
	CPolygon::SetSize(LIFESIZEMAX.x, LIFESIZEMAX.y);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);

	
	// 残りライフ
	float fRateWork; // ライフと武器それぞれの現在値/最大値割合
	fRateWork = (float)m_Status.nHP / (float)STATUS_MAX_LIFE;
	if (m_Status.nHP < m_Status.nMaxHP / 3)
	{
		CPolygon::SetTexture(CTextureData::GetTextureResource(LIFEGAGE_RED));
	}
	else
	{
		CPolygon::SetTexture(CTextureData::GetTextureResource(LIFEGAGE_GREEN));
	}
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x - (LIFESIZEMAX.x / 2.0f) * (1.0f - fRateWork), POS_LIFEWAKU_CALCULAT.y);
	CPolygon::SetSize(LIFESIZEMAX.x * fRateWork, LIFESIZEMAX.y);
	CPolygon::SetUV(1.0f/*SIZE_LIFEWAKU.x * fRateWork*/, 1.0f);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);

	// 武器ゲージ
	// 武器枠
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_WAKU));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x, POS_LIFEWAKU_CALCULAT.y - LIFETOWEAPONDISTANCE);
	CPolygon::SetSize(SIZE_LIFEWAKU.x, SIZE_LIFEWAKU.y);
	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);
	// 武器背景
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_HAIKEI));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x, POS_LIFEWAKU_CALCULAT.y - LIFETOWEAPONDISTANCE);
	CPolygon::SetSize(LIFESIZEMAX.x, LIFESIZEMAX.y);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);
	// 残り武器ゲージ
	int nMagazineNum = m_uqpAttackMethod->GetLongWeapon()->GetMagazineNum();
	fRateWork = (float)nMagazineNum / (float)MAX_MACHINEGUN;
	CPolygon::SetTexture(CTextureData::GetTextureResource(MP_GAGE));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x - (LIFESIZEMAX.x / 2.0f) * (1.0f - fRateWork), POS_LIFEWAKU_CALCULAT.y - LIFETOWEAPONDISTANCE);
	CPolygon::SetSize(LIFESIZEMAX.x * fRateWork, LIFESIZEMAX.y);
	CPolygon::SetUV(1.0f/*SIZE_LIFEWAKU.x * fRateWork*/, 1.0f);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);

	if (m_Status.nHP < m_Status.nMaxHP / 3)
	{
		// ピンチ時の画面端を赤くする
		CPolygon::SetTexture(CTextureData::GetTextureResource(PINCHI_WAKU_RED));
		CPolygon::SetPos(0.0f, 0.0f);
		CPolygon::SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		CPolygon::SetUV(1.0f, 1.0f);
		CPolygon::SetColor(1.0f, 1.0f, 1.0f);
		CPolygon::SetAlpha((1.0f / TIME_CNT) * m_nTimeCnt);
		CPolygon::Draw(pDC);
	}

	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(0.0f, 0.0f, 0.0f);

	SetZWrite(true);							// Zバッファ更新する
	SetBlendState(BS_NONE);		                // アルファブレンド無効
}

//**************************************************************************************************************
// 可視変更フラグ
//**************************************************************************************************************
void CPlayer::SetVisible(bool bVisible)
{ 
	m_bVisible = bVisible; 
	//m_pHandWeaponManager->SetVisible(bVisible);
	//m_pLongRangeWeaponManager->SetVisible(bVisible);
}

//=============================================================================
// 移動処理まとめ : キーボードとマウス操作
//=============================================================================
void CPlayer::MouseMove(XMFLOAT3& vMove, XMFLOAT4X4& mW)
{

	// 移動ベクトル(XMVECTOR)
	XMVECTOR vecMove = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	// カメラワールド変換行列取得
	XMFLOAT4X4& mtxCameraWorld = m_pCamera->GetWorldMatrix();
	// モデル座標軸取得
	XMFLOAT3 vX = XMFLOAT3(mtxCameraWorld._11, mtxCameraWorld._12, mtxCameraWorld._13);
	XMFLOAT3 vY = XMFLOAT3(mtxCameraWorld._21, mtxCameraWorld._22, mtxCameraWorld._23);
	XMFLOAT3 vZ = XMFLOAT3(mtxCameraWorld._31, mtxCameraWorld._32, mtxCameraWorld._33);
	// **********移動**********
	// キー入力による移動方向決定用ベクトル(量は含まない)
	XMVECTOR InputMove = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	if (CInput::GetKeyPress(VK_W)) {
		// 単位化して加算
 		InputMove -= XMVector3Normalize(XMVector3Cross(
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
			XMVectorSet(
				vX.x,
				0.0f,
				vX.z,
				1.0f)));
		// 単位化
		InputMove = XMVector3Normalize(InputMove);

		// ステート設定
		if (CharaState::Dash != m_State  && CharaState::DashJump != m_State) {
			if (CharaState::Reload == m_State || CharaState::ReloadWalk == m_State)
			{
				// ステート変更
				SetState(CharaState::ReloadWalk);
			}
			else
			{
				// ステート変更
				SetState(CharaState::Walk);
			}
		}
		// アニメーション番号設定
		m_nNextAnimStack = ANIM_RUN;
	}
	if (CInput::GetKeyPress(VK_S)) {
		// 単位化して加算
		InputMove += XMVector3Normalize(XMVector3Cross(
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
			XMVectorSet(
				vX.x,
				0.0f,
				vX.z,
				1.0f)));
		// 単位化
		InputMove = XMVector3Normalize(InputMove);

		// ステート設定
		if (CharaState::Dash != m_State  && CharaState::DashJump != m_State) {
			if (CharaState::Reload == m_State || CharaState::ReloadWalk == m_State)
			{
				// ステート変更
				SetState(CharaState::ReloadWalk);
			}
			else
			{
				// ステート変更
				SetState(CharaState::Walk);
			}
		}

		// アニメーション番号設定
		m_nNextAnimStack = ANIM_RUN;
	}
	if (CInput::GetKeyPress(VK_A)) {
		// 単位化して加算
		InputMove -= XMVector3Normalize(XMVectorSet(
			vX.x,
			0.0f,
			vX.z,
			1.0f));
		// 単位化
		InputMove = XMVector3Normalize(InputMove);

		// ステート設定
		if (CharaState::Dash != m_State && CharaState::DashJump != m_State) {
			if (CharaState::Reload == m_State || CharaState::ReloadWalk == m_State)
			{
				// ステート変更
				SetState(CharaState::ReloadWalk);
			}
			else
			{
				// ステート変更
				SetState(CharaState::Walk);
			}
		}

		// アニメーション番号設定
		m_nNextAnimStack = ANIM_RUN;
	}
	if (CInput::GetKeyPress(VK_D)) {
		// 単位化して加算
		InputMove += XMVector3Normalize(XMVectorSet(
			vX.x,
			0.0f,
			vX.z,
			1.0f));
		// 単位化
		InputMove = XMVector3Normalize(InputMove);

		// ステート設定
		if (CharaState::Dash != m_State && CharaState::DashJump != m_State) {
			if (CharaState::Reload == m_State || CharaState::ReloadWalk == m_State)
			{
				// ステート変更
				SetState(CharaState::ReloadWalk);
			}
			else
			{
				// ステート変更
				SetState(CharaState::Walk);
			}
		}

		// アニメーション番号設定
		m_nNextAnimStack = ANIM_RUN;
	}

	//*********************
	// 移動方向の決定
	//*********************
	// 反映し単位化:XZ平面の移動方向決定
	vecMove += InputMove;
	vecMove = XMVector3Normalize(vecMove);
	// モデルの向き(移動方向に向ける)
	m_Transform.RotationModel(XMFLOAT3(XMVectorGetX(vecMove), XMVectorGetY(vecMove), XMVectorGetZ(vecMove)));

	//*********************
	// 移動量の決定
	//*********************
	// ステートが歩き状態なら移動量設定
	if (CharaState::Walk == m_State || CharaState::ReloadWalk == m_State)
	{
		XMStoreFloat3(&vMove, vecMove * WALKSPEED);
	}

	if (0 < m_nDashNum)
	{
		// ダッシュステート設定処理(待機状態か歩き状態から遷移可能)
		if ((CharaState::Wait == m_State || CharaState::Walk == m_State) &&
			CInput::GetKeyTrigger(VK_LSHIFT))
		{
			// 地上ダッシュ
			if (m_bGround) {
				m_fDashVol = DASHVOL;
				// ダッシュ回数減少
				--m_nDashNum;
				// ステート変更
				SetState(Dash);
				m_nNextAnimStack = ANIM_RUN;
			}
		}
	}
	if ((CharaState::Dash == m_State) &&
		CInput::GetKeyRelease(VK_LSHIFT))
	{
		// ステート変更
		SetState(CharaState::Wait);
		// ダッシュ移動量リセット
		m_fDashVol = 0;
	}
	
	// ステートがダッシュなら移動速度Up
	if (CharaState::Dash == m_State || CharaState::DashJump == m_State)
	{
		// 現在向いている方向のベクトルに現在のダッシュ移動量をかけ、加速度に加える
		XMStoreFloat3(&vMove, XMVector3Normalize(XMVectorSet(mW._31, mW._32, mW._33, 1.0f)) * m_fDashVol);

		if (CharaState::DashJump != m_State) {
			// ダッシュ移動量減衰(0.5以下なら0にしてステートを待機状態に)
			m_fDashVol *= DASHVOLDEL;
		}
		if (m_fDashVol < DASHVOLDELLIMIT)
		{
			m_fDashVol = 0;
			// ステート変更
			SetState(CharaState::Wait);
		}
	}
}

//=============================================================================
// 現在のステート描画
//=============================================================================
void CPlayer::DrawCurrentState(CharaState state)
{
	switch (m_State)
	{
	case CharaState::Wait:
		CDebugProc::Print("State : Wait\n");
		break;
	case CharaState::Walk:
		CDebugProc::Print("State : Walk\n");
		break;
	case CharaState::Dash:
		CDebugProc::Print("State : Dash\n");
		break;
	case CharaState::Jump:
		CDebugProc::Print("State : Jump\n");
		break;
	case CharaState::DashJump:
		CDebugProc::Print("State : DashJump\n");
		break;
	case CharaState::HandRangeAttack:
		CDebugProc::Print("State : Attack\n");
		break;
	case CharaState::LongRangeAttack:
		CDebugProc::Print("State : LongRangeAttack\n");
		break;
	case CharaState::Reload:
		CDebugProc::Print("State : Reload\n");
		break;
	case CharaState::ReloadWalk:
		CDebugProc::Print("State : ReloadWalk\n");
		break;
	case CharaState::Dying:
		CDebugProc::Print("State : Dying\n");
		break;
	default:
		break;
	}
}

//=============================================================================
// アニメーション更新処理まとめ
//=============================================================================
void CPlayer::AnimationUpdate()
{
	// TODO : アニメーション更新(全キャラアニメーションするので基底クラスで行うべき)
	// アニメーション番号更新
	switch (m_nAnimStack)
	{
	// 何も入力されていなければ待機アニメーションに遷移する
	case ANIM_IDLE:
	case ANIM_WALK:
	case ANIM_RUN:
	case ANIM_SLASH:
	case ANIM_HITBACK:
		if (m_nAnimStack != m_nNextAnimStack) {
			m_nAnimStack = m_nNextAnimStack;
			m_dAnimTime = 0.0;
		}
		break;
	case ANIM_JUMP:
		if (m_nAnimStack != m_nNextAnimStack) {
			if (ANIM_DYING == m_nNextAnimStack) {
				m_nAnimStack = m_nNextAnimStack;
				m_dAnimTime = 0.0;
			}
		}
		break;
	default:
		break;
	}

	switch (m_nAnimStack)
	{
	case ANIM_JUMP:
		// 接地していない
		if (!m_bGround)
		{
			// 現在のアニメーション時間が総アニメーション時間を超えていない
			if (m_dAnimTime <= GetAnimDuration(m_nAnimStack))
			{
				// 現在のアニメーション時間が空中ポーズフレーム以下
				if (m_dAnimTime < GetAnimDuration(ANIM_JUMP) / 2)
				{
					// アニメーション時間更新
					if (CharaState::Dash == m_State || CharaState::DashJump == m_State)
					{
						m_dAnimTime += ANIM_SPEED + ANIM_SPEED * (m_fDashVol / DASHVOL);
					}
					else
					{
						m_dAnimTime += ANIM_SPEED;
					}
				}
				else
				{
					// 効果中なら再びアニメーション時間更新
					if (m_Transform.GetCurAccel().y < 0)
					{
						m_dAnimTime += ANIM_SPEED;
					}
				}
			}
		}
		else// 接地している
		{
			// ステート設定
			SetState(Wait);
			// アニメーション番号を待機アニメーション番号に
			m_nAnimStack = ANIM_IDLE;
			m_dAnimTime = 0.0;
		}

		break;
	default:
		if (CharaState::Dash == m_State || CharaState::DashJump == m_State )
		{
			m_dAnimTime += ANIM_SPEED + ANIM_SPEED * (m_fDashVol / DASHVOL);
		}
		else
		{
			m_dAnimTime += ANIM_SPEED;
		}
		break;
	}
	// アニメーション時間更新
	// アニメーション開始してからの時間が総アニメーション時間を超えていると、デフォルトのアニメーションに遷移する
	double Double = GetAnimDuration(m_nAnimStack);
	if (m_dAnimTime >= GetAnimDuration(m_nAnimStack)) 
	{
		switch (m_nAnimStack) {
		case !ANIM_JUMP:
			// アニメーション時間をリセット
			m_dAnimTime = 0.0;
		case ANIM_SLASH:
		case ANIM_HITBACK:
			// ステート設定
			SetState(CharaState::Wait);
			// アニメーション番号を待機アニメーション番号に
			m_nAnimStack = ANIM_IDLE;
			// 再攻撃可能
			m_uqpAttackMethod->GetHandWeapon();
			break;
		case ANIM_DYING:
			// この後の爆発ですぐ不可視になる
			if (m_bVisible) {
				// エフェクシア
				StartDeath();

				// ｹﾞｰﾑｵｰﾊﾞｰ時の処理
				CSound::Play(SE_EXPLOSION);

			}
			break;
		}
	}
}