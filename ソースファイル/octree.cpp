//=======================================================================================
//
//	  Octree関連モジュール
//
//=======================================================================================
#include "octree.h"
#include "model.h"
#include <vector>
#include <algorithm>  // std::sort, std::unique

//---------------------------------------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------------------------------------
Octree::Octree()
{
	// ルートノード初期化
	m_RootNode.bIndex = false;
	ZeroMemory(&m_RootNode.box, sizeof(BOX));
	for (int i = 0; i < 8; ++i) {
		m_RootNode.child[i] = nullptr;
	}
	m_RootNode.index.nIndex = 0;
	m_RootNode.index.pIndex = nullptr;
	m_RootNode.ObjList.clear();
	
	// デバッグ用メンバ変数
	m_bDrawObject = false;		                    // 描画フラグ(オブジェクトが存在する)
	m_bDrawRoot = false;                            // 描画フラグ(ルートノード)
	m_bDrawLandOctree = false;                      // 描画フラグ（地形ポリゴンが存在するノード）
	m_bDrawAll = false;                             // 描画フラグ(全ての最小ノード)
	m_SearchPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 探索ポイント
	m_fRootNodeScale = 20000.0f;                    // 最小ノードの大きさ
	m_fMinNodeLimit = 200.0f;                       // ルートノードの最小限界
	float fScaleWork = m_fRootNodeScale;            // 最小ノードの大きさ計算用ワーク
	
	// 最小ノード計算
	while (true)
	{
		fScaleWork = fScaleWork / 2;
		if (fScaleWork < m_fMinNodeLimit)
		{	
			fScaleWork *= 2;
			break;
		}
	}
	m_fMinNodeScale = fScaleWork;
}

//---------------------------------------------------------------------------------------
// デストラクタ
//---------------------------------------------------------------------------------------
Octree::~Octree()
{
}

//---------------------------------------------------------------------------------------
// Octree初期化
//---------------------------------------------------------------------------------------
void Octree::InitOctree(TAssimpVertex* pVertex, UINT* pIndex, UINT nIndex)
{
	// 空間レベル初期化
	m_nSpaceLevel = 0;
	
	// ルートノード初期化
	m_RootNode.bIndex = false;
	ZeroMemory(&m_RootNode.box, sizeof(BOX));
	for (int i = 0; i < 8; ++i) {
		m_RootNode.child[i] = nullptr;
	}
	m_RootNode.index.nIndex = 0;
	m_RootNode.index.pIndex = nullptr;
	m_RootNode.ObjList.clear();
	
	// 最初に最大範囲を設定
	m_RootNode.box.x_min = -m_fRootNodeScale/2.0f;			
	m_RootNode.box.x_max =  m_fRootNodeScale/2.0f;
	m_RootNode.box.y_min = -m_fRootNodeScale/2.0f;
	m_RootNode.box.y_max =  m_fRootNodeScale/2.0f;
	m_RootNode.box.z_min = -m_fRootNodeScale/2.0f;
	m_RootNode.box.z_max =  m_fRootNodeScale/2.0f;

	// ルートノードは全ての頂点のインデックス情報を持つ
	m_RootNode.index.pIndex = pIndex;
	m_RootNode.index.nIndex = nIndex;
	
	// オクツリーを構築
	MakeOctree(&m_RootNode, pVertex);					

	// ノード描画用頂点作成
	MakeLineVertex();
}

//---------------------------------------------------------------------------------------
// プリミティブ初期化
//---------------------------------------------------------------------------------------
void Octree::MakeLineVertex()
{
	ID3D11Device* pDevice = GetDevice();

	m_meshBox.SetPimitiveType(PT_LINE);
	
	int nVertexNum = 8;
	VERTEX_3D* pVertexWk = new VERTEX_3D[nVertexNum];
	VERTEX_3D* pVtx = pVertexWk;
	pVtx[0].vtx = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pVtx[1].vtx = XMFLOAT3(1.0f, 1.0f, 0.0f);
	pVtx[2].vtx = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pVtx[3].vtx = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pVtx[4].vtx = XMFLOAT3(0.0f, 1.0f, 1.0f);
	pVtx[5].vtx = XMFLOAT3(1.0f, 1.0f, 1.0f);
	pVtx[6].vtx = XMFLOAT3(1.0f, 0.0f, 1.0f);
	pVtx[7].vtx = XMFLOAT3(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 8; ++i, ++pVtx) {
		pVtx->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx->nor = XMFLOAT3(0, 0, 0);
		pVtx->tex = XMFLOAT2(0, 0);
	}
	int	box_idx[] = {	// LINELIST で描画するときの連結順序
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7,
	};
	
	int nNumIndex = _countof(box_idx);
	int* pIndexWk = new int[nNumIndex];
	int* pIdx = pIndexWk;
	for (int i = 0; i < nNumIndex; ++i, ++pIdx) {
		*pIdx = box_idx[i];
	}

	m_meshBox.Init(pVertexWk, nVertexNum, pIndexWk, nNumIndex);

	delete[] pIndexWk;
	delete[] pVertexWk;

	m_meshSearchPoint.SetPimitiveType(PT_LINE);
	nVertexNum = 6;
	pVertexWk = new VERTEX_3D[nVertexNum];
	pVtx = pVertexWk;
	pVtx[0].vtx = XMFLOAT3(-3000.0f, 0.0f, 0.0f);
	pVtx[1].vtx = XMFLOAT3(3000.0f, 0.0f, 0.0f);
	pVtx[2].vtx = XMFLOAT3(0.0f, -3000.0f, 0.0f);
	pVtx[3].vtx = XMFLOAT3(0.0f, 3000.0f, 0.0f);
	pVtx[4].vtx = XMFLOAT3(0.0f, 0.0f, -3000.0f);
	pVtx[5].vtx = XMFLOAT3(0.0f, 0.0f, 3000.0f);
	for (int i = 0; i < nVertexNum; ++i, ++pVtx) {
		pVtx->diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		pVtx->nor = XMFLOAT3(0, 0, 0);
		pVtx->tex = XMFLOAT2(0, 0);
	}
	nNumIndex = 6;
	pIndexWk = new int[nNumIndex];
	pIdx = pIndexWk;
	for (int i = 0; i < nNumIndex; ++i, ++pIdx) {
		*pIdx = i;
	}
	
	m_meshSearchPoint.Init(pVertexWk, nVertexNum, pIndexWk, nNumIndex);
	delete[] pIndexWk;
	delete[] pVertexWk;
}

//---------------------------------------------------------------------------------------
// Octree終了
//---------------------------------------------------------------------------------------
void Octree::DeleteOctree(NODE* nodept)
{
	for (int i = 0; i < 8; i++) {
		if (nodept->child[i]) {
			DeleteOctree(nodept->child[i]);		// チャイルドがあれば再帰で削除
			SAFE_DELETE_ARRAY(nodept->child[i]->index.pIndex);
			delete nodept->child[i];			// メモリから削除
			nodept->child[i] = NULL;
		}
		else
		{
			SAFE_DELETE_ARRAY(nodept->index.pIndex);
		}
	}
	nodept->ObjList.clear();
	m_meshBox.Fin();
}

void Octree::ClearVector()
{
	// 最小ノードvectorをクリア
	m_NodesVector.clear();
}

//---------------------------------------------------------------------------------------
// Octree構築
//---------------------------------------------------------------------------------------
void Octree::MakeOctree(NODE* nodept,TAssimpVertex* pVertex)
{
	BOX testbox;
	int index;

	// ８領域分リピート
	for (index = 0; index < 8; ++index) {
		// ループ変数を元に現在領域を分割した対象領域を求める(求める順番はモートン順序に準じているので変えないこと)
		switch (index)
		{
		case 0:// 左・下・手前
			testbox.x_min = nodept->box.x_min;
			testbox.x_max = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.y_min = nodept->box.y_min;
			testbox.y_max = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.z_min = nodept->box.z_min;
			testbox.z_max = (nodept->box.z_max + nodept->box.z_min) / 2;
			break;
		case 1:// 右・下・手前
			testbox.x_min = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.x_max = nodept->box.x_max;
			testbox.y_min = nodept->box.y_min;
			testbox.y_max = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.z_min = nodept->box.z_min;
			testbox.z_max = (nodept->box.z_max + nodept->box.z_min) / 2;
			break;
		case 2:// 左・上・手前
			testbox.x_min = nodept->box.x_min;
			testbox.x_max = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.y_min = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.y_max = nodept->box.y_max;
			testbox.z_min = nodept->box.z_min;
			testbox.z_max = (nodept->box.z_max + nodept->box.z_min) / 2;
			break;
		case 3:// 右・上・手前
			testbox.x_min = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.x_max = nodept->box.x_max;
			testbox.y_min = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.y_max = nodept->box.y_max;
			testbox.z_min = nodept->box.z_min;
			testbox.z_max = (nodept->box.z_max + nodept->box.z_min) / 2;
			break;
		case 4:// 左・下・奥
			testbox.x_min = nodept->box.x_min;
			testbox.x_max = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.y_min = nodept->box.y_min;
			testbox.y_max = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.z_min = (nodept->box.z_max + nodept->box.z_min) / 2;
			testbox.z_max = nodept->box.z_max;
			break;
		case 5:// 右・下・奥
			testbox.x_min = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.x_max = nodept->box.x_max;
			testbox.y_min = nodept->box.y_min;
			testbox.y_max = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.z_min = (nodept->box.z_max + nodept->box.z_min) / 2;
			testbox.z_max = nodept->box.z_max;
			break;
		case 6:// 左・上・奥
			testbox.x_min = nodept->box.x_min;
			testbox.x_max = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.y_min = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.y_max = nodept->box.y_max;
			testbox.z_min = (nodept->box.z_max + nodept->box.z_min) / 2;
			testbox.z_max = nodept->box.z_max;
			break;
		case 7:// 右・上・奥
			testbox.x_min = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.x_max = nodept->box.x_max;
			testbox.y_min = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.y_max = nodept->box.y_max;
			testbox.z_min = (nodept->box.z_max + nodept->box.z_min) / 2;
			testbox.z_max = nodept->box.z_max;
			break;
		
		
		default:
			break;
		}
		// 対象領域のサイズが十分小さくなったらこれ以上分割せず関数を終了
		if (testbox.x_max - testbox.x_min < m_fMinNodeLimit)
		{
			// デバッグ用に地形ポリゴンが存在している最小ノードリストに追加
			// 領域内に頂点データがあるかチェック
			// 頂点インデックス情報格納ワーク
			INDEX* pIndex = new INDEX;
			pIndex->pIndex = nullptr;
			pIndex->nIndex = 0;
			if (CheckNodeBoxHit(&nodept->box, pVertex, pIndex, nodept->index))
			{
				// 頂点が存在するフラグ設定
				nodept->bIndex = true;
				m_LandDrawNodesIndexVector.push_back(m_NodesVector.size());
			}
			// 頂点インデックス情報格納ワーク解放
			SAFE_DELETE_ARRAY(pIndex->pIndex);
			SAFE_DELETE(pIndex);
			
			// 最小ノードvectorにセット
			m_NodesVector.push_back(nodept);
			


			break;
		}
	
		
		// 頂点インデックス情報格納ワーク
		INDEX* pIndex = new INDEX;
		pIndex->pIndex = nullptr;
		pIndex->nIndex = 0;
		// 領域内に頂点データがあるかチェック
		if (CheckNodeBoxHit(&testbox, pVertex, pIndex, nodept->index)) 
		{
			// 空間レベルを＋１
			m_nSpaceLevel++;

			// 頂点が存在するフラグ設定
			nodept->bIndex = true;

			// データが領域内に存在するなら新規ノードを作成
			nodept->child[index] = new NODE;

			// 一度全てのメンバを初期化
			nodept->child[index]->bIndex = false;
			ZeroMemory(&nodept->child[index]->box, sizeof(BOX));
			for (int i = 0; i < 8; ++i) {
				nodept->child[index]->child[i] = nullptr;
			}
			nodept->child[index]->index.nIndex = 0;
			nodept->child[index]->index.pIndex = nullptr;
			nodept->child[index]->ObjList.clear();
		
			// 新規ノードの領域を設定
			nodept->child[index]->box = testbox;
			// 新規ノードの頂点インデックス情報を設定
			nodept->child[index]->index = *pIndex;
		
			// 頂点インデックス情報格納ワーク解放
			//SAFE_DELETE_ARRAY(pIndex->pIndex);
			SAFE_DELETE(pIndex);
			// 再帰呼び出しで新規ノードの分割を実行
			MakeOctree(nodept->child[index], pVertex);
			// 空間レベル-1
			m_nSpaceLevel--;
		}
		else
		{
			// 頂点データが無くても最後までノード作成
			// 空間レベルを＋１
			m_nSpaceLevel++;

			// 新規ノードを作成
			nodept->child[index] = new NODE;
			
			// 一度全てのメンバを初期化
			nodept->child[index]->bIndex = false;
			ZeroMemory(&nodept->child[index]->box, sizeof(BOX));
			for (int i = 0; i < 8; ++i) {
				nodept->child[index]->child[i] = nullptr;
			}
			nodept->child[index]->index.nIndex = 0;
			nodept->child[index]->index.pIndex = nullptr;
			nodept->child[index]->ObjList.clear();
		
			// 新規ノードの領域を設定
			nodept->child[index]->box = testbox;
			// 頂点インデックス情報格納ワーク解放
			SAFE_DELETE(pIndex);
			// 再帰呼び出しで新規ノードの分割を実行
			MakeOctree(nodept->child[index], pVertex);
			// 空間レベル-1
			m_nSpaceLevel--;
		}
		
		// 頂点インデックス情報格納ワーク解放
		if (pIndex) {
			delete pIndex;
		}
		
	}


}

//---------------------------------------------------------------------------------------
// Octree内ボックスのポリゴン含みチェック
//---------------------------------------------------------------------------------------
bool Octree::CheckNodeBoxHit(BOX* boxpt, TAssimpVertex* pVertex, INDEX* pIndex, INDEX _Index)
{

	// ボックス内に一つでも頂点があればノードを作成
	TAssimpVertex* pt;
	pt = pVertex;
	// ノード内に存在する頂点のインデックス情報一時格納用
	std::vector<UINT> Indexvector;
	
	/* 
	インデックスを元に総頂点から、各ノードのボックス内に頂点が存在するポリゴンを全て特定し、
	ポリゴンの頂点のインデックスを各ノードに格納する
	*/
	for (UINT i = 0; i < _Index.nIndex; i += 3) 
	{
		// ポリゴンの頂点がノードボックス内に存在するか
		bool bIn = false;

		if (boxpt->x_min <= pt[_Index.pIndex[i + 0]].vPos.x && pt[_Index.pIndex[i + 0]].vPos.x < boxpt->x_max &&
			boxpt->y_min <= pt[_Index.pIndex[i + 0]].vPos.y && pt[_Index.pIndex[i + 0]].vPos.y < boxpt->y_max &&
			boxpt->z_min <= pt[_Index.pIndex[i + 0]].vPos.z && pt[_Index.pIndex[i + 0]].vPos.z < boxpt->z_max)
			bIn = true;
		if (!bIn) {
			if (boxpt->x_min <= pt[_Index.pIndex[i + 1]].vPos.x && pt[_Index.pIndex[i + 1]].vPos.x < boxpt->x_max &&
				boxpt->y_min <= pt[_Index.pIndex[i + 1]].vPos.y && pt[_Index.pIndex[i + 1]].vPos.y < boxpt->y_max &&
				boxpt->z_min <= pt[_Index.pIndex[i + 1]].vPos.z && pt[_Index.pIndex[i + 1]].vPos.z < boxpt->z_max)
				bIn = true;
		}
		if (!bIn) {
			if (boxpt->x_min <= pt[_Index.pIndex[i + 2]].vPos.x && pt[_Index.pIndex[i + 2]].vPos.x < boxpt->x_max &&
				boxpt->y_min <= pt[_Index.pIndex[i + 2]].vPos.y && pt[_Index.pIndex[i + 2]].vPos.y < boxpt->y_max &&
				boxpt->z_min <= pt[_Index.pIndex[i + 2]].vPos.z && pt[_Index.pIndex[i + 2]].vPos.z < boxpt->z_max)
				bIn = true;
		}

		/* 
		ノードボックス内に一つでも頂点が存在していた場合、存在した全ての頂点インデックス情報を
		ノード内インデックス情報に保存する
		*/
		if (bIn)
		{
			Indexvector.push_back(_Index.pIndex[i + 0]);
			Indexvector.push_back(_Index.pIndex[i + 1]);
			Indexvector.push_back(_Index.pIndex[i + 2]);
		}
	}
	
	// vector内にインデックスが格納されていれば頂点が存在していた
	if (Indexvector.size() > 0) 
	{
		// インデックス数保存
		pIndex->nIndex = Indexvector.size();
		// ノード内総インデックス数分メモリ確保
		pIndex->pIndex = new UINT[Indexvector.size()];
		
		// インデックスデータ保存
		for (UINT i = 0; i < Indexvector.size(); ++i)
		{
			pIndex->pIndex[i] = Indexvector[i];
		}

		return true;
	}
	else
	{
		return false;
	}
	
}

//---------------------------------------------------------------------------------------
// Octree描画
//---------------------------------------------------------------------------------------
void Octree::DrawOctreeBox(NODE* nodept)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxScale, mtxTrans;
	
	// 頂点データを設定
	mtxScale = XMMatrixScaling(
		nodept->box.x_max - nodept->box.x_min,
		nodept->box.y_max - nodept->box.y_min,
		nodept->box.z_max - nodept->box.z_min);
	mtxWorld = mtxScale;
	mtxTrans = XMMatrixTranslation(nodept->box.x_min, nodept->box.y_min, nodept->box.z_min);
	mtxWorld *= mtxTrans;
	XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);

	m_meshBox.SetDiffuse((0 < nodept->ObjList.size()) ? XMFLOAT4(1.0f, 0.0f, 1.0f, 0.5f) : XMFLOAT4(0.25f, 0.5f, 1.0f, 0.25f));
	m_meshBox.Draw();
	// チャイルドノードのボックス描画
	for (int i = 0; i < 8; i++) {
		if (nodept->child[i] && nodept->bIndex) {
			DrawOctreeBox(nodept->child[i]);
		}
	}
}

void Octree::DrawOctreeVector()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// 描画するノードリストを全て精査し、描画フラグがオフになっているものはリストから省く、ONのものは描画する。
	for (auto itr = m_DrawNodesIndexVector.begin(); itr != m_DrawNodesIndexVector.end();)
	{
		NODE* pNode = m_NodesVector[*itr];
		if (pNode->ObjList.size() <= 0)
		{
			itr = m_DrawNodesIndexVector.erase(itr);
			continue;
		}
		else
		{
			XMMATRIX mtxWorld, mtxScale, mtxTrans;

			// 頂点データを設定
			mtxScale = XMMatrixScaling(
				pNode->box.x_max - pNode->box.x_min,
				pNode->box.y_max - pNode->box.y_min,
				pNode->box.z_max - pNode->box.z_min);
			mtxWorld = mtxScale;
			mtxTrans = XMMatrixTranslation(pNode->box.x_min, pNode->box.y_min, pNode->box.z_min);
			mtxWorld *= mtxTrans;
			XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);

		
			bool bIn[2] = { false, false };
			XMFLOAT3 Diffuse = XMFLOAT3(0, 0, 0);
			for (auto NodeItr = pNode->ObjList.begin(); NodeItr != pNode->ObjList.end();)
			{
				CGameObj* pObj = *NodeItr;
				if (!bIn[0] && pObj->GetTag() == GOT_PLAYER)
				{
					// プレイヤーが登録されたノードを色付け
					Diffuse = XMFLOAT3(Diffuse.x, Diffuse.y, Diffuse.z + 1.0f);
					bIn[0] = true;
				}
				else if (!bIn[1] && pObj->GetTag() == GOT_ENEMY || pObj->GetTag() == GOT_BOSS)
				{
					// 敵が登録されたノードを色付け
					Diffuse = XMFLOAT3(Diffuse.x + 1.0f, Diffuse.y, Diffuse.z);
					bIn[1] = true;
				}
				++NodeItr;
			}

			if (!bIn[0] && !bIn[1])
			{
				// 該当のオブジェクトが登録されていない
				++itr;
				continue;
			}

			// 色設定
			m_meshBox.SetDiffuse(XMFLOAT4(Diffuse.x, Diffuse.y, Diffuse.z, 0.5f));
			// 描画
			m_meshBox.Draw();
			++itr;
		}
	}
}

// 地形を含む最小ノードのみ描画
void Octree::DrawOctreeLandVector()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// 描画するノードリストを全て精査し、描画フラグがオフになっているものはリストから省く、ONのものは描画する。
	for (auto itr = m_LandDrawNodesIndexVector.begin(); itr != m_LandDrawNodesIndexVector.end();)
	{
		NODE* pNode = m_NodesVector[*itr];
		XMMATRIX mtxWorld, mtxScale, mtxTrans;

		// 頂点データを設定
		mtxScale = XMMatrixScaling(
			pNode->box.x_max - pNode->box.x_min,
			pNode->box.y_max - pNode->box.y_min,
			pNode->box.z_max - pNode->box.z_min);
		mtxWorld = mtxScale;
		mtxTrans = XMMatrixTranslation(pNode->box.x_min, pNode->box.y_min, pNode->box.z_min);
		mtxWorld *= mtxTrans;
		XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);
		
		for (auto NodeItr = pNode->ObjList.begin(); NodeItr != pNode->ObjList.end();)
		{
			CGameObj* pObj = *NodeItr;
			if (pObj->GetTag() == GOT_PLAYER)
			{
				// プレイヤーが登録されたノードを色付け
				m_meshBox.SetDiffuse(XMFLOAT4(0.0f, 1.0f, 1.0f, 0.5f));
				break;
			}
			else
			{
				// 地形モデルが登録されたノードを色付け
				m_meshBox.SetDiffuse(XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f));
			}
			++NodeItr;
		}
		
		// 描画を実行
		m_meshBox.Draw();
		++itr;
	}
}

// ルートノードの描画
void Octree::DrawOctreeRootNode()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// ルートノードのみ描画
	m_RootNode;
	XMMATRIX mtxWorld, mtxScale, mtxTrans;

	// 頂点データを設定
	mtxScale = XMMatrixScaling(
		m_RootNode.box.x_max - m_RootNode.box.x_min,
		m_RootNode.box.y_max - m_RootNode.box.y_min,
		m_RootNode.box.z_max - m_RootNode.box.z_min);
	mtxWorld = mtxScale;
	mtxTrans = XMMatrixTranslation(m_RootNode.box.x_min, m_RootNode.box.y_min, m_RootNode.box.z_min);
	mtxWorld *= mtxTrans;
	XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);

	m_meshBox.SetDiffuse(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));

	// 描画を実行
	m_meshBox.Draw();
}

// 全ての最小ノードの描画
void Octree::DrawOctreeAll()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	for (auto itr = m_NodesVector.begin(); itr != m_NodesVector.end();)
	{
		XMMATRIX mtxWorld, mtxScale, mtxTrans;
		NODE* pNode = *itr;
		// 頂点データを設定
		mtxScale = XMMatrixScaling(
			pNode->box.x_max - pNode->box.x_min,
			pNode->box.y_max - pNode->box.y_min,
			pNode->box.z_max - pNode->box.z_min);
		mtxWorld = mtxScale;
		mtxTrans = XMMatrixTranslation(pNode->box.x_min, pNode->box.y_min, pNode->box.z_min);
		mtxWorld *= mtxTrans;
		XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);

		m_meshBox.SetDiffuse(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));

		// 描画を実行
		m_meshBox.Draw();

		++itr;
	}

	
}


//----- Octree の描画
void Octree::DrawOctree()
{
	if (m_bDrawObject) 
	{
		// プレイヤーと敵
		DrawOctreeVector();
	}
	if (m_bDrawLandOctree)
	{
		// 地形モデル
		DrawOctreeLandVector();
	}
	if (m_bDrawRoot)
	{
		// ルートノード
		DrawOctreeRootNode();
	}
	if (m_bDrawAll)
	{
		// 全ての最小ノード
		DrawOctreeAll();
	}
}

//---------------------------------------------------------------------------------------
// Octree探索初期化
//---------------------------------------------------------------------------------------
void Octree::InitSearchTree(NODE* nodept)
{
	// セレクトされていない状態にセット
	//nodept->OnSelect = false;		
	// チャイルドノードの状態もリセット
	//for (int i = 0; i < 8; i++) {
	//	// チャイルドノードのセレクトがtrueなら再帰で続行
	//	if (nodept->child[i] && nodept->child[i]->OnSelect) InitSearchTree(nodept->child[i]);
	//}
}

//---------------------------------------------------------------------------------------
// Octree探索
//---------------------------------------------------------------------------------------
NODE* Octree::SearchTree(NODE* nodept, XMFLOAT3* point)
{
	//***********************************************************************************
	//  ここに探索ポイント（引数 point）が対象ノードの領域に含まれていれば
	//  ノードの OnSelect フラグがＴＲＵＥになるような処理を追加する。
	//  もちろんチャイルド（子）を持っていれば、再帰で実行して同様の処理を
	//  末端まで行う。
	//***********************************************************************************
	NODE* pNode = nullptr;
	if (nodept->box.x_min <= point->x&&point->x < nodept->box.x_max &&
		nodept->box.y_min <= point->y&&point->y < nodept->box.y_max &&
		nodept->box.z_min <= point->z&&point->z < nodept->box.z_max)
	{
		// 現探索ノードボックス内に探索ポイントが含まれれば、戻り値更新
		pNode = nodept;
		// 子ノードも探索
		for (int i = 0; i < 8; ++i) {
			// 子ノードが存在すれば再帰で実行
			if (nodept->child[i]) {
				pNode = SearchTree(nodept->child[i], point);
				if (pNode)
					return pNode;
				else
					continue;
			}
		}
		// 子ノードが無い or 子ノードを全て探索したが、探索ポイントを含む子ノードがなかった
		return pNode;
	}
	else
	{
		// 範囲外ならnullを返す
		return nullptr;
	}
}

NODE* Octree::SearchVector(XMFLOAT3* point)
{
	// 空間番号を求める
	XMFLOAT3 _point;
	_point.x = (point->x - (-m_fRootNodeScale / 2/*ルート空間の左端座標のX成分*/))   / m_fMinNodeScale;
	_point.y = (point->y - (-m_fRootNodeScale / 2/*ルート空間の下端座標のY成分*/))   / m_fMinNodeScale;
	_point.z = (point->z - (-m_fRootNodeScale / 2/*ルート空間の手前端座標のZ成分*/)) / m_fMinNodeScale;

	// マイナスの値になるはずがないのでマイナスならnullを返す
	if (_point.x < 0 || _point.y < 0 || _point.z < 0)
	{
		return nullptr;
	}

	// 最小ノードのインデックス番号を求める
	int vectorIndex = Get3DMortonOrder((BYTE)_point.x, (BYTE)_point.y, (BYTE)_point.z);

	// ルートノード外の座標についてはnullを返す
	if (vectorIndex < 0 || (int)m_NodesVector.size() <= vectorIndex)
	{
		return nullptr;
	}

	// vector内インデックス番号箇所にポリゴンが存在するか
	if (m_NodesVector[vectorIndex]->bIndex)
	{
		return m_NodesVector[vectorIndex];
	}
	else
	{
		return nullptr;
	}
}

int Octree::GetVectorNumber(XMFLOAT3* point)
{
	// 空間番号を求める
	XMFLOAT3 _point;
	_point.x = (point->x - (-m_fRootNodeScale / 2/*ルート空間の左端座標のX成分*/))   / m_fMinNodeScale;
	_point.y = (point->y - (-m_fRootNodeScale / 2/*ルート空間の下端座標のY成分*/))   / m_fMinNodeScale;
	_point.z = (point->z - (-m_fRootNodeScale / 2/*ルート空間の手前端座標のZ成分*/)) / m_fMinNodeScale;

	// マイナスの値になるはずがないのでマイナスならnullを返す
	if (_point.x < 0 || _point.y < 0 || _point.z < 0)
	{
		return -1;
	}

	// 最小ノードのインデックス番号を求める
	int vectorIndex = Get3DMortonOrder((BYTE)_point.x, (BYTE)_point.y, (BYTE)_point.z);

	// ルートノード外の座標についてはnullを返す
	if (vectorIndex < 0 || (int)m_NodesVector.size() <= vectorIndex)
	{
		return -1;
	}

	return vectorIndex;
}

bool Octree::EraseAndPushObjInNode(NODEINDEXWORK* pIndexWork, CGameObj* pObj)
{
	/* 参照ノードの選択方法について
	自分の中心座標を中心としてOBBを完全に包み込む、最小ノードの集まりで作る立方体を求め、その最小ノード全てについて参照する
	まず自身のOBBの中心からXYZそれぞれの面までの距離を求め、
	それぞれを最小ノードの大きさで割った値＋１は
	最終的に参照する最小ノードの集まりであるOBBにおける、中心ノードを除いた中心から各面までのノード数である。
	そのノード数  *          2             +     1
	　　　　　    　(中心基準の反対側分)      (中心分)
	が最小ノードの集まりであるOBBの一辺を構成する最小ノードの数になる
	これらの数値を使用してＯＢＢの中心を含む最小ノードを中心とする最小ノードの集合体OBBを求め、その構成最小ノードを全て所属ノードとする
	（結果的に所属する可能性があるノードとなっている）
	*/

	// 前回所属していたノードを保存
	pIndexWork->OldIndex.clear();
	pIndexWork->OldIndex = pIndexWork->CurIndex;
	pIndexWork->CurIndex.clear();

	// OBBのXYZそれぞれの面までの距離を求める
	XMFLOAT3 vOBBScl = pObj->GetBBox();
	XMFLOAT3 vObjScale = pObj->GetTransform()->GetScale();
	float fObjOBBScalX;
	float fObjOBBScalY;
	float fObjOBBScalZ;
	
	fObjOBBScalX = vOBBScl.x * vObjScale.x;
	fObjOBBScalY = vOBBScl.y * vObjScale.y;
	fObjOBBScalZ = vOBBScl.z * vObjScale.z;

	// 最小ノード集まりOBBの中心ノードを除く、中心から各面までのノード数
	int nNodeNumX = (int)(fObjOBBScalX / m_fMinNodeScale + 1);
	int nNodeNumY = (int)(fObjOBBScalY / m_fMinNodeScale + 1);
	int nNodeNumZ = (int)(fObjOBBScalZ / m_fMinNodeScale + 1);

	// 各軸方向ベクトル
	XMFLOAT4X4 mW = pObj->GetTransform()->GetWorld();
	XMVECTOR vecX = XMVector3Normalize(XMVectorSet(mW._11, mW._12, mW._13, 1.0f));
	XMVECTOR vecY = XMVector3Normalize(XMVectorSet(mW._21, mW._22, mW._23, 1.0f));
	XMVECTOR vecZ = XMVector3Normalize(XMVectorSet(mW._31, mW._32, mW._33, 1.0f));
	XMVECTOR vCenter = XMLoadFloat3(&pObj->GetCenter());
	XMMATRIX mWorld = XMMatrixTranslationFromVector(vCenter);
	mWorld *= XMLoadFloat4x4(&mW);
	XMStoreFloat4x4(&mW, mWorld);

	// 最小ノード集まり立方体の左下手前の座標
	XMVECTOR vecBaceVertex = XMVectorSet(mW._41, mW._42, mW._43, 1.0f);
	vecBaceVertex +=
		- vecX * m_fMinNodeScale * (float)nNodeNumX
		- vecY * m_fMinNodeScale * (float)nNodeNumY
		- vecZ * m_fMinNodeScale * (float)nNodeNumZ;

	XMFLOAT3 vCheckPoint;
	// 最初のチェックポイントは左下手前の頂点
	XMVECTOR vecCheckPoint = vecBaceVertex;

	// 最小ノードの集まり立方体の一辺のノード数
	int nLoopNumX = nNodeNumX * 2 + 1;
	int nLoopNumY = nNodeNumY * 2 + 1;
	int nLoopNumZ = nNodeNumZ * 2 + 1;

	for (int k = 0; k < nLoopNumZ; ++k)
	{
		vecCheckPoint += vecZ * m_fMinNodeScale * (float)k;
		for (int j = 0; j < nLoopNumY; ++j)
		{
			vecCheckPoint += vecY * m_fMinNodeScale * (float)j;
			for (int i = 0; i < nLoopNumX; ++i)
			{
				vecCheckPoint += vecX * m_fMinNodeScale * (float)i;

				// 求めたチェックポイントの座標から所属ノードのインデックス番号を求め、保存する
				XMStoreFloat3(&vCheckPoint, vecCheckPoint);
				int nIndexNum = GetVectorNumber(&vCheckPoint);

				// 有効でないインデックス値は-1で例外処理
				if (nIndexNum < 0)
				{
					vecCheckPoint -= vecX * m_fMinNodeScale * (float)i;
					continue;
				}
				pIndexWork->CurIndex.push_back(nIndexNum);

				// デバッグ用に描画フラグをOnにする(描画フラグがtrue == 登録されているオブジェクトがあるので描画vectorに追加しない)
				if (m_NodesVector[nIndexNum]->ObjList.size() <= 0)
				{
					// TODO : 重複チェック（長時間プレイを放置すると重複により要素が際限なく増える）
					m_DrawNodesIndexVector.push_back(nIndexNum);
				}
				vecCheckPoint -= vecX * m_fMinNodeScale * (float)i;
			}
			vecCheckPoint -= vecY * m_fMinNodeScale * (float)j;
		}
		vecCheckPoint -= vecZ * m_fMinNodeScale * (float)k;
	}



	// 描画ノードリストから重複を省く
	/*std::sort(m_DrawNodesIndexVector.begin(), m_DrawNodesIndexVector.end());
	m_DrawNodesIndexVector.erase(std::unique(m_DrawNodesIndexVector.begin(), m_DrawNodesIndexVector.end()), m_DrawNodesIndexVector.end());*/

	// OldIndexとCurIndexを比較し、OldIndexの中にあり、CurIndexに無い値をインデックス値とする最小ノード内のObjListから自身へのポインタを省く。
	bool bWork = false; // 発見するとtrueに
	for (unsigned int j = 0; j < pIndexWork->OldIndex.size(); ++j)
	{
		bWork = false;
		for (unsigned int i = 0; i < pIndexWork->CurIndex.size(); ++i)
		{
			if (pIndexWork->OldIndex[j] != pIndexWork->CurIndex[i])
			{
				continue;
			}
			else
			{
				// 同じ値を発見
				bWork = true;
				break;
			}

		}
		if (!bWork)
		{
			// 最小ノードから自身へのポインタを省く
			EraseObjInNode(pIndexWork->OldIndex[j], pObj);
			
		}
	}


	// CurIndexにあり、OldIndexに無い値をインデックス値とする最小ノードに自身へのポインタを追加する。
	for (unsigned int j = 0; j < pIndexWork->CurIndex.size(); ++j)
	{
		bWork = false;
		for (unsigned int i = 0; i < pIndexWork->OldIndex.size(); ++i)
		{
			if (pIndexWork->CurIndex[j] != pIndexWork->OldIndex[i])
			{
				continue;
			}
			else
			{
				// 同じ値を発見したので追加しない
				bWork = true;
				break;
			}

		}
		if (!bWork)
		{
			// 最小ノードに自身へのポインタを追加
			PushObjInNode(pIndexWork->CurIndex[j], pObj);
		}
	}

	return true;
}

NODE* Octree::GetNodeInVector(int nIndex)
{ 
	// 範囲外
	if (nIndex < 0 || (int)m_NodesVector.size() <= nIndex)return nullptr;

	return m_NodesVector[nIndex]; 
}

// 最小ノードのオブジェクト所属リストに追加する
bool Octree::PushObjInNode(int nIndex, CGameObj* pObj)               
{
	// 同じオブジェクトが同じノードに複数回登録されることを防ぐ（重複チェック）
	bool bCheck = true;
	for (auto itr = m_NodesVector[nIndex]->ObjList.begin(); itr != m_NodesVector[nIndex]->ObjList.end();)
	{
		if (*itr == pObj)
		{
			// 既に登録されている
			bCheck = false;
			break;
		}

		++itr;
	}
	if (bCheck) {
		m_NodesVector[nIndex]->ObjList.push_back(pObj);
	}
	return bCheck;
}

// 最小ノードのオブジェクト所属リストから省く
bool Octree::EraseObjInNode(int nIndex, CGameObj* pObj)              
{
	// 範囲外
	if (nIndex < 0 || (int)m_NodesVector.size() < nIndex)return false;

	for (auto itr = m_NodesVector[nIndex]->ObjList.begin(); itr != m_NodesVector[nIndex]->ObjList.end();)
	{
		if (*itr != pObj)
		{
			++itr;
		}
		else
		{
			// ポインタ発見
			itr = m_NodesVector[nIndex]->ObjList.erase(itr);
			return true;
		}
	}

	// リスト内に該当ポインタ無し（一回の処理で同じノード内に探索ポイントが複数ある場合に起こる）
	return false;
}

void Octree::SetCameraOctTree(CCamera* pCamera)
{
	m_meshBox.SetCamera(pCamera);
	m_meshSearchPoint.SetCamera(pCamera);
}

void Octree::SetLightOctTree(CLight* pLight)
{
	m_meshBox.SetLight(pLight);
	m_meshSearchPoint.SetLight(pLight);
}


DWORD Octree::BitSeparateFor3D(BYTE n)
{
	DWORD s = n;
	s = (s | s << 8) & 0x0000f00f;
	s = (s | s << 4) & 0x000c30c3;
	s = (s | s << 2) & 0x00249249;
	return s;
}

DWORD Octree::Get3DMortonOrder(BYTE x, BYTE y, BYTE z)  
{
	return BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2;
}
//=======================================================================================
//	End of File
//=======================================================================================
