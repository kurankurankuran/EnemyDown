// 追尾ミサイル [Machinegun.cpp]
#include "Machinegun.h"
#include "Scene.h"
#include "Enemy.h"
#include "Explosion.h"
//#include "Game.h"
//#include "EffectManager.h"
#include "Player.h"
#include "EffekseerManager.h"
#include "MGManager.h"
//#include "Texture.h"
//#include "Sound.h"
#include "Text.h"

// 定数
namespace {
	const float MOVE_SPEED = /*10*/270.0f;
	const float g_fSpeedEnemy = 25.0f;
	const int   MAX_LIFE =/* 30000*/300;
	const int   g_nDamage = 50;
	const int   g_nDamageEnemy = 10;

	const int DAMAGE_CNT = 100;
	const XMFLOAT2 DAMAGE_SCALE = XMFLOAT2(8.0f, 16.0f);
	const XMFLOAT4 DAMAGE_COLOR = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
}



// コンストラクタ
CMachinegun::CMachinegun(CScene* pScene) : CWeapon(pScene)
{
	//SetTag(GOT_MACHINEGUN);
}

// デストラクタ
CMachinegun::~CMachinegun()
{
}

// 初期化
HRESULT CMachinegun::Init()
{
	HRESULT hr = S_OK;
	
	CGameObj::SetRadius(250.0f);
	CGameObj::SetBBox(XMFLOAT3(30.f, 30.f, 30.0f));
	m_pBBox->SetTrigger(true);
	m_vCenter = XMFLOAT3(0, 0, 0);

	// 速度設定
	if (m_Type == GOT_PLAYER)
	{
		m_fSpeed = MOVE_SPEED;
	}
	else
	{
		m_fSpeed = g_fSpeedEnemy;
	}
	// ライフ設定
	m_Status.nHP = MAX_LIFE;
	
	return hr;
}

void CMachinegun::Fin()
{
	CEffekseerManager::Stop(m_nHandle, true);
	CModel::Fin();
}

// 更新
void CMachinegun::Update()
{
	// 生存時間更新
	--m_Status.nHP;
	if (m_Status.nHP <= 0) {
		Fin();
		delete this;
		return;

	}



	// ワールド変換行列取得
	XMFLOAT4X4 mW = m_Transform.GetWorld();
	// モデル座標軸取得
	XMFLOAT3 vX = XMFLOAT3(mW._11, mW._12, mW._13);
	XMFLOAT3 vY = XMFLOAT3(mW._21, mW._22, mW._23);
	XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
	// 座標取得、クリア
	m_Transform.SetPos(XMFLOAT3(mW._41, mW._42, mW._43));
	mW._41 = mW._42 = mW._43 = 0.0f;
	m_Transform.SetVel(XMFLOAT3(0, 0, 0));

	// 前進
	m_Transform.SetVel(
		XMFLOAT3(
			m_Transform.GetVel().x + vZ.x * m_fSpeed,
			m_Transform.GetVel().y + vZ.y * m_fSpeed,
			m_Transform.GetVel().z + vZ.z * m_fSpeed
		));

	//**********当たり判定**********
	// 地形との当たり判定
	XMFLOAT3 X, N;
	if (m_pLand->Collision(
		this,
		XMFLOAT3(
			m_Transform.GetPos().x + m_Transform.GetVel().x,
			m_Transform.GetPos().y + m_Transform.GetVel().y + m_vBBox.y,
			m_Transform.GetPos().z + m_Transform.GetVel().z),// P0 レイの始点
		XMFLOAT3(
			m_Transform.GetPos().x + m_Transform.GetVel().x,
			m_Transform.GetPos().y + m_Transform.GetVel().y - m_vBBox.y,
			m_Transform.GetPos().z + m_Transform.GetVel().z),  // P1 レイの終点(少し地面にめり込むまでレイを伸ばしている)
		&X, &N))
	{

		CSound::Play(SE_DEFEND);
		int Handle = 0;
		if (GOT_ENEMY == m_Type) {
			Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
		}
		else if (GOT_PLAYER == m_Type)
		{
			Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
		}
		else
		{
			Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
		}
		CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());


		Fin();
		delete this;
		return;
	}




	// 座標更新
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
	m_Transform.SetWorld(mW);

	// エフェクシアに座標を反映
	CEffekseerManager::SetMatrix(m_nHandle, true, m_Transform.GetWorld());

	// ノードに登録
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);


	// 自分の登録ノード内にいるオブジェクトとの当たり判定
	std::vector<CGameObj*> ColObjVector = CGameObj::CollisionOBB();
	if (ColObjVector.size() > 0)
	{
		switch (m_Type) {
			// 自分がプレイヤーから発射された弾なら
		case GOT_PLAYER:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// 衝突相手のIDごとの処理
				switch (ColObjVector[i]->GetTag())
				{
					// 敵かボス
				case GOT_ENEMY:
				case GOT_BOSS:
				{
					// 相手が死んでない
					if (ColObjVector[i]->GetStatus().nHP <= 0)continue;

					// ライフ減らす処理
					int nDamage = ColObjVector[i]->GetStatus().nRES - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);
						// ヒットストップ
						ColObjVector[i]->SetHitStopTime(0.2f);
					}
					else
					{
						nDamage = 0;
					}

					// ダメージテキスト
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);

					// サウンド
					CSound::Play(SE_DEFEND);

					// エフェクト
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// 終了処理
					Fin();
					delete this;
					return;
				}
				// プレイヤーならスルー
				case GOT_PLAYER:

					break;
				// 他のオブジェクトなら自壊
				default:
				{
					// エフェクト
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// 終了処理
					Fin();
					delete this;
					return;
				}
				}

			}

			break;
			// 自分が敵かボスから発射された弾なら
		case GOT_BOSS:
		case GOT_ENEMY:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// 衝突相手がプレイヤーなら以下の処理
				if (ColObjVector[i]->GetTag() == GOT_PLAYER) {
					// 相手が死んでない
					if (ColObjVector[i]->GetStatus().nHP <= 0)continue;
					// ライフ減らす処理
					int nDamage = ColObjVector[i]->GetStatus().nRES - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);
						// ヒットストップ
						ColObjVector[i]->SetHitStopTime(0.2f);
					}
					else
					{
						nDamage = 0;
					}

					// ダメージテキスト
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);
					
					// サウンド
					CSound::Play(SE_PLAYER_DAMAGE);

					// エフェクト
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// 終了処理
					Fin();
					delete this;
					return;

				}
				// 親オブジェクト以外のオブジェクトなら自壊
				else if (ColObjVector[i] != m_pParent) {

					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// 終了処理
					Fin();
					delete this;
					return;
				}
			}
			break;
		}
	}



}

void CMachinegun::Draw()
{
	//if (m_bUnit)return;

	// 描画はエフェクシアで行う
}

// 発射
int CMachinegun::Fire(XMFLOAT4X4* pWorld, EGameObjTag Type, CModel* pModel)
{
	CScene* pScene = CScene::GetCurrent();
	if (!pScene)
		return -1;
	CMachinegun* pMachinegun = new CMachinegun(pScene);
	pMachinegun->GetTransform()->SetWorld(*pWorld);
	pMachinegun->Init();
	pMachinegun->m_pTarget = pModel;
	pMachinegun->m_Type = Type;
	pMachinegun->SetBSVisible((EBoundingShape)(pScene)->GetBound());

	return 1;
}

