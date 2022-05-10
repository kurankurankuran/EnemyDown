//=============================================================================
//
// モデル クラス実装 [Model.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "Model.h"
#include "Scene.h"
#include "Input.h"
#include "DebugProc.h"
#include "EffekseerManager.h"


// 3Dモデル ファイル名
namespace {
	LPCSTR g_pszModelPath[MAX_MODEL] = {
		"data/model/Player/Paladin.fbx",// プレイヤー   
		"data/model/Mutant/Mutant.fbx",            // ミュータントモデル
		"data/model/Wizard/Wizard.fbx",            // 魔法使いモデル
		"data/model/Maria/Maria.fbx",              // マリアモデル
		"data/model/sphere.fbx",                   // 敵球体（停止）モデル
		"data/model/sky.fbx",                      // スカイ ドーム
		"data/model/Terrian/Terrian.fbx",        // 地面モデル
		"data/model/cube.fbx",                     // 触れたら死ぬキューブ
		"data/model/WallRock/WallRock.fbx",        // 岩モデル
		"data/model/Cliff/Sedimentary.fbx", // 崖モデル
	};
}

// 静的メンバ
CAssimpModel* CModel::m_pModels[MAX_MODEL] = { nullptr };

// コンストラクタ
CModel::CModel(CScene* pScene) : CGameObj(pScene)
{
	m_pModel = nullptr;
	m_nDethEffectTimer = 0;
	m_ModelType = MAX_MODEL;
	m_bGround = false;
	m_pLand = nullptr;
}

// デストラクタ
CModel::~CModel()
{
	/*if (m_pModel) {
		m_pModel->Release();
		delete m_pModel;
		m_pModel = nullptr;
	}*/
}

// 全モデル読込
HRESULT CModel::LoadAll()
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// Assimp用シェーダ初期化
	if (!CAssimpModel::InitShader(pDevice))
		return E_FAIL;

	// 全モデル読込
	HRESULT hr = S_OK;
	for (int i = 0; i < MAX_MODEL; ++i) {
		SAFE_DELETE(m_pModels[i]);
		
		
		m_pModels[i] = new CAssimpModel();
		if (!m_pModels[i]->Load(pDevice, pDC, g_pszModelPath[i])) {
			hr = E_FAIL;
			break;
		}
	}
	return hr;
}

// 全モデル解放
void CModel::ReleaseAll()
{
	// 全モデル解放
	for (int i = 0; i < MAX_MODEL; ++i) {
		if (m_pModels[i]) {
			m_pModels[i]->Release();
			delete m_pModels[i];
			m_pModels[i] = nullptr;
		}
	}

	// Assimp用シェーダ終了処理
	CAssimpModel::UninitShader();
}

// 最初の初期化
HRESULT CModel::Awake()
{
	return CGameObj::Awake();
}

// 初期化
HRESULT CModel::Init()
{
	// 地形モデルポインタ
	m_pLand = (CLand*)m_pScene->Find(GOT_LAND);

	HRESULT hr = CGameObj::Init();
	if (FAILED(hr))
		return hr;

	return hr;
}

// 終了処理
void CModel::Fin()
{
	CGameObj::Fin();
}

// 更新
void CModel::Update()
{
	CGameObj::Update();
}

// 描画
void CModel::Draw()
{
	if (!m_bVisible) return;
	if (m_pModel) {
	
		ID3D11DeviceContext* pDC = GetDeviceContext();
		m_pModel->Draw(pDC, m_Transform.GetWorld(), eOpacityOnly);
	}
}

// 半透明部分描画
void CModel::DrawAlpha()
{
	if (m_bVisible) {
		if (m_pModel) {
			ID3D11DeviceContext* pDC = GetDeviceContext();
			m_pModel->Draw(pDC, m_Transform.GetWorld(), eTransparentOnly);
		}
		CGameObj::DrawAlpha();
	}

}

// モデル設定
void CModel::SetModel(EModelType model)
{
	if (model < 0 || model >= MAX_MODEL) {
		m_pModel = nullptr;
		m_ModelType = MAX_MODEL;
	}
	else {
		m_pModel = m_pModels[model];
		m_ModelType = model;
	}
	
	// カメラと光源を設定
	if (m_pModel && m_pScene) {
		m_pModel->SetCamera(m_pScene->GetCameraManager()->GetCamera());
		m_pModel->SetLight(m_pScene->GetLight());
	}
	// 境界情報取得
	if (m_pModel) {
		SetCenter(m_pModel->GetCenter());
		SetBBox(m_pModel->GetBBox()); 
		SetRadius(m_pModel->GetRadius());
	}
	else {
		SetCenter(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetBBox(XMFLOAT3(0.5f, 0.5f, 0.5f));
		SetRadius(0.5f);
	}
}



// Assimpモデル取得
CAssimpModel* CModel::GetAssimp(EModelType model)
{
	if (model < 0 || model >= MAX_MODEL)
		return nullptr;
	return m_pModels[model];
}

// ボーンの現在のワールドマトリクスを取得
XMFLOAT4X4 CModel::GetBoneGlobalMatrix(std::string BoneName)
{
	return m_pModel->GetBoneGlobalMatrix(BoneName.c_str());
}

void CModel::StartHitMotion()
{
	SetState(CharaState::HitBack);
	m_dAnimTime = 0;
}

void CModel::StartDeath()// 死亡演出開始(GameObjIdからエフェクトを自動選択)
{
	// 演出中不可視
	m_bVisible = false;
	int nEfkHandle = GetTag();

	switch (nEfkHandle)
	{
	case		GOT_ENEMY:         // 敵死亡時の爆発
		m_nDethEffectTimer = 100;
		nEfkHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(DEATH_EXPLOSION_ENEMY_EFK), false, TIMER_DEATH_EXPLOSION_ENEMY_EFK);
		// 死亡音
		CSound::Play(SE_EXPLOSION);
		break;
	case		GOT_BOSS:         // ボス死亡時の爆発
		m_nDethEffectTimer = 100;
		nEfkHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(DEATH_EXPLOSION_ENEMY_EFK), false, TIMER_DEATH_EXPLOSION_ENEMY_EFK);

		break;
	case		GOT_PLAYER:         // プレイヤー死亡時の爆発
		m_nDethEffectTimer = 120;
		nEfkHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(DEATH_EXPLOSION_PLAYER_EFK), false, TIMER_DEATH_EXPLOSION_PLAYER_EFK);

		break;
	default:
		// Tagに対応したEfk無し
		m_nDethEffectTimer = 0;
		return;
	}

	CEffekseerManager::SetMatrix(nEfkHandle, false, m_Transform.GetWorld());
}



// 衝突した相手のOBBの外側に押し出す
std::vector<CGameObj*> CModel::CollisionOBBPlane(float* fAngle,XMFLOAT3* newPos)            // 衝突した相手のOBBの外側に押し出す
{
	std::vector<CGameObj*> vectorColObj = CGameObj::CollisionOBB();// 衝突したオブジェクト

	if (0 < vectorColObj.size())
	{
		XMFLOAT3 vForce = XMFLOAT3(0, 0, 0); // 押し出しベクトル
		float fNormalAngle = XM_PI;
		for (unsigned int i = 0; i < vectorColObj.size(); ++i)
		{
			// 相手のOBB
			CBox* pColBox = vectorColObj[i]->GetBBOXInstance();
			// OBB取得できたか
			if (!pColBox)continue;
			// トリガー判定か
			if (pColBox->GetTrigger())continue;

			// 相手のOBB頂点情報
			VERTEX_3D* pColVertex = pColBox->GetVertex();
			// 相手のOBBインデックス情報
			UINT* pColIndex = pColBox->GetIndex();
			// 相手のOBBインデックス数
			UINT nColIndexNum = pColBox->GetIndexNum();
			// 相手のワールド行列
			XMMATRIX mColWorld = XMLoadFloat4x4(&vectorColObj[i]->GetTransform()->GetWorld());
			// 相手のメッシュの座標変換用ワールド行列
			XMMATRIX meshWorld = XMMatrixTranslationFromVector(XMLoadFloat3(&vectorColObj[i]->GetCenter()));
			meshWorld *= mColWorld;


			// 自分の中心から相手の中心へのベクトルを求める
			// 相手の中心
			XMFLOAT3 vColObjCenter; 
			XMStoreFloat3(&vColObjCenter, XMVector3TransformCoord(XMLoadFloat3(&vectorColObj[i]->GetCenter()), mColWorld));
			// 自分の中心
			XMFLOAT3 vCenter; 
			XMStoreFloat3(&vCenter, XMVector3TransformCoord(XMLoadFloat3(&m_vCenter), XMLoadFloat4x4(&m_Transform.GetWorld())));
			// 中心間ベクトル
			//XMFLOAT3 vCenterToCenter = XMFLOAT3(vCenter.x - vColObjCenter.x, vCenter.y - vColObjCenter.y, vCenter.z - vColObjCenter.z);
			XMFLOAT3 vCenterToCenter = XMFLOAT3(vColObjCenter.x - vCenter.x, vColObjCenter.y - vCenter.y, vColObjCenter.z - vCenter.z);

			// ********中心間ベクトルについての当たり判定********
			for (unsigned int i = 0; i < nColIndexNum; i += 3) {
				// 相手のOBBのメッシュの頂点座標
				XMFLOAT3 V0;
				XMFLOAT3 V1;
				XMFLOAT3 V2;
				XMStoreFloat3(&V0, XMVector3TransformCoord(XMLoadFloat3(&pColVertex[pColIndex[i + 0]].vtx), meshWorld));
				XMStoreFloat3(&V1, XMVector3TransformCoord(XMLoadFloat3(&pColVertex[pColIndex[i + 1]].vtx), meshWorld));
				XMStoreFloat3(&V2, XMVector3TransformCoord(XMLoadFloat3(&pColVertex[pColIndex[i + 2]].vtx), meshWorld));

				// 辺に対応するベクトル
				XMFLOAT3 V0V1(V1.x - V0.x, V1.y - V0.y, V1.z - V0.z);
				XMFLOAT3 V1V2(V2.x - V1.x, V2.y - V1.y, V2.z - V1.z);
				XMFLOAT3 V2V0(V0.x - V2.x, V0.y - V2.y, V0.z - V2.z);

				// 三角形(=平面)の法線ベクトルを求める
				XMVECTOR N = XMVector3Normalize(
					XMVector3Cross(XMLoadFloat3(&V0V1),
						XMLoadFloat3(&V1V2)));

				// tの式(t=(N・(V0-P0))/(N・W))の分母を求める
				float base;
				XMStoreFloat(&base, XMVector3Dot(N,
					XMLoadFloat3(&vCenterToCenter)));
				if (base == 0.0f)
					continue;	// 面とレイが平行

				// 分子を求める
				XMFLOAT3 P0V0(
					V0.x - vCenter.x,
					V0.y - vCenter.y,
					V0.z - vCenter.z);
				float dot;
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMLoadFloat3(&P0V0)));
				// tから交点を求める
				float t = dot / base;

				if (t <= 0.0f)
					continue;	// 交点が始点の後ろ
				if (t > 1.0f)
					continue;   // 交点が終点よりも奥


				XMFLOAT3 X(
					vCenter.x + t * vCenterToCenter.x,
					vCenter.y + t * vCenterToCenter.y,
					vCenter.z + t * vCenterToCenter.z);

				// 内外判定
				XMFLOAT3 V0X(X.x - V0.x, X.y - V0.y, X.z - V0.z);
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMVector3Cross(XMLoadFloat3(&V0V1),
						XMLoadFloat3(&V0X))));
				if (dot < 0.0f)
					continue;
				XMFLOAT3 V1X(X.x - V1.x, X.y - V1.y, X.z - V1.z);
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMVector3Cross(XMLoadFloat3(&V1V2),
						XMLoadFloat3(&V1X))));
				if (dot < 0.0f)
					continue;
				XMFLOAT3 V2X(X.x - V2.x, X.y - V2.y, X.z - V2.z);
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMVector3Cross(XMLoadFloat3(&V2V0),
						XMLoadFloat3(&V2X))));
				if (dot < 0.0f)
					continue;


				// 当たり

				X;// 衝突点
				N;// 衝突ポリゴンの法線（単位化済み）
				// Y軸上方向により向いている法線を返す
				if (fAngle)
				{
					float _fNormalAngle;
					XMStoreFloat(&_fNormalAngle, XMVector3AngleBetweenNormals(XMVectorSet(0, 1, 0, 1), N));
					if (_fNormalAngle < fNormalAngle)
					{
						fNormalAngle = _fNormalAngle;
						*fAngle = fNormalAngle;
					}
				}
				/*  押し出しベクトルの計算
					①衝突したメッシュのいずれかの頂点から自分の中心へのベクトルと
					メッシュの法線の内積を計算し、押し出しベクトルの計算方法を決定する
					②押し出しベクトルを計算後、次の衝突オブジェクトに進む
				*/
				XMFLOAT3 _vForce = XMFLOAT3(0, 0, 0);
				float r;// 法線に投影したOBBの影の半分の長さ
				float s;// 衝突メッシュの頂点から自分の中心までのベクトルの影を法線に落とした際の長さ
				float fLen[3];
				XMFLOAT4X4 mWorld = m_Transform.GetWorld();
				XMStoreFloat(&fLen[0], XMVector3Dot(XMVectorSet(mWorld._11, mWorld._12, mWorld._13, 1.0f)*m_vBBox.x, N));
				XMStoreFloat(&fLen[1], XMVector3Dot(XMVectorSet(mWorld._21, mWorld._22, mWorld._23, 1.0f)*m_vBBox.y, N));
				XMStoreFloat(&fLen[2], XMVector3Dot(XMVectorSet(mWorld._31, mWorld._32, mWorld._33, 1.0f)*m_vBBox.z, N));
				r = fabsf(fLen[0]) + fabsf(fLen[1]) + fabsf(fLen[2]);
				// 衝突メッシュの頂点から自分の中心方向のベクトル
				XMVECTOR vecVertexToCenter = XMVectorSet(vCenter.x - V0.x, vCenter.y - V0.y, vCenter.z - V0.z, 1.0f);
				XMStoreFloat(&s, XMVector3Dot(vecVertexToCenter, N));
				s = fabsf(s);

				float fDot;
				XMStoreFloat(&fDot, XMVector3Dot(vecVertexToCenter, N));
				if (0 <= fDot)
				{
					XMStoreFloat3(&_vForce, (r - s) * N);
				}
				else
				{
					XMStoreFloat3(&_vForce, (r + s) * N);
				}

				vForce.x += _vForce.x;
				vForce.y += _vForce.y;
				vForce.z += _vForce.z;
			}
		}

		if (newPos)
		{
			newPos->x = m_Transform.GetPos().x + vForce.x;
			newPos->y = m_Transform.GetPos().y + vForce.y;
			newPos->z = m_Transform.GetPos().z + vForce.z;
		}
	}



	return vectorColObj;
}

bool CModel::CollisionOBB(CGameObj* pOther)
{


	if (CGameObj::CollisionOBB(pOther))// プレイヤーの登録ノードに登録されているオブジェクトとのみ当たり判定を行う新たな衝突関数をGameObjで定義する必要あるかも
	{
		// 相手の頂点情報
		// 相手の頂点数
		TAssimpVertex* pVertex = nullptr;
		UINT*          pIndex = nullptr;
		UINT           nVertexNum = 0;
		UINT           nIndexNum = 0;
		// 頂点配列、インデックス配列を取得しておく
		CModel* pModel = (CModel*)pOther;
		CAssimpModel* pAssimp = GetAssimp(pModel->GetModelType());
		pAssimp->GetVertexCount(&nVertexNum, &nIndexNum);
		pVertex = new TAssimpVertex[nVertexNum];
		pIndex = new UINT[nIndexNum];
		pAssimp->GetVertex(pVertex, pIndex);

		// 自分の上下左右前後にＯＢＢ面まで伸びる線分を作り、相手のワールド行列を相手のモデルの頂点にかけ、線分と相手の頂点から衝突ポリゴンの法線を導く
		// 自分のＯＢＢの中心点
		XMFLOAT4X4 mWorld = m_Transform.GetWorld();
		XMMATRIX mtxWorld = XMLoadFloat4x4(&mWorld);
		XMFLOAT3 vCenter;
		XMVECTOR vecCenter; // 計算用
		vecCenter = XMVector3TransformCoord(XMVectorSet(m_vCenter.x, m_vCenter.y, m_vCenter.z, 1.0f), mtxWorld);
		XMStoreFloat3(&vCenter, vecCenter); // OBB中心点保存
		// 自分のローカル座標軸取得
		XMFLOAT3 vX = XMFLOAT3(mWorld._11, mWorld._12, mWorld._13);
		XMFLOAT3 vY = XMFLOAT3(mWorld._21, mWorld._22, mWorld._23);
		XMFLOAT3 vZ = XMFLOAT3(mWorld._31, mWorld._32, mWorld._33);
		// 中心から面の位置までのベクトル
		XMVECTOR vecX = XMVector3Normalize(XMVectorSet(vX.x, vX.y, vX.z, 1.0f)) * m_vBBox.x;
		XMVECTOR vecY = XMVector3Normalize(XMVectorSet(vY.x, vY.y, vY.z, 1.0f)) * m_vBBox.y;
		XMVECTOR vecZ = XMVector3Normalize(XMVectorSet(vZ.x, vZ.y, vZ.z, 1.0f)) * m_vBBox.z;
		
		// 線分作成
		XMFLOAT3 vP0, vP1;
		XMStoreFloat3(&vP0, vecCenter);
		vP1 = XMFLOAT3(
		    m_Transform.GetPos().x + m_Transform.GetVel().x, 
			m_Transform.GetPos().y + m_Transform.GetVel().y, 
			m_Transform.GetPos().z + m_Transform.GetVel().z);
		
		// レイ（線分）
		XMFLOAT3 vW(vP1.x - vP0.x, vP1.y - vP0.y, vP1.z - vP0.z);

		// 相手の全ての三角形について
		for (UINT i = 0; i < nIndexNum; i += 3) {
			// 三角形の頂点座標
			XMFLOAT3& V0 = pVertex[pIndex[i + 0]].vPos;
			XMFLOAT3& V1 = pVertex[pIndex[i + 1]].vPos;
			XMFLOAT3& V2 = pVertex[pIndex[i + 2]].vPos;
			// 辺に対応するベクトル
			XMFLOAT3 V0V1(V1.x - V0.x, V1.y - V0.y, V1.z - V0.z);
			XMFLOAT3 V1V2(V2.x - V1.x, V2.y - V1.y, V2.z - V1.z);
			XMFLOAT3 V2V0(V0.x - V2.x, V0.y - V2.y, V0.z - V2.z);

			// 三角形(=平面)の法線ベクトルを求める
			XMVECTOR N = XMVector3Normalize(
				XMVector3Cross(XMLoadFloat3(&V0V1),
					XMLoadFloat3(&V1V2)));

			// tの式(t=(N・(V0-P0))/(N・W))の分母を求める
			float base;
			XMStoreFloat(&base, XMVector3Dot(N,
				XMLoadFloat3(&vW)));
			if (base == 0.0f)
				continue;	// 面とレイが平行

			// 分子を求める
			XMFLOAT3 P0V0(V0.x - vP0.x, V0.y - vP0.y, V0.z - vP0.z);
			float dot;
			XMStoreFloat(&dot, XMVector3Dot(N,
				XMLoadFloat3(&P0V0)));
			// tから交点を求める
			float t = dot / base;

			if (t < 0.0f)
				continue;	// 交点が始点の後ろ
			if (t > 1.0f)
				continue;   // 交点が終点よりも奥


			XMFLOAT3 X(vP0.x + t * vW.x,
				vP0.y + t * vW.y, vP0.z + t * vW.z);

			// 内外判定
			XMFLOAT3 V0X(X.x - V0.x, X.y - V0.y, X.z - V0.z);
			XMStoreFloat(&dot, XMVector3Dot(N,
				XMVector3Cross(XMLoadFloat3(&V0V1),
					XMLoadFloat3(&V0X))));
			if (dot < 0.0f)
				continue;
			XMFLOAT3 V1X(X.x - V1.x, X.y - V1.y, X.z - V1.z);
			XMStoreFloat(&dot, XMVector3Dot(N,
				XMVector3Cross(XMLoadFloat3(&V1V2),
					XMLoadFloat3(&V1X))));
			if (dot < 0.0f)
				continue;
			XMFLOAT3 V2X(X.x - V2.x, X.y - V2.y, X.z - V2.z);
			XMStoreFloat(&dot, XMVector3Dot(N,
				XMVector3Cross(XMLoadFloat3(&V2V0),
					XMLoadFloat3(&V2X))));
			if (dot < 0.0f)
				continue;


			// 当たり

			X;// 衝突点
			N;// 衝突ポリゴンの法線

			// 反射方向に進行ベクトルを変更
			XMVECTOR vecW = XMVectorSet(vW.x, vW.y, vW.z, 1.0f);
			vecW = vecW - 2 * XMVector3Dot(vecW, N)*N;
			XMStoreFloat3(&vW, vecW);
			m_Transform.GetVel().x += vW.x;
			m_Transform.GetVel().y += vW.y;
			m_Transform.GetVel().z += vW.z;
			
			// 頂点配列、インデックス配列の解放
			SAFE_DELETE_ARRAY(pIndex);
			SAFE_DELETE_ARRAY(pVertex);

			return true;// 当たっている
		}
		// 頂点配列、インデックス配列の解放
		SAFE_DELETE_ARRAY(pIndex);
		SAFE_DELETE_ARRAY(pVertex);
	}

	

	return false;
}


//**************************************************************************************************************
// 当たり判定まとめ処理
//**************************************************************************************************************
void CModel::Collision()
{
	// 触れたら死ぬオブジェクトとの当たり判定
	vector<CGameObj*> vectorColObj = CGameObj::CollisionOBB();
	if (0 < vectorColObj.size())
	{
		for (auto itr = vectorColObj.begin(); itr != vectorColObj.end();)
		{
			CGameObj* pColObj = *itr;
			// 死亡判定オブジェクトに触れると死亡
			if (pColObj->GetTag() == GOT_DEATHOBJ)
			{
				m_Status.nHP = 0;

				return;
			}

			++itr;
		}
	}



	//**********当たり判定**********
	// 地形モデル接地フラグとOBB接地フラグ
	bool bTerrian, bBBox;
	bTerrian = bBBox = false;
	
	// 地形着地判定
	XMFLOAT3 X, N;
	if (m_pLand->Collision(
		this,
		XMFLOAT3(
			m_Transform.GetPos().x,
			m_Transform.GetPos().y + m_vBBox.y,
			m_Transform.GetPos().z),// P0 線分の始点
		XMFLOAT3(
			m_Transform.GetPos().x,
			m_Transform.GetPos().y - 1.0f,
			m_Transform.GetPos().z),  // P1 線分の終点(少し地面にめり込むまで線分を伸ばしている)
		&X, &N))
	{
		// 接地している場合の処理
		// Y軸方向の速度0
		m_Transform.SetVel(XMFLOAT3(m_Transform.GetVel().x, 0, m_Transform.GetVel().z));
		// 加速度0
		m_Transform.SetCurAccel(XMFLOAT3(0, 0, 0));
		// 位置調整
		m_Transform.SetPos(X);
		// 地形モデルに接地
		bTerrian = true;

		m_Transform.GetWorld()._41 = m_Transform.GetPos().x;
		m_Transform.GetWorld()._42 = m_Transform.GetPos().y;
		m_Transform.GetWorld()._43 = m_Transform.GetPos().z;
	}

	// OBBとの当たり判定
	float fLandAngle;
	if (0 < CollisionOBBPlane(&fLandAngle, &X).size())
	{
		if (fLandAngle < XMConvertToRadians(45.0f))
		{
			// Y軸方向の速度0
			m_Transform.SetVel(XMFLOAT3(m_Transform.GetVel().x, 0, m_Transform.GetVel().z));
			// 加速度0
			m_Transform.SetCurAccel(XMFLOAT3(0, 0, 0));
			// OBBモデルに接地
			bBBox = true;
		}
		// 位置調整
		m_Transform.SetPos(X);
		m_Transform.GetWorld()._41 = m_Transform.GetPos().x;
		m_Transform.GetWorld()._42 = m_Transform.GetPos().y;
		m_Transform.GetWorld()._43 = m_Transform.GetPos().z;
	}

	// 地形モデルかOBBのいずれかに接地していれば接地フラグON
	if (bTerrian || bBBox)
	{
		m_bGround = true;
	}
	else
	{
		m_bGround = false;


	}
}
