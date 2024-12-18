// 地面 [Land.cpp]
#include "Land.h"
#include "DebugProc.h"
#include "octree.h"
#include "Scene.h"
#include "Player.h"
#include "Input.h"
#include <list>


namespace{
	const float LIMIT_LANDANGLE = 45.0f; // 乗れる斜面角度の限界
	const float DYNAMIC_FRICTION = 0.85f;
}


// コンストラクタ
CLand::CLand(CScene* pScene) : CModel(pScene)
{
	SetTag(GOT_LAND);
	SetObjID(EObjID::OI_LAND);
	m_pVertex = nullptr;
	m_nVertex = 0;
	m_pIndex = nullptr;
	m_nIndex = 0;

}

// デストラクタ
CLand::~CLand()
{
}

HRESULT CLand::Awake() // 最初の初期化
{
	SetModel(MT_LAND);

	// 頂点配列、インデックス配列を取得しておく
	CAssimpModel* pModel = GetAssimp(MT_LAND);
	pModel->GetVertexCount(&m_nVertex, &m_nIndex);
	m_pVertex = new TAssimpVertex[m_nVertex];
	m_pIndex = new UINT[m_nIndex];
	pModel->GetVertex(m_pVertex, m_pIndex);


	// (ここでいいのか？)
	// オクトツリー初期化
	// ツリーを初期化・構築
	Octree* pOctree = CScene::GetOctree();
	pOctree->InitOctree(m_pVertex, m_pIndex, m_nIndex);
	// 対象フラグをクリア
	pOctree->InitSearchTree(pOctree->GetRootNode());
	// 現在の探索ポイントでツリーを探索、対象フラグを設定
	pOctree->SearchTree(pOctree->GetRootNode(), pOctree->GetSearchPoint());

	// メッシュにカメラ＆ライトセット
	pOctree->SetCameraOctTree(m_pScene->GetCameraManager()->GetCamera());
	pOctree->SetLightOctTree(m_pScene->GetLight());

	return S_OK;
}

// 初期化
HRESULT CLand::Init()
{
	return S_OK;
}

// 終了処理
void CLand::Fin()
{
	// オクトツリー削除
	Octree* pOctree = CScene::GetOctree();
	pOctree->DeleteOctree(pOctree->GetRootNode());
	pOctree->ClearVector();

	CModel::Fin();

	// 頂点配列、インデックス配列の解放
	SAFE_DELETE_ARRAY(m_pIndex);
	SAFE_DELETE_ARRAY(m_pVertex);
}

void CLand::Update()  // 更新処理
{

	Octree* pOctree = CScene::GetOctree();
	if (CInput::GetKeyTrigger(VK_O))
	{
		pOctree->SetDrawObjectFlg(!pOctree->GetDrawObjectFlg());
		
	}
	if (CInput::GetKeyTrigger(VK_P))
	{
		pOctree->SetDrawLandOctreeFlg(!pOctree->GetDrawLandOctreeFlg());
	
	}
	if (CInput::GetKeyTrigger(VK_I))
	{
		pOctree->SetDrawRootNodeFlg(!pOctree->GetDrawRootNodeFlg());

	}
	if (CInput::GetKeyTrigger(VK_U))
	{
		pOctree->SetDrawAllNodeFlg(!pOctree->GetDrawAllNodeFlg());

	}
	
	
	//CModel::Update();
}

void CLand::Draw()
{

	CModel::Draw();	
	
	
	// Octree描画
	m_pScene->GetLight()->SetEnable(false);
	Octree* pOctree = CScene::GetOctree();
	pOctree->DrawOctree();
	m_pScene->GetLight()->SetEnable(true);
}

bool CLand::Collision(XMFLOAT3 vPos, XMFLOAT3 vP0, XMFLOAT3 vP1, XMFLOAT3* pX, XMFLOAT3* pN)
{
	/*
	地形のオクツリーの中からプレイヤーの座標が所属するノード + その周囲 = 計27ノードを探索し、
	そのノードに含まれるポリゴンについてレイによる当たり判定を行う
	*/
	NODE* pNodeArray[27];
	XMFLOAT3 _vPos = vPos;
	// 所属するノード + 周囲ノードを配列に格納していく(ポリゴンが含まれていないならnullが格納される)
	Octree* pOctree = CScene::GetOctree();
	int nArrayNum = 0;
	float fMinNodeScaleX = pOctree->GetMinNodeScale();
	float fMinNodeScaleY = fMinNodeScaleX;
	float fMinNodeScaleZ = fMinNodeScaleX;
	for (int k = 0, nCodeZ = -1; k < 3; ++k, ++nCodeZ)
	{
		_vPos.z = vPos.z + fMinNodeScaleZ * nCodeZ;
		for (int j = 0, nCodeY = -1; j < 3; ++j, ++nCodeY)
		{
			_vPos.y = vPos.y + fMinNodeScaleY * nCodeY;
			for (int i = 0, nCodeX = -1; i < 3; ++i, ++nCodeX)
			{
				_vPos.x = vPos.x + fMinNodeScaleX * nCodeX;

				// 配列に格納
				pNodeArray[nArrayNum] = pOctree->SearchVector(&_vPos);
				++nArrayNum;
			}
		}
	}

	// レイ（線分）
	XMFLOAT3 vW(vP1.x - vP0.x, vP1.y - vP0.y, vP1.z - vP0.z);
	for (int j = 0; j < 27; ++j)
	{
		if (pNodeArray[j]) {
			// 全ての三角形について
			for (UINT i = 0; i < pNodeArray[j]->index.nIndex; i += 3) {
				// 三角形の頂点座標
				XMFLOAT3& V0 = m_pVertex[pNodeArray[j]->index.pIndex[i + 0]].vPos;
				XMFLOAT3& V1 = m_pVertex[pNodeArray[j]->index.pIndex[i + 1]].vPos;
				XMFLOAT3& V2 = m_pVertex[pNodeArray[j]->index.pIndex[i + 2]].vPos;
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
				if (pX) *pX = X;
				if (pN) XMStoreFloat3(pN, N);

				return true;// 当たっている
			}
		}
	}

	return false;	// 当たっていない
}


bool CLand::Collision(CGameObj* pObj, XMFLOAT3 vPX, XMFLOAT3 vPZ, XMFLOAT3* pX, XMFLOAT3* pN)
{
	bool bLand = false;
	XMFLOAT3 vSlopeVec = XMFLOAT3(0,0,0); // 斜面にのった後摩擦力も計算した後の運動ベクトル
	/*
	地形のオクツリーの中からプレイヤーの座標が所属するノード + その周囲 = 計27ノードを探索し、
	そのノードに含まれるポリゴンについてレイによる当たり判定を行う
	*/
	std::vector<int> NodeIndexVector = pObj->GetIndexWork()->CurIndex;
	// 所属するノード + 周囲ノードを配列に格納していく(ポリゴンが含まれていないならnullが格納される)
	Octree* pOctree = CScene::GetOctree();
	
	XMFLOAT3 vP0 = XMFLOAT3(
		pObj->GetTransform()->GetPos().x + pObj->GetCenter().x,
		pObj->GetTransform()->GetPos().y + pObj->GetCenter().y,
		pObj->GetTransform()->GetPos().z + pObj->GetCenter().z
	);
	XMFLOAT3 vP1 = XMFLOAT3(
		pObj->GetTransform()->GetPos().x,
		pObj->GetTransform()->GetPos().y,
		pObj->GetTransform()->GetPos().z
	);
	
	// レイ（線分）
	XMFLOAT3 vW(
		vP1.x - vP0.x,
		vP1.y - vP0.y,
		vP1.z - vP0.z);

	// 自分のOBBの中心点から進行方向にベクトルを飛ばし、衝突したポリゴン上の点までの距離
	// (これが最小の値のポリゴンの押し出しベクトルを返す)
	float fCenterToFacePointDistance;
	XMStoreFloat(&fCenterToFacePointDistance, XMVector3Length(XMLoadFloat3(&vW))); // 最大値で初期化
	
	XMVECTOR vecCenter = XMLoadFloat3(&vP0);	// 自分のＯＢＢの中心点

	for (auto itr = NodeIndexVector.begin(); itr != NodeIndexVector.end();)
	{
		NODE* pNode = pOctree->GetNodeInVector(*itr);
		if (pNode) {
			
			// *******************地形系モデルとの当たり判定************************
			// 全ての三角形について
			for (UINT i = 0; i < pNode->index.nIndex; i += 3) {
				// 三角形の頂点座標
				XMFLOAT3& V0 = m_pVertex[pNode->index.pIndex[i + 0]].vPos;
				XMFLOAT3& V1 = m_pVertex[pNode->index.pIndex[i + 1]].vPos;
				XMFLOAT3& V2 = m_pVertex[pNode->index.pIndex[i + 2]].vPos;
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
				long double t = (long double)(dot / base);

				if (t < 0.0f)
					continue;	// 交点が始点の後ろ
				if (t > 1.0f)
					continue;   // 交点が終点よりも奥


				XMFLOAT3 X(
					(float)(vP0.x + t * vW.x),
					(float)(vP0.y + t * vW.y),
					(float)(vP0.z + t * vW.z));

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



				// 自分のOBB中心点からの距離が最も近いメッシュを導く
				float fDistance;
				XMStoreFloat(&fDistance, XMVector3Length(XMVectorSet(X.x, X.y, X.z, 1.0f) - vecCenter));
				if (fCenterToFacePointDistance < fDistance)
				{
					continue;
				}
				else
				{
					fCenterToFacePointDistance = fDistance;

					// 斜面角度が大きいと斜面方向にずり落ちる
					float fLandAngle;
					XMStoreFloat(&fLandAngle, XMVector3AngleBetweenNormals(-XMVector3Normalize(XMLoadFloat3(&vW)), N));
					if (XMConvertToRadians(LIMIT_LANDANGLE) < fLandAngle)
					{
						// 斜面角度
						float fSlopeAngle;
						XMStoreFloat(&fSlopeAngle, XMVector3AngleBetweenNormals(XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f), -N));
						// 斜面の横方向単位ベクトル
						XMVECTOR vecSlopeX = XMVector3Cross(XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f), N);
						// 斜面下方向単位ベクトル
						XMVECTOR vecSlopeUnder = XMVector3Normalize(XMVector3Cross(vecSlopeX, -N));

						// 運動量
						float fUnderLength;
						fUnderLength = GRAVITY_SCALE * sinf(fSlopeAngle);
						// 摩擦力
						float fFriction;
						fFriction = DYNAMIC_FRICTION * GRAVITY_SCALE * cosf(fSlopeAngle);
						// 運動量決定
						XMStoreFloat3(&vSlopeVec, vecSlopeUnder * fUnderLength - vecSlopeUnder * fFriction);



						// 当たり
						if (pX) *pX = X;
						if (pN) XMStoreFloat3(pN, N);
						bLand = true;
					}
					else
					{
						

						// 当たり
						if (pX) *pX = X;
						if (pN) XMStoreFloat3(pN, N);
						bLand = true;
					}
				}
			}
		}
		++itr;
	}

	if (bLand)
	{
		// 斜面運動ベクトルを加算
		// 運動量を加速度に加算
		pObj->GetTransform()->SetCurAccel(
			XMFLOAT3(
				vSlopeVec.x + pObj->GetTransform()->GetCurAccel().x,
				vSlopeVec.y + pObj->GetTransform()->GetCurAccel().y,
				vSlopeVec.z + pObj->GetTransform()->GetCurAccel().z
			));
	}

	return bLand;	// 当たっていない
}