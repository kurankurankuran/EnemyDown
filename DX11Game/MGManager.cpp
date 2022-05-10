#include"MGManager.h"
#include"Input.h"
#include"Sound.h"
#include"Texture.h"
#include"Polygon.h"
#include"EffekseerManager.h"
//#include "Camera.h"
#include"CameraManager.h"
namespace {
	const int g_nReLoadCnt = 120;
	const int g_nChangeNum = 100;
	const float g_fFireRate = 0.05f * 60.0f;
	const float g_fMoveDelCoefficient = 0.25f;              // 光源が目的地まで滑らかに移動する用の減衰係数
	const XMFLOAT3 g_fGeneratePos(100.0f, 100.0f, 0.0f); // プレイヤー基準の光源相対座標
	const int g_nChargeRate = 15;                            // MP自動回復レート
}


//static TPartsInfo g_partsInit = 
//{
//  MODEL_MISSILE, MODEL_PLAYER_CANON, { 0.0f, 100.0f, 0.0f}, {0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f}, true
//};


CMGManager::CMGManager(CScene* pScene) : CWeaponManager(pScene)
{
	
}

CMGManager::~CMGManager()
{

}

HRESULT CMGManager::Init(XMFLOAT4X4* pWorld)
{
	// マガジン数
	m_nMagazine = MAX_MACHINEGUN;
	// 発射レート減少フラグ
	m_bDelFireRate = false;
	// 発射レート
	m_fFireRate = 0.0f;
	// 武器チェンジ可能時間調整用
	m_nChangeNum = g_nChangeNum;
	m_nChangeCnt = m_nChangeNum;
	// 親オブジェクトのワールドマトリクス
	m_pParentWorld = pWorld;
	// リロード可能時間調整用
	m_nReloadCnt = g_nReLoadCnt;
	// 地形との当たり判定用
	m_pLand = (CLand*)m_pScene->Find(GOT_LAND);


	switch (m_pParent->GetTag())
	{
	case GOT_PLAYER:
		// 発射源生成
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_PLAYER_EFK), true, TIMER_MACHINGUN_GENERATER_PLAYER_EFK);
		break;
	case GOT_ENEMY:
		// 発射源生成
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_ENEMY_EFK), true, TIMER_MACHINGUN_GENERATER_ENEMY_EFK);
		break;
	case GOT_BOSS:
		// 発射源生成
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_ENEMY_EFK), true, TIMER_MACHINGUN_GENERATER_ENEMY_EFK);
		break;
	default:
		break;
	} 
	
	CEffekseerManager::SetMatrix(m_nEffekseerHandle, true, *m_pParentWorld);
	// 光源位置
	m_vGeneratePos = XMFLOAT3(m_pParentWorld->_41, m_pParentWorld->_42, m_pParentWorld->_43);
	// MP自動回復レート
	m_nChargeCnt = 0;

	return true;
}

void CMGManager::Fin()
{
	CEffekseerManager::Stop(m_nEffekseerHandle, true);

	// 光源爆発
	switch (m_pParent->GetTag())
	{
	case GOT_PLAYER:
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_PLAYER_EXPLOSION_EFK), false, TIMER_MACHINGUN_GENERATER_PLAYER_EXPLOSION_EFK);
		break;
	case GOT_ENEMY:
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK), false, TIMER_MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK);
		break;
	case GOT_BOSS:
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK), false, TIMER_MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK);
		break;
	default:
		break;
	}
	XMFLOAT4X4 mW = *m_pParentWorld;
	mW._41 = m_vGeneratePos.x;
	mW._42 = m_vGeneratePos.y;
	mW._43 = m_vGeneratePos.z;
	CEffekseerManager::SetMatrix(m_nEffekseerHandle, false, mW);
}

void CMGManager::Update()
{


	if (m_pParent) {
	
		if (!m_bVisible)return;

		// 光源の位置更新
		XMFLOAT4X4 mW = *m_pParentWorld;
		XMVECTOR vecX = XMVectorSet(mW._11, mW._12, mW._13, 1.0f);
		XMVECTOR vecY = XMVectorSet(mW._21, mW._22, mW._23, 1.0f);
		XMVECTOR vecZ = XMVectorSet(mW._31, mW._32, mW._33, 1.0f);
		XMVECTOR vecPos = XMVectorSet(mW._41, mW._42, mW._43, 1.0f);
		// 目的地点
		XMVECTOR vecPurPosPos = vecPos + vecX * g_fGeneratePos.x + vecY * g_fGeneratePos.y + vecZ * g_fGeneratePos.z;
		// 現在位置更新
		if (m_pScene->GetCameraManager()->GetMode() !=CCameraManager::ECameraMode::FPS_CAMERAMODE && m_eType == GOT_PLAYER)
		{
			XMStoreFloat3(&m_vGeneratePos, XMLoadFloat3(&m_vGeneratePos) + (vecPurPosPos - XMLoadFloat3(&m_vGeneratePos)) * g_fMoveDelCoefficient);
		}
		else
		{
			XMStoreFloat3(&m_vGeneratePos, XMLoadFloat3(&m_vGeneratePos) + (vecPurPosPos - XMLoadFloat3(&m_vGeneratePos)));
		}
		mW._41 = m_vGeneratePos.x;
		mW._42 = m_vGeneratePos.y;
		mW._43 = m_vGeneratePos.z;
		CEffekseerManager::SetMatrix(m_nEffekseerHandle, true, mW);

		// 武器変更可能時間カウント更新
		if (m_nChangeCnt > 0) {
			--m_nChangeCnt;
		}
		// 弾の発射レート更新
		if (m_bDelFireRate)
		{
			--m_fFireRate;
			m_bDelFireRate = false;
			if (m_fFireRate < 0.0f)
			{
				m_fFireRate = g_fFireRate;
			}
		}
		else
		{
			m_fFireRate = 0.0f;
		}

		// MP回復
		if (m_nMagazine < MAX_MACHINEGUN)
		{
			m_nChargeCnt++;
			if (g_nChargeRate < m_nChargeCnt)
			{
				m_nMagazine++;
				m_nChargeCnt = 0;
			}
		}
		else 
		{
			m_nChargeCnt = 0;
		}
	}



	
}

int CMGManager::Fire(int nDamage,int nTotalTime, int nDelay)
{
	// 発射レート減少フラグ更新
	m_bDelFireRate = true;
	// 発射レート
	if (m_fFireRate > 0.0f)return 0;
	// 発射レートリセット
	m_fFireRate = g_fFireRate;      
	// マガジン数減少
	--m_nMagazine;
	if (m_nMagazine <= 0) 
	{
		m_nMagazine = 0;
		// リロード可能
		m_bCreate = true;
		return 0; 
	}

	// 弾生成
	CMachinegun* pMachinegun = new CMachinegun(m_pScene);
	// 味方の弾か敵の弾か設定
	pMachinegun->SetType(m_eType);
	// 地形モデルのポインタセット（当たり判定用）
	if(m_pLand)
		pMachinegun->SetLand(m_pLand);

	// 初期化
	pMachinegun->Init();
	// ダメージ設定
	pMachinegun->SetDamage(nDamage);
	// 親設定
	pMachinegun->SetParent(m_pParent);
	// 初期パラメータ設定(プレイヤーの体のどの位置から発射するか決める際はここを変更する)
	XMFLOAT3 vAngle, vScale, vPos;
	vAngle = XMFLOAT3(0, 0, 0);
	vScale = XMFLOAT3(1, 1, 1);
	vPos = XMFLOAT3(0, 0, 0);
	pMachinegun->GetTransform()->SetAngle(vAngle);
	pMachinegun->GetTransform()->SetScale(vScale);
	pMachinegun->GetTransform()->SetPos(vPos);
	pMachinegun->SetBSVisible((EBoundingShape)(m_pScene)->GetBound());

	// ワールドマトリクスに反映させていく
	XMMATRIX mtxWorld, mtxScl, mtxRot, mtxTranslate;
	// ワールドマトリクスの初期化
	mtxWorld = XMMatrixIdentity();
	// スケールを反映
	mtxScl = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(vAngle.x),
		XMConvertToRadians(vAngle.y), XMConvertToRadians(vAngle.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	// 移動を反映
	mtxTranslate = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	
	switch (m_eType)
	{
	case GOT_PLAYER:
	{
		// カメラオブジェクト取得
		CCamera* pCamera = m_pScene->GetCameraManager()->GetCamera();
		if (!pCamera)return 0;
		// カメラアングル取得
		XMFLOAT3 vCameraAngle = pCamera->GetAngle();
		// カメラワールド座標
		XMFLOAT4X4 mtxCameraWorld = pCamera->GetWorldMatrix();

		// カメラモードによって撃ち出す方向を変える
		if (m_pScene->GetCameraManager()->GetMode() != CCameraManager::ECameraMode::FPS_CAMERAMODE) {
			// 親のワールド行列にかけ合わせる
			mtxCameraWorld._41 = m_vGeneratePos.x;
			mtxCameraWorld._42 = m_vGeneratePos.y;
			mtxCameraWorld._43 = m_vGeneratePos.z;
			mtxWorld *= XMLoadFloat4x4(&mtxCameraWorld);

		}
		else {
			
			// ジェネレーター座標からカメラの注視点方向に飛ぶ
			XMFLOAT4X4 mPlayerWorld = m_pParent->GetTransform()->GetWorld();
			mPlayerWorld._41 = mPlayerWorld._42 = mPlayerWorld._43 = 0.0f;


			// ワールド変換行列に反映
			mPlayerWorld._41 = m_vGeneratePos.x;
			mPlayerWorld._42 = m_vGeneratePos.y;
			mPlayerWorld._43 = m_vGeneratePos.z;

			// 進行方向設定
			mPlayerWorld._31 = mtxCameraWorld._31;
			mPlayerWorld._32 = mtxCameraWorld._32;
			mPlayerWorld._33 = mtxCameraWorld._33;

			// 親のワールド行列にかけ合わせる
			mtxWorld *= XMLoadFloat4x4(&mPlayerWorld);
		}

		// ワールドマトリックス設定
		XMStoreFloat4x4(&pMachinegun->GetTransform()->GetWorld(), mtxWorld);
		// ノードに登録
		Octree* pOctree = CScene::GetOctree();
		pOctree->EraseAndPushObjInNode(pMachinegun->GetIndexWork(), pMachinegun);

		// エフェクシア処理
		int nHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_PLAYER_EFK), true, TIMER_MACHINGUN_PLAYER_EFK);
		CEffekseerManager::SetMatrix(nHandle, true, pMachinegun->GetTransform()->GetWorld());
		pMachinegun->SetEfkHandle(nHandle);

		// 発射音
		CSound::Play(SE_SHOT);
	}
		break;
	default:
	{
		// ジェネレーター座標から親の向いている方向に飛ぶ
		XMFLOAT4X4 mEnemyWorld = m_pParent->GetTransform()->GetWorld();
		mEnemyWorld._41 = m_vGeneratePos.x;
		mEnemyWorld._42 = m_vGeneratePos.y;
		mEnemyWorld._43 = m_vGeneratePos.z;

		// 親オブジェクトのワールドマトリクスを反映
		mtxWorld *= XMLoadFloat4x4(&mEnemyWorld);
		// ワールドマトリックス設定
		XMStoreFloat4x4(&pMachinegun->GetTransform()->GetWorld(), mtxWorld);

		// ノードに登録
		Octree* pOctree = CScene::GetOctree();
		pOctree->EraseAndPushObjInNode(pMachinegun->GetIndexWork(), pMachinegun);
		// エフェクシア処理
		int nHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_ENEMY_EFK), true, TIMER_MACHINGUN_ENEMY_EFK);
		CEffekseerManager::SetMatrix(nHandle, true, pMachinegun->GetTransform()->GetWorld());
		pMachinegun->SetEfkHandle(nHandle);

		// 発射音
		CSound::Play(SE_SHOT);
	}
		break;
	}

	return 1;
}

int CMGManager::Create( EGameObjTag Type, CModel* pEnemy)
{
	if (!m_pScene)
		return -1;

	if (!m_bCreate)
		return -1;
	// リロード不可能(弾を使い切ると可能に)
	m_bCreate = false;
	// マガジン補充
	m_nMagazine = MAX_MACHINEGUN;
	// 敵味方判別
	m_eType = Type;


	return 1;
}

