#include "SwordAttackManager.h"
#include "Model.h"
#include "Scene.h"
#include "Sound.h"
#include <DirectXMath.h>
#include "TextureData.h"


namespace {
	// 全モーション時間
	const int ALLMOTION_COUNT_PLAYER = 60 * 3;
	
	const XMFLOAT3 GENERATE_POS_PLAYER =   XMFLOAT3(-10.0f, 0.0f,  -90.0f);
	const XMFLOAT3 GENERATE_SCALE_PLAYER = XMFLOAT3(3.0f, 3.0f, 17.5f);
	const XMFLOAT3 GENERATE_ANGLE_PLAYER = XMFLOAT3(0.0f,   15.0f,   0.0f);
	const std::string BONE_NAME_PLAYER = std::string("Sword_joint");

	const XMFLOAT3 GENERATE_POS_MUTANT = XMFLOAT3(125.0f, 0.0f, -20.0f);
	const XMFLOAT3 GENERATE_SCALE_MUTANT = XMFLOAT3(15.0f, 3.0f, 3.0f);
	const XMFLOAT3 GENERATE_ANGLE_MUTANT = XMFLOAT3(0.0f, 40.0f,  0.0f);
	const std::string BONE_NAME_MUTANT = std::string("Mutant:LeftForeArm");

	const XMFLOAT3 GENERATE_POS_MARIA   = XMFLOAT3(-40.0f,10.0f, -70.0f);
	const XMFLOAT3 GENERATE_SCALE_MARIA = XMFLOAT3(  3.0f, 3.0f, 30.0f);
	const XMFLOAT3 GENERATE_ANGLE_MARIA = XMFLOAT3(  5.0f, 30.0f, 0.0f);
	const std::string BONE_NAME_MARIA = std::string("RightHand");
}

CSwordAttackManager::CSwordAttackManager(CScene* pScene) : CWeaponManager(pScene)
{
	
}

CSwordAttackManager::~CSwordAttackManager()
{

}

HRESULT CSwordAttackManager::Init(XMFLOAT4X4* pWorld)
{
	m_nDamage = 0;
	m_bVisible = false;
	m_bAttackflg = false;
	m_pParentWorld = pWorld;
	m_nTime = 0;
	m_nTimeCnt = 0;
	m_nStartTime = 0;

	m_vPos = XMFLOAT3(0, 0, 0);
	m_vAngle = XMFLOAT3(0, 0, 0);
	m_vScale = XMFLOAT3(0, 0, 0);

	
	return true;
}

void CSwordAttackManager::Fin()
{
	for (unsigned int i = 0; i < m_pWeapon.size(); ++i) {
		if (m_pWeapon[i]) {
			delete m_pWeapon[i];
			m_pWeapon[i] = nullptr;
		}
	}
	m_pWeapon.clear();
}

void CSwordAttackManager::Update()
{
	if (m_pParent) {
		m_bOldVisible = m_bVisible;
		m_bVisible = m_pParent->GetVisible();
		// 親が不可視なら自分も不可視になる
		if (m_bVisible != m_bOldVisible) {
			if (m_bVisible) {
				SetVisible(true);
			}
			else
			{
				SetVisible(false);
			}
		}

		// 親がヒットバック状態になったらすぐに攻撃を取り消す
		CModel* pParentModel = (CModel*)m_pParent;
		if (CharaState::HitBack == pParentModel->GetState())
		{
			m_bAttackflg = false;
			m_nTimeCnt = m_nTime;
			Create(m_eType);
		}

		if (m_bAttackflg)
		{
			m_nTimeCnt--;
			if (m_nTimeCnt < 0)
			{
				m_nTimeCnt = 0;
			}

			if (m_nTimeCnt == m_nStartTime)
			{
				// マガジン数減少
				--m_nMagazine;
				if (m_nMagazine <= 0)
				{
					m_nMagazine = 0;
					// リロード可能
					m_bCreate = true;
					//return 0;
				}
				
				// 弾生成
				CSwordAttack* pSwordAttack = new CSwordAttack(m_pScene);
			
				// 味方の弾か敵の弾か設定
				pSwordAttack->SetType(m_eType);
				// テクスチャ設定
				pSwordAttack->SetTexture(m_TexName);
				// 初期化
				pSwordAttack->Init();
				// 初期パラメータ設定(ボーン位置基準のローカルトランスフォーム)
				pSwordAttack->GetTransform()->SetAngle(m_vAngle);
				pSwordAttack->GetTransform()->SetScale(m_vScale);
				pSwordAttack->GetTransform()->SetPos(m_vPos);
				// 剣の軌跡の長さを設定
				pSwordAttack->SetTraceScale(m_vScale);
				// ダメージ設定
				pSwordAttack->SetDamage(m_nDamage);
				// 武器の持続時間
				pSwordAttack->SetMaxHP(m_nTimeCnt);
				pSwordAttack->SetHP(m_nTimeCnt);
				// 当たり判定可視
				pSwordAttack->SetBSVisible((EBoundingShape)(m_pScene)->GetBound());
				// 発生地点の基準となるボーンの名前
				std::string strBoneName = m_strBoneName;
				pSwordAttack->SetBoneName(strBoneName);

				// ワールドマトリクスに反映させていく
				XMMATRIX mtxWorld, mtxScl, mtxRot, mtxTranslate;
				// ワールドマトリクスの初期化
				mtxWorld = XMMatrixIdentity();
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(m_pParentWorld));

				// 生成位置を基準ボーンの位置に合わせる
				CModel* pParentModel = (CModel*)m_pParent;
				pParentModel->SetAnimTime(pParentModel->GetAnimTime());
				XMFLOAT4X4 AnimGlobalMat = pParentModel->GetBoneGlobalMatrix(strBoneName);
				mtxWorld = XMLoadFloat4x4(&AnimGlobalMat) * mtxWorld;
				// 親オブジェクトのポインタを渡す
				pSwordAttack->SetParentObj(m_pParent);

				// 剣の当たり判定のローカル：位置回転大きさを設定
				XMMATRIX mtxLocalWorld;
				mtxLocalWorld = XMMatrixIdentity();
				// スケールを反映
				mtxScl = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
				mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxScl);
				// 回転を反映
				mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_vAngle.x),
					XMConvertToRadians(m_vAngle.y), XMConvertToRadians(m_vAngle.z));
				mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxRot);
				// 移動を反映
				mtxTranslate = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);
				mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxTranslate);

				mtxWorld = mtxLocalWorld * mtxWorld;

				// ワールドマトリックス設定
				XMStoreFloat4x4(&pSwordAttack->GetTransform()->GetWorld(), mtxWorld);
				// ノードに登録
				Octree* pOctree = CScene::GetOctree();
				pOctree->EraseAndPushObjInNode(pSwordAttack->GetIndexWork(), pSwordAttack);

				CSound::Play(SE_SWORD_SLASH);
			}
			else if (m_nTimeCnt <= 0)
			{
				m_bAttackflg = false;
				m_nTimeCnt = m_nTime;
				Create(m_eType);
			}
		}

		

	}
}

int CSwordAttackManager::Fire(int nDamage, int nTotalTime,int nDelay)
{
	if (!m_bAttackflg)
	{
		m_bAttackflg = true;

		m_nDamage = nDamage;

		if (nTotalTime) 
		{
			m_nTime = nTotalTime;
			m_nTimeCnt = m_nTime;
		}
		if (nDelay)
		{
			m_nStartTime = m_nTime - nDelay;
		}
		return 1;
	}
	else {
		return 0;
	}
}


int CSwordAttackManager::Create(EGameObjTag Type, CModel* pEnemy)
{
	if (!m_pScene)
		return -1;

	if (!m_bCreate)
		return -1;
	// リロード不可能(弾を使い切ると可能に)
	m_bCreate = false;
	// 攻撃中フラグはfalse
	m_bAttackflg = false;
	// マガジン補充
	m_nMagazine = MAX_SWORDATTACK;
	// 敵味方判別
	m_eType = Type;

	
	switch (m_eType)
	{
	case GOT_PLAYER:
		m_vPos = GENERATE_POS_PLAYER;
		m_vAngle = GENERATE_ANGLE_PLAYER;
		m_vScale = GENERATE_SCALE_PLAYER;
		m_strBoneName = BONE_NAME_PLAYER.c_str();
		m_TexName = GAGE_WAKU;
		break;
	case GOT_ENEMY:
	
		m_vPos = GENERATE_POS_MUTANT;
		m_vAngle = GENERATE_ANGLE_MUTANT;
		m_vScale = GENERATE_SCALE_MUTANT;
		m_strBoneName = BONE_NAME_MUTANT.c_str();
		m_TexName = GAGE_HAIKEI;
		break;
	case GOT_BOSS:
		m_vPos = GENERATE_POS_MARIA;
		m_vAngle = GENERATE_ANGLE_MARIA;
		m_vScale = GENERATE_SCALE_MARIA;
		m_strBoneName = BONE_NAME_MARIA.c_str();
		m_TexName = GAGE_HAIKEI;
		break;

	default:
		m_vPos = GENERATE_POS_PLAYER;
		m_vAngle = GENERATE_ANGLE_PLAYER;
		m_vScale = GENERATE_SCALE_PLAYER;

		m_TexName = GAGE_HAIKEI;
		break;
	}
	
	
	return 1;
}

void CSwordAttackManager::SetVisible(bool bVisible)
{
	if (m_pParent)
	{
		for (unsigned int i = 0; i < m_pWeapon.size(); ++i)
		{
			if (m_pWeapon[i]) {
				m_pWeapon[i]->SetVisible(bVisible);
			}
		}
	}
}
