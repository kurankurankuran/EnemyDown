// 剣攻撃 [SwordAttack.cpp]
#include "SwordAttack.h"
#include "Scene.h"
#include "Enemy.h"
#include "Explosion.h"
//#include "Game.h"
#include "Player.h"
#include "Sound.h"
#include "EffectManager.h"
#include "EffekseerManager.h"
#include "Text.h"
#include "DebugProc.h"
#include "TextureData.h"

// 定数
namespace {
	// プレイヤー
	//const int TIME_PLAYER = 15; // 持続時間
	const int DAMAGE_TO_ENEMY = 9999; // ダメージ
	const int INV_TIME_PLAYER = 30;// ヒットした相手に与える無敵時間
	
	// エネミー
	//const int TIME_ENEMY = 150;  // 持続時間
	const int ENEMY_DAMAGE_TO_PLAYER = 20;  // ダメージ
	const int INV_TIME_ENEMY = 10;// ヒットした相手に与える無敵時間

	// ボス
	//const int TIME_BOSS = 120;  // 持続時間
	const int BOSS_DAMAGE_TO_PLAYER = 20;  // ダメージ
	const int INV_TIME_BOSS = 10;// ヒットした相手に与える無敵時間

	// テキスト表示時間
	const int DAMAGE_CNT = 100;
	const XMFLOAT2 DAMAGE_SCALE = XMFLOAT2(8.0f, 16.0f);
	const XMFLOAT4 DAMAGE_COLOR = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	const int SPLIT = 40;
}



// コンストラクタ
CSwordAttack::CSwordAttack(CScene* pScene) : CWeapon(pScene)
{
	SetTag(GOT_GAMEOBJ);
}

// デストラクタ
CSwordAttack::~CSwordAttack()
{
}

// 初期化
HRESULT CSwordAttack::Init()
{
	// 半透明処理のためリストの最後尾に移動
	m_pScene->ToEnd(this);

	SetModel(MT_SPHERE);
	posArray = std::vector<PosBuffer>(SPLIT, PosBuffer());
	m_Mesh.SetCamera(m_pScene->GetCameraManager()->GetCamera());
	m_Mesh.SetLight(m_pScene->GetLight());
	m_Mesh.SetPimitiveType(PT_UNDEFINED);
	m_fLongScale = 0.0f;
	HRESULT hr = CModel::Init();
	m_pBBox->SetTrigger(true);
	return hr;
}

void CSwordAttack::Fin()
{
	CModel::Fin();
}

// 更新
void CSwordAttack::Update()
{
	// 親がヒットストップ中なら自分もストップ
	if (0 < m_pParent->GetHitStopTime())
	{
		return;
	}

	// 親がヒットバック状態になったらすぐに自分を消去する
	CModel* pParentModel = (CModel*)m_pParent;
	if (CharaState::HitBack == pParentModel->GetState())
	{
		Fin();
		delete this;
		return;
	}

	// 持続時間更新
	--m_Status.nHP;
	if (m_Status.nHP <= 0) {
		if (m_Status.nHP == 0)
		{
			Fin();
		}
		// 剣の軌跡を後ろから消していく処理
		if (0 < posArray.size())
		{
			posArray.pop_back();
			// 剣の軌跡メッシュ作成
			MakeTraces();
			return;
		}
		delete this;
		return;
	}
	if (!m_bVisible) {
		Fin();
		delete this;
		return;
	}

	
	// ワールドマトリクスに反映させていく(親→アニメ→ローカル)
	XMMATRIX mtxWorld, mtxScl, mtxRot, mtxTranslate;
	// ワールドマトリクスの初期化
	mtxWorld = XMMatrixIdentity();
	mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&m_pParent->GetTransform()->GetWorld()));
	
	// 剣をもつ手のモデルに位置を合わせる
	pParentModel->SetAnimIndex(pParentModel->GetAnimStack());
	pParentModel->SetAnimTime(pParentModel->GetAnimTime());
	XMFLOAT4X4 AnimGlobalMat = pParentModel->GetBoneGlobalMatrix(m_strBoneName);
	mtxWorld = XMLoadFloat4x4(&AnimGlobalMat) * mtxWorld;
	
	// 剣の当たり判定のローカル：位置回転大きさを設定
	XMMATRIX mtxLocalWorld;
	mtxLocalWorld = XMMatrixIdentity();
	// スケールを反映
	mtxScl = XMMatrixScaling(
		m_Transform.GetScale().x, 
		m_Transform.GetScale().y, 
		m_Transform.GetScale().z);
	mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxScl);
	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_Transform.GetAngle().x),
		XMConvertToRadians(m_Transform.GetAngle().y), 
		XMConvertToRadians(m_Transform.GetAngle().z));
	mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxRot);
	// 移動を反映
	mtxTranslate = XMMatrixTranslation(
		m_Transform.GetPos().x, 
		m_Transform.GetPos().y, 
		m_Transform.GetPos().z);
	mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxTranslate);

	// ワールドマトリックス設定
	mtxWorld = mtxLocalWorld * mtxWorld;
	XMStoreFloat4x4(&m_Transform.GetWorld(), mtxWorld);
	
	// 当たり判定を行う際m_vPosの値がワールド座標におけるものとして扱うが、剣に関してはローカルの値として扱っているので一時的にm_vPosの値を変更する
	XMFLOAT3 vPos = m_Transform.GetPos();
	m_Transform.SetPos(
		XMFLOAT3(
			m_Transform.GetWorld()._41,
			m_Transform.GetWorld()._42,
			m_Transform.GetWorld()._43
		));
	// 親の登録ノード内にいるオブジェクトとの当たり判定
	std::vector<CGameObj*> ColObjVector = CGameObj::CollisionOBB();

	if (ColObjVector.size() > 0)
	{
		switch (m_Type) {
			// 自分がプレイヤーから発射された弾なら
		case GOT_PLAYER:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// 相手が不可視でない
				if (!ColObjVector[i]->GetVisible())continue;
				// 相手が死んでない
				if (ColObjVector[i]->GetStatus().nHP <= 0)continue;
				// 相手が無敵時間中でない
				if (ColObjVector[i]->GetInvTime() > 0)continue;

				// 衝突相手のIDごとの処理
				switch (ColObjVector[i]->GetTag())
				{
					// 敵かボス
				case GOT_ENEMY:
				case GOT_BOSS:
				{
					// ライフ減らす処理
					int nDamage = ColObjVector[i]->GetStatus().nDEF - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);
						// ヒットストップ
						ColObjVector[i]->SetHitStopTime(1.0f);
						// ダメージモーション発生
						CModel* pModel = (CModel*)ColObjVector[i];
						pModel->StartHitMotion();
						// 無敵時間発生
						ColObjVector[i]->SetInvTime(INV_TIME_PLAYER);
						m_pParent->SetHitStopTime(0.5f);
					}
					else
					{
						nDamage = 0;
					}

					// ダメージテキスト
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);
					
					// エフェクト
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// サウンド
					CSound::Play(SE_SWORD_HIT);
					
					break;
				}
				// プレイヤーならスルー
				case GOT_PLAYER:

					break;
				// ID未設定オブジェクトならスルー
				default:
				{
					break;
				
				}
				}

			}

			break;
			// 自分が敵かボスから発射された弾なら
		case GOT_BOSS:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// 相手が可視の場合のみ当たり判定を行う
				if (!ColObjVector[i]->GetVisible())continue;
				// 相手がヒットストップ中でない
				//if (ColObjVector[i]->GetHitStopTime() > 0)continue;
				// 相手が死んでない
				if (ColObjVector[i]->GetStatus().nHP <= 0)continue;
				// 相手が無敵時間中でない
				if (ColObjVector[i]->GetInvTime() > 0)continue;

				// 衝突相手がプレイヤーなら以下の処理
				if (ColObjVector[i]->GetTag() == GOT_PLAYER) {

					// ライフ減らす処理
					int nDamage = ColObjVector[i]->GetStatus().nDEF - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);
						// ダメージモーション発生
						CModel* pModel = (CModel*)ColObjVector[i];
						pModel->StartHitMotion();

						// 無敵時間
						ColObjVector[i]->SetInvTime(INV_TIME_BOSS);
					}
					else
					{
						nDamage = 0;
					}

					// ダメージテキスト
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);

					// エフェクト
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());
					
					// サウンド
					CSound::Play(SE_PLAYER_DAMAGE);

					break;
				}
				// ID未設定オブジェクトならスルー
				else if (ColObjVector[i]->GetTag() != GOT_ENEMY && ColObjVector[i]->GetTag() != GOT_BOSS) {

					break;
				}
			}
			break;
		case GOT_ENEMY:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// 相手が可視の場合のみ当たり判定を行う
				if (!ColObjVector[i]->GetVisible())continue;
				// 相手がヒットストップ中でない
				//if (ColObjVector[i]->GetHitStopTime() > 0)continue;
				// 相手が死んでない
				if (ColObjVector[i]->GetStatus().nHP <= 0)continue;
				// 相手が無敵時間中でない
				if (ColObjVector[i]->GetInvTime() > 0)continue;

				// 衝突相手がプレイヤーなら以下の処理
				if (ColObjVector[i]->GetTag() == GOT_PLAYER) {

					// ライフ減らす処理
					int nDamage = ColObjVector[i]->GetStatus().nDEF - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);

						// ダメージモーション発生
						CModel* pModel = (CModel*)ColObjVector[i];
						pModel->StartHitMotion();

						// 無敵時間
						ColObjVector[i]->SetInvTime(INV_TIME_ENEMY);
					}
					else
					{
						nDamage = 0;
					}

					// ダメージテキスト
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);

					// エフェクト
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// サウンド
					CSound::Play(SE_PLAYER_DAMAGE);

					break;
				}
				// ID未設定オブジェクトならスルー
				else if (ColObjVector[i]->GetTag() != GOT_ENEMY && ColObjVector[i]->GetTag() != GOT_BOSS) {
					
					break;
				}
			}
			break;
		}

	
	}

	// 値を元に戻す
	m_Transform.SetPos(vPos);

	// ノードに登録
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

	// 剣の軌跡メッシュ作成
	MakeTraces();
}



void CSwordAttack::Draw()
{
	int nCurCullMode = GetCurCullMode();
	SetCullMode(CULLMODE_NONE);
	m_pScene->GetLight()->SetEnable(false);

	// 半透明部分描画
	SetBlendState(BS_ALPHABLEND);
	//SetZWrite(false);
	if (index.size() >= 4)
	{
		m_Mesh.SetTexture(CTextureData::GetTextureResource(m_TexName));
		m_Mesh.Draw();
	}
	//SetZWrite(true);
	SetBlendState(BS_NONE);

	m_pScene->GetLight()->SetEnable(true);
	SetCullMode(nCurCullMode);
}

// 発射
int CSwordAttack::Fire(XMFLOAT4X4* pWorld, EGameObjTag Type, CModel* pModel)
{
	/*CScene* pScene = CScene::GetCurrent();
	if (!pScene)
		return -1;
	CSwordAttack* pMissile = new CSwordAttack(pScene);
	pMissile->SetWorld(*pWorld);
	pMissile->Init();
	pMissile->m_pTarget = pModel;
	pMissile->m_Type = Type;

	pMissile->SetBSVisible((EBoundingShape)(pScene)->GetBound());*/



	return 1;
}

void CSwordAttack::CreateCurveVertex(std::vector<PosBuffer>& usedPosArray)
{
	if (usedPosArray.size() < 3 || SPLIT < 1) { return; }
	std::vector<PosBuffer> newPosArray;
	newPosArray.reserve(usedPosArray.size() + (usedPosArray.size() - 1) * SPLIT);
	const float amount = 1.0f / (SPLIT + 1);

	PosBuffer newPos;
	newPosArray.push_back(usedPosArray.front());
	for (size_t i = 0; i < usedPosArray.size() - 1; ++i)
	{
		float ratio = amount;
		//  CatMulに使う4つの点を作る（p0, p3がない時の処理も書く）
		XMVECTOR p0Head = i == 0 ? (XMLoadFloat3(&usedPosArray[1].headPos) + XMLoadFloat3(&usedPosArray[2].headPos)) * 0.5f : XMLoadFloat3(&usedPosArray[i - 1].headPos);
		XMVECTOR p1Head = XMLoadFloat3(&usedPosArray[i].headPos);
		XMVECTOR p2Head = XMLoadFloat3(&usedPosArray[i + 1].headPos);
		XMVECTOR p3Head = i == usedPosArray.size() - 2 ? (p0Head + p2Head) * 0.5f : XMLoadFloat3(&usedPosArray[i + 2].headPos);

		XMVECTOR p0Tail = i == 0 ? (XMLoadFloat3(&usedPosArray[1].tailPos) + XMLoadFloat3(&usedPosArray[2].tailPos)) * 0.5f : XMLoadFloat3(&usedPosArray[i - 1].tailPos);
		XMVECTOR p1Tail = XMLoadFloat3(&usedPosArray[i].tailPos);
		XMVECTOR p2Tail = XMLoadFloat3(&usedPosArray[i + 1].tailPos);
		XMVECTOR p3Tail = i == usedPosArray.size() - 2 ? (p0Tail + p2Tail) * 0.5f : XMLoadFloat3(&usedPosArray[i + 2].tailPos);

		for (size_t j = 0; j < static_cast<size_t>(SPLIT - 1); ++j)
		{
			newPos = PosBuffer();

			newPos.isUsed = true;
			XMStoreFloat3(&newPos.headPos, XMVectorCatmullRom(p0Head, p1Head, p2Head, p3Head, ratio));
			XMStoreFloat3(&newPos.tailPos, XMVectorCatmullRom(p0Tail, p1Tail, p2Tail, p3Tail, ratio));

			newPosArray.push_back(newPos);
			ratio += amount;
		}
		newPosArray.push_back(usedPosArray[i + 1]);
	}
	usedPosArray = newPosArray;
}

void CSwordAttack::MakeTraces()
{
	XMFLOAT4X4 mW = m_Transform.GetWorld();
	XMFLOAT3 vZ = XMFLOAT3(
		mW._31, 
		mW._32, 
		mW._33);
	XMStoreFloat3(&vZ, XMVector3Normalize(XMLoadFloat3(&vZ)));

	XMFLOAT3 vHead;
	vHead.x = mW._41 - vZ.x * m_fLongScale * m_vBBox.y;
	vHead.y = mW._42 - vZ.y * m_fLongScale * m_vBBox.y;
	vHead.z = mW._43 - vZ.z * m_fLongScale * m_vBBox.y;
	XMFLOAT3 vTail;
	vTail.x = mW._41 + vZ.x * m_fLongScale * m_vBBox.y;
	vTail.y = mW._42 + vZ.y * m_fLongScale * m_vBBox.y;
	vTail.z = mW._43 + vZ.z * m_fLongScale * m_vBBox.y;
	tempPos.headPos = vHead;
	tempPos.tailPos = vTail;
	tempPos.isUsed = true;
	//データを更新
	for (int i = posArray.size() - 1; i > 0; --i)
	{
		posArray[i] = posArray[i - 1];
	}
	if (posArray.empty())return;
	posArray.front() = tempPos;
	tempPos = PosBuffer();

	//曲線を作る
	std::vector<PosBuffer> usedPosArray;
	for (unsigned int i = 0; i < posArray.size(); ++i)
	{
		if (posArray[i].isUsed) {
			usedPosArray.push_back(posArray[i]);
		}
	}
	if (usedPosArray.empty())return;
	CreateCurveVertex(usedPosArray);

	//頂点データを更新する
	float amount = 1.0f / (usedPosArray.size() - 1);
	float v = 0;
	vertex.clear();
	index.clear();
	vertex.resize(usedPosArray.size() * 2);
	/*usedPosArray[0].isUsed = true;*/
	for (size_t i = 0, j = 0; i < vertex.size() && j < usedPosArray.size(); i += 2, ++j)
	{
		//if (!usedPosArray[j].isUsed)continue;
		vertex[i].vtx = usedPosArray[j].headPos;
		vertex[i].tex = XMFLOAT2(1.0f, v);
		vertex[i].nor = XMFLOAT3(mW._21, mW._22, mW._23);
		vertex[i].diffuse = XMFLOAT4(1, 1, 1, 0.7f);
		index.push_back(i);
		vertex[i + 1].vtx = usedPosArray[j].tailPos;
		vertex[i + 1].tex = XMFLOAT2(0.0f, v);
		vertex[i + 1].nor = XMFLOAT3(mW._21, mW._22, mW._23);
		vertex[i + 1].diffuse = XMFLOAT4(1, 1,	1, 0.1f);
		index.push_back(i + 1);

		v += amount;
	}
	if (4 <= index.size()) 
	{
		HRESULT hr;
		hr = m_Mesh.Init(&vertex[0], vertex.size(), &index[0], index.size());
	}
}

void CSwordAttack::SetTraceScale(XMFLOAT3 vScale)
{
	//m_Transform.SetScale(vScale);
	m_fLongScale = m_Transform.GetScale().x;
	if (m_fLongScale < m_Transform.GetScale().y)
	{
		m_fLongScale = m_Transform.GetScale().y;
	}
	if (m_fLongScale < m_Transform.GetScale().z)
	{
		m_fLongScale = m_Transform.GetScale().z;
	}
}