//=============================================================================
//
// オブジェクト クラス実装 [GameObj.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include"GameObj.h"
#include"Scene.h"
#include <vector>

// 保存する変数の位置を指定
DATARECORD CGameObj::m_gDataRecord[] =
{
   DATA_LOCAL(CGameObj, m_vStartPos),
   DATA_LOCAL(CGameObj, m_vStartAngle),
   DATA_LOCAL(CGameObj, m_vStartScale),
   DATA_LOCAL(CGameObj, m_vCenter),
   DATA_LOCAL(CGameObj, m_vBBox),
   DATA_END
};


// グローバル変数
namespace {
	const CMeshMaterial g_material = {
		XMFLOAT4(0.0f, 1.0f, 0.0f, 0.3f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		1.0f
	};
	const XMFLOAT4 g_vRed(1.0f, 0.0f, 0.0f, 0.3f);
	const XMFLOAT4 g_vGreen(0.0f, 1.0f, 0.0f, 0.3f);
	const float DEL_HITSTOPTIME = 0.1f;
	

	
}



// コンストラクタ
CGameObj::CGameObj(CScene* pScene) : m_pScene(pScene)
{
	// タグ
	m_Tag = GOT_GAMEOBJ;

	// 連結リストに登録
	m_pBack = NULL;
	CGameObj* pTop = m_pScene->GetObj();
	m_pNext = pTop;
	if (pTop) {
		pTop->m_pBack = this;
	}
	else{
		// 次がnullだとリストの末尾
		m_pScene->SetLastObj(this);
	}
	m_pScene->SetObj(this);



	// ステータス初期化
	m_Status.nATK = 0;
	m_Status.nDEF = 0;
	m_Status.nMaxHP = 0;
	m_Status.nHP = 0;
	m_Status.nINT = 0;
	m_Status.nRES = 0;

	// バウンディボックス・スフィア
	m_vBBox = XMFLOAT3(0.5f, 0.5f, 0.5f);
	m_fRadius = 0.5f;
	m_pBBox = nullptr;
	m_pBSphere = nullptr;
	m_eBS = BOUND_NONE;

	// 可視
	m_bVisible = true;
	// ヒットストップ時間
	m_fHitStopTime = 0.0f;
}

// デストラクタ
CGameObj::~CGameObj(void)
{
	// 連結リストから削除
	if (m_pNext) {
		m_pNext->m_pBack = m_pBack;
	}
	else
	{
		// 前がいないということは現在の自分の後ろのオブジェクトが末尾
		m_pScene->SetLastObj(m_pBack);
	}
	if (m_pBack) {
		m_pBack->m_pNext = m_pNext;
	} else {
		m_pScene->SetObj(m_pNext);
	}

	// ノードから自分を削除
	for (unsigned int i = 0; i < m_NodeIndexWork.CurIndex.size(); ++i)
	{
		CScene::GetOctree()->EraseObjInNode(m_NodeIndexWork.CurIndex[i], this);
	}
}

// 初期化前に行いたい処理
HRESULT CGameObj::Awake()    
{
	return S_OK;
}

// 初期化
HRESULT CGameObj::Init()
{
	// 地面接地フラグ
	m_bGround = false;
	// 状態
	SetState(CharaState::Wait);
	// トランスフォーム初期化
	m_Transform.SetPos(m_vStartPos);
	m_Transform.SetAngle(m_vStartAngle);
	m_Transform.SetScale(m_vStartScale);
	m_Transform.UpdateWorld();
	m_Transform.SetVel(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_Transform.SetCurAccel(XMFLOAT3(0.0f, 0.0f, 0.0f));
	

	m_fHitStopTime = 0;
	m_nInvTime = 0;

	CGameObj::SetRadius(m_fRadius);
	CGameObj::SetBBox(m_vBBox);

	// ノードに登録
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

	return S_OK;
}

// 解放
void CGameObj::Fin()
{
	SAFE_DELETE(m_pBBox);
	SAFE_DELETE(m_pBSphere);
}

// 更新q
void CGameObj::Update()
{
	// XZ平面移動
	switch (m_State)
	{
	case Walk:
		// 速度に加速度を加算
		m_Transform.SetVel(
			XMFLOAT3(
				m_Transform.GetVel().x + m_Transform.GetCurAccel().x,
				0.0f,
				m_Transform.GetVel().z + m_Transform.GetCurAccel().z));

		// 最高速度制限
		XMVECTOR vecVel = XMLoadFloat3(&m_Transform.GetVel());
		if (m_Transform.GetVelMax() < XMVectorGetX(XMVector3Length(vecVel)))
		{
			XMStoreFloat3(&m_Transform.GetVel(), XMVector3Normalize(vecVel) * m_Transform.GetVelMax());
		}
		break;
	default:
		m_Transform.SetVel(XMFLOAT3(0, 0, 0));
		break;
	}
	// Y軸移動
	m_Transform.SetVel(
		XMFLOAT3(
			m_Transform.GetVel().x,
			m_Transform.GetVel().y + m_Transform.GetCurAccel().y,
			m_Transform.GetVel().z));
	if (m_Transform.GetVel().y < -m_Transform.GetFallSpeedMax())
	{
		m_Transform.SetVel(XMFLOAT3(
			m_Transform.GetVel().x,
			-m_Transform.GetFallSpeedMax(),
			m_Transform.GetVel().z
		));
	}
	
	// 座標に速度を加算
	m_Transform.SetPos(
		XMFLOAT3(
			m_Transform.GetPos().x + m_Transform.GetVel().x,
			m_Transform.GetPos().y + m_Transform.GetVel().y,
			m_Transform.GetPos().z + m_Transform.GetVel().z
		));

	// ワールド行列に反映
	//m_Transform.UpdateWorld();
	m_Transform.GetWorld()._41 = m_Transform.GetPos().x;
	m_Transform.GetWorld()._42 = m_Transform.GetPos().y;
	m_Transform.GetWorld()._43 = m_Transform.GetPos().z;
	m_Transform.SetWorld(m_Transform.GetWorld());
}

// 描画
void CGameObj::Draw()
{
}

// 半透明描画
void CGameObj::DrawAlpha()
{
	m_pScene->GetLight()->SetEnable(false);
	if (m_eBS != BOUND_NONE) {
		XMVECTOR vCenter = XMLoadFloat3(&m_vCenter);
		int nCurCullNum = GetCurCullMode();

		if (m_eBS == BOUND_OBB) {
			XMMATRIX mWorld = XMMatrixTranslationFromVector(vCenter);
			mWorld *= XMLoadFloat4x4(&m_Transform.GetWorld());
		
			XMFLOAT4X4 mW;
			XMStoreFloat4x4(&mW, mWorld);
			if (m_pBBox) {
				SetCullMode(CULLMODE_NONE);//カリング無し
				m_pBBox->SetWorld(&mW);
				m_pBBox->Draw();
				SetCullMode(nCurCullNum);//元のカリングに戻す
			}
		}
		else
		{
			XMMATRIX mWorld = XMLoadFloat4x4(&m_Transform.GetWorld());
			vCenter = XMVector3TransformCoord(vCenter, mWorld);
			mWorld = XMMatrixTranslationFromVector(vCenter);
			XMFLOAT4X4 mW;
			XMStoreFloat4x4(&mW, mWorld);
			//if (m_pBBox && m_bBBox) {
			if (m_pBBox && m_eBS == BOUND_AABB) {
				SetCullMode(CULLMODE_NONE);//カリング無し
				m_pBBox->SetWorld(&mW);
				m_pBBox->Draw();
				SetCullMode(nCurCullNum);//元のカリングに戻す
			}
			if (m_pBSphere && m_eBS == BOUND_SPHERE) {
				m_pBSphere->SetWorld(&mW);
				m_pBSphere->Draw();
			
			}

		}
	}
	m_pScene->GetLight()->SetEnable(true);

}

void CGameObj::DrawUI()
{
}

// 最初の全初期化
HRESULT CGameObj::AwakeAll(CGameObj* pTop)
{
	HRESULT hr = S_OK;
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		hr = pObj->Awake();
		if (FAILED(hr)) {
			return hr;
		}
	}
	return hr;
}

// 全初期化
HRESULT CGameObj::InitAll(CGameObj* pTop)
{
	HRESULT hr = S_OK;
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		hr = pObj->Init();
		if (FAILED(hr)) {
			return hr;
		}
	}
	return hr;
}

// 全更新
void CGameObj::UpdateAll(CGameObj* pTop)
{
	CGameObj* pNext;
	for (CGameObj* pObj = pTop; pObj; pObj = pNext) {
		pNext = pObj->m_pNext;
		// ヒットストップ処理
		if (pObj->GetHitStopTime() > 0)
		{
			pObj->SetHitStopTime(pObj->GetHitStopTime() - DEL_HITSTOPTIME);
			continue;
		}

		pObj->Update();
	}
}

// 全描画
void CGameObj::DrawAll(CGameObj* pTop)
{
	// 不透明部分描画
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		//if (pObj->m_bVisible) {
			pObj->Draw();
		//}
	}

	// 半透明部分描画
	SetBlendState(BS_ALPHABLEND);
	SetZWrite(false);
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		//if (pObj->m_bVisible) {
			pObj->DrawAlpha();
		//}
	}
	SetZWrite(true);
	SetBlendState(BS_NONE);
	
}

void CGameObj::DrawUIAll(CGameObj* pTop)
{
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		pObj->DrawUI();
	}
}

// 全終了処理
void CGameObj::FinAll(CGameObj* pTop)
{
	CGameObj* pNext;
	for (CGameObj* pObj = pTop; pObj; pObj = pNext) {
		pNext = pObj->m_pNext;
		pObj->Fin();
		delete pObj;
	}
}


void CGameObj::AddHP(int nHP)
{
	m_Status.nHP += nHP;
	if (m_Status.nMaxHP < m_Status.nHP)
	{
		m_Status.nHP = m_Status.nMaxHP;
	}
}
void CGameObj::SetHP(int nHP)
{
	m_Status.nHP = nHP;
	if (m_Status.nMaxHP < m_Status.nHP)
	{
		m_Status.nHP = m_Status.nMaxHP;
	}
}

std::vector<CGameObj*> CGameObj::CollisionOBB()                 // OBB当たり判定(自分のノードに登録されているオブジェクトとの)
{
	std::vector<CGameObj*> ColObjVector;
	if (!m_bVisible)
		return ColObjVector;

	Octree* pOctree = CScene::GetOctree();
	
	// 自分の所属ノードから登録されているオブジェクトを抽出
	std::vector<CGameObj*> NodeInObjVector;
	for (unsigned int i = 0; i < m_NodeIndexWork.CurIndex.size(); ++i)
	{
		for (
			auto itr = pOctree->GetNodeInVector(m_NodeIndexWork.CurIndex[i])->ObjList.begin(); 
			itr != pOctree->GetNodeInVector(m_NodeIndexWork.CurIndex[i])->ObjList.end();
			)
		{
			if (*itr != this) {
				NodeInObjVector.push_back(*itr);
			}
			++itr;
		}
		
	}

	// ベクターから重複を省く
	std::sort(NodeInObjVector.begin(), NodeInObjVector.end());
	NodeInObjVector.erase(std::unique(NodeInObjVector.begin(), NodeInObjVector.end()), NodeInObjVector.end());

	XMFLOAT4X4 mW1 = m_Transform.GetWorld();     // 次フレーム座標の予想ワールド行列
	for (unsigned int i = 0; i < NodeInObjVector.size(); ++i)
	{
		
		XMFLOAT4X4 mW2 = NodeInObjVector[i]->GetTransform()->GetWorld();
		// 中心座標取得
		XMFLOAT3 vP1, vP2;
	
		XMStoreFloat3(&vP1, XMVector3TransformCoord(XMLoadFloat3(&m_vCenter), XMLoadFloat4x4(&mW1)));
		XMStoreFloat3(&vP2, XMVector3TransformCoord(XMLoadFloat3(&NodeInObjVector[i]->GetCenter()), XMLoadFloat4x4(&mW2)));
		// 中心座標間のベクトルを求める
		XMVECTOR vD;
		vD = XMVectorSet(vP2.x - vP1.x, vP2.y - vP1.y, vP2.z - vP1.z, 0.0f);
		// モデル座標軸を求める
		XMVECTOR vN[6];
		vN[0] = XMVectorSet(mW1._11, mW1._12, mW1._13, 0.0f);
		vN[1] = XMVectorSet(mW1._21, mW1._22, mW1._23, 0.0f);
		vN[2] = XMVectorSet(mW1._31, mW1._32, mW1._33, 0.0f);
		vN[3] = XMVectorSet(mW2._11, mW2._12, mW2._13, 0.0f);
		vN[4] = XMVectorSet(mW2._21, mW2._22, mW2._23, 0.0f);
		vN[5] = XMVectorSet(mW2._31, mW2._32, mW2._33, 0.0f);
		// OBBの大きさ(半分)を掛けたベクトルを求める
		XMFLOAT3& vBB1 = m_vBBox;
		XMFLOAT3& vBB2 = NodeInObjVector[i]->GetBBox();
		XMVECTOR vL[6];
		vL[0] = XMVectorSet(mW1._11 * vBB1.x, mW1._12 * vBB1.x, mW1._13 * vBB1.x, 0.0f);
		vL[1] = XMVectorSet(mW1._21 * vBB1.y, mW1._22 * vBB1.y, mW1._23 * vBB1.y, 0.0f);
		vL[2] = XMVectorSet(mW1._31 * vBB1.z, mW1._32 * vBB1.z, mW1._33 * vBB1.z, 0.0f);
		vL[3] = XMVectorSet(mW2._11 * vBB2.x, mW2._12 * vBB2.x, mW2._13 * vBB2.x, 0.0f);
		vL[4] = XMVectorSet(mW2._21 * vBB2.y, mW2._22 * vBB2.y, mW2._23 * vBB2.y, 0.0f);
		vL[5] = XMVectorSet(mW2._31 * vBB2.z, mW2._32 * vBB2.z, mW2._33 * vBB2.z, 0.0f);
		// 分離軸が面の法線ベクトル(モデル座標軸)
		float fL, f, fD;
		bool bCol = true;
		for (int i = 0; i < 6; ++i) {
			XMVECTOR& vS = vN[i];//分離軸(両オブジェクトの座標軸)
			// OBBの影(の半分)の合計
			fL = 0.0f;
			for (int j = 0; j < 6; ++j) {

				// 分離軸に中心点から各面に垂直かつ各面までの距離の大きさベクトルの影を落とし、その長さを合計する
				XMStoreFloat(&f, XMVector3Dot(vS, vL[j]));
				// 絶対値化して加算
				fL += fabsf(f);
			}
			// 中心座標間の距離と比較
			XMStoreFloat(&fD, XMVector3Dot(vS, vD)); // 分離軸に中心間ベクトルの影を落とした時の大きさ（）
			fD = fabsf(fD);
			if (fL < fD)
			{
				bCol = false;/*return false*/;// 当たっていない
				break;
			}
		}
		if (!bCol)
		{
			continue;
		}
		// ３Dの場合は双方の方向ベクトルに垂直な分離軸についても求める 
		// 分離軸がそれぞれの辺に垂直なベクトル
		XMVECTOR vS;
		for (int i = 0; i < 3; ++i) {
			for (int j = 3; j < 6; ++j) {
				// 分離軸候補を求める
				vS = XMVector3Normalize(XMVector3Cross(vN[i], vN[j]));
				// OBBの影(の半分)の合計
				fL = 0.0f;
				for (int k = 0; k < 6; ++k) {
					XMStoreFloat(&f, XMVector3Dot(vS, vL[k]));
					fL += fabsf(f);
				}
				// 中心座標間の距離と比較
				XMStoreFloat(&fD, XMVector3Dot(vS, vD));
				fD = fabsf(fD);
				if (fL < fD)
				{
					bCol = false;/*return false*/;// 当たっていない
					break;
				}
			}
			if (!bCol)
			{
				break;
			}
		}
		if (bCol) {
			ColObjVector.push_back(NodeInObjVector[i]);// 当たっている

			// ここで関数ポインタ利用してクラスごとに衝突時の処理（UnityでいうOnCollisionなど）を追加していいかも

		}
	}

	return ColObjVector; // 当たっているオブジェクトポインタベクターを返す
}

// OBB同士の衝突判定
bool CGameObj::CollisionOBB(CGameObj* pOther)
{
	if(pOther == this) return false;
	if (!m_bVisible || !pOther->m_bVisible)
		return false;

	// ワールド マトリックス取得
	XMFLOAT4X4 mW1 = m_Transform.GetWorld();
	mW1._41 = m_Transform.GetPos().x;
	mW1._42 = m_Transform.GetPos().y;
	mW1._43 = m_Transform.GetPos().z;
	XMFLOAT4X4 mW2 = pOther->GetTransform()->GetWorld();
	// 中心座標取得
	XMFLOAT3 vP1, vP2;
	XMStoreFloat3(&vP1, XMVector3TransformCoord(XMLoadFloat3(&m_vCenter), XMLoadFloat4x4(&mW1)));
	XMStoreFloat3(&vP2, XMVector3TransformCoord(XMLoadFloat3(&pOther->GetCenter()),XMLoadFloat4x4(&mW2)));
	// 中心座標間のベクトルを求める
	XMVECTOR vD;
	vD = XMVectorSet(vP2.x - vP1.x, vP2.y - vP1.y, vP2.z - vP1.z, 0.0f);
	// モデル座標軸を求める
	XMVECTOR vN[6];
	vN[0] = XMVectorSet(mW1._11, mW1._12, mW1._13, 0.0f);
	vN[1] = XMVectorSet(mW1._21, mW1._22, mW1._23, 0.0f);
	vN[2] = XMVectorSet(mW1._31, mW1._32, mW1._33, 0.0f);
	vN[3] = XMVectorSet(mW2._11, mW2._12, mW2._13, 0.0f);
	vN[4] = XMVectorSet(mW2._21, mW2._22, mW2._23, 0.0f);
	vN[5] = XMVectorSet(mW2._31, mW2._32, mW2._33, 0.0f);
	// OBBの大きさ(半分)を掛けたベクトルを求める
	XMFLOAT3& vBB1 = m_vBBox;
	XMFLOAT3& vBB2 = pOther->GetBBox();
	XMVECTOR vL[6];
	vL[0] = XMVectorSet(mW1._11 * vBB1.x, mW1._12 * vBB1.x, mW1._13 * vBB1.x, 0.0f);
	vL[1] = XMVectorSet(mW1._21 * vBB1.y, mW1._22 * vBB1.y, mW1._23 * vBB1.y, 0.0f);
	vL[2] = XMVectorSet(mW1._31 * vBB1.z, mW1._32 * vBB1.z, mW1._33 * vBB1.z, 0.0f);
	vL[3] = XMVectorSet(mW2._11 * vBB2.x, mW2._12 * vBB2.x, mW2._13 * vBB2.x, 0.0f);
	vL[4] = XMVectorSet(mW2._21 * vBB2.y, mW2._22 * vBB2.y, mW2._23 * vBB2.y, 0.0f);
	vL[5] = XMVectorSet(mW2._31 * vBB2.z, mW2._32 * vBB2.z, mW2._33 * vBB2.z, 0.0f);
	// 分離軸が面の法線ベクトル(モデル座標軸)
	float fL, f, fD;
	for (int i = 0; i < 6; ++i) {
		XMVECTOR& vS = vN[i];//分離軸(両オブジェクトの座標軸)
		// OBBの影(の半分)の合計
		fL = 0.0f;
		for (int j = 0; j < 6; ++j) {

			// 分離軸に中心点から各面に垂直かつ各面までの距離の大きさベクトルの影を落とし、その長さを合計する
			XMStoreFloat(&f, XMVector3Dot(vS, vL[j]));
			// 絶対値化して加算
			fL += fabsf(f);
		}
		// 中心座標間の距離と比較
		XMStoreFloat(&fD, XMVector3Dot(vS, vD)); // 分離軸に中心間ベクトルの影を落とした時の大きさ（）
		fD = fabsf(fD);
		if (fL < fD) return false;// 当たっていない
	}
    // ３Dの場合は双方の方向ベクトルに垂直な分離軸についても求める 
	// 分離軸がそれぞれの辺に垂直なベクトル
	XMVECTOR vS;
	for (int i = 0; i < 3; ++i) {
		for (int j = 3; j < 6; ++j) {
			// 分離軸候補を求める
			vS = XMVector3Normalize(XMVector3Cross(vN[i], vN[j]));
			// OBBの影(の半分)の合計
			fL = 0.0f;
			for (int k = 0; k < 6; ++k) {
				XMStoreFloat(&f, XMVector3Dot(vS, vL[k]));
				fL += fabsf(f);
			}
			// 中心座標間の距離と比較
			XMStoreFloat(&fD, XMVector3Dot(vS, vD));
			fD = fabsf(fD);
			if (fL < fD) return false;// 当たっていない
		}
	}
	return true;// 当たっている
}


// AABB同士の衝突判定
bool CGameObj::CollisionAABB(CGameObj* pOther)
{
	if (pOther == this) return false;
	if (!m_bVisible || !pOther->m_bVisible)
		return false;
	// 中心座標を求める
	XMFLOAT3 vA, vB;
	XMStoreFloat3(&vA,
		XMVector3TransformCoord(
			XMLoadFloat3(&m_vCenter),
			XMLoadFloat4x4(&m_Transform.GetWorld())));
	XMStoreFloat3(&vB,
		XMVector3TransformCoord(
			XMLoadFloat3(&pOther->m_vCenter),
			XMLoadFloat4x4(&pOther->GetTransform()->GetWorld())));
	// 衝突判定
	XMFLOAT3& vAr = m_vBBox;
	XMFLOAT3& vBr = pOther->m_vBBox;
	return vA.x - vAr.x <= vB.x + vBr.x &&
		vB.x - vBr.x <= vA.x + vAr.x &&
		vA.y - vAr.y <= vB.y + vBr.y &&
		vB.y - vBr.y <= vA.y + vAr.y &&
		vA.z - vAr.z <= vB.z + vBr.z &&
		vB.z - vBr.z <= vA.z + vAr.z;
}

// 境界球同士の衝突判定
bool CGameObj::CollisionBS(CGameObj* pOther)
{
	if (pOther == this) return false;
	if (!m_bVisible || !pOther->m_bVisible)
		return false;
	// 中心座標を求める
	XMFLOAT3 vA, vB;
	XMStoreFloat3(&vA,
		XMVector3TransformCoord(
			XMLoadFloat3(&m_vCenter),
			XMLoadFloat4x4(&m_Transform.GetWorld())));
	XMStoreFloat3(&vB,
		XMVector3TransformCoord(
			XMLoadFloat3(&pOther->m_vCenter),
			XMLoadFloat4x4(&pOther->GetTransform()->GetWorld())));
	// 衝突判定
	float dx = vA.x - vB.x;
	float dy = vA.y - vB.y;
	float dz = vA.z - vB.z;
	float dr = m_fRadius + pOther->m_fRadius;
	return dx * dx + dy * dy + dz * dz <= dr * dr;
}

// 境界ボックス設定
void CGameObj::SetBBox(XMFLOAT3 vBBox)
{
	HRESULT hr = S_OK;
	if (vBBox.x == 0 || vBBox.y == 0 || vBBox.z == 0)return;
	if (m_pBBox) {
		m_pBBox->Fin();
		SAFE_DELETE(m_pBBox);
	}
	m_vBBox = vBBox;

	m_pBBox = new CBox();
	hr = m_pBBox->Init(vBBox);

	if (FAILED(hr)) {
		m_pBBox->Fin();
		SAFE_DELETE(m_pBBox);
	}
	if (m_pBBox) {
		m_pBBox->SetCamera(m_pScene->GetCameraManager()->GetCamera());
		std::shared_ptr<CCamera> pMiniCamera = m_pScene->GetCameraManager()->GetMiniMapCamera();
		if (pMiniCamera)
		{
			m_pBBox->SetMiniMapCamera(pMiniCamera);
		}
		m_pBBox->SetLight(m_pScene->GetLight());
		m_pBBox->SetMaterial(&g_material);
	}
}

// 境界球設定
void CGameObj::SetRadius(float fRadius)
{
	HRESULT hr = S_OK;
	if (m_pBSphere) {
		m_pBSphere->Fin();
		SAFE_DELETE(m_pBSphere);
	}

	m_fRadius = fRadius;

	m_pBSphere = new CSphere();
	hr = m_pBSphere->Init(16, 8, fRadius);

	if (FAILED(hr)) {
		m_pBSphere->Fin();
		SAFE_DELETE(m_pBSphere);
	}
	if (m_pBSphere) {
		m_pBSphere->SetCamera(m_pScene->GetCameraManager()->GetCamera());
		m_pBSphere->SetLight(m_pScene->GetLight());
		m_pBSphere->SetMaterial(&g_material);
	}
}
