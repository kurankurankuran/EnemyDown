//=======================================================================================
//
//	  Octree関連モジュール
//
//=======================================================================================
#pragma once
#include "main.h"
#include "AssimpModel.h"
#include "mesh.h"
#include "Camera.h"
#include "Light.h"
#include <vector>
#include <list>

//-------- 構造体
struct BOX {					// 領域
	float	x_min, x_max;			// Ｘ方向　最小値、最大値
	float	y_min, y_max;			// Ｙ方向　最小値、最大値
	float	z_min, z_max;			// Ｚ方向　最小値、最大値
};

struct INDEX {
	UINT* pIndex;               // ポリゴンの頂点情報を格納するためのメモリ領域確保用 
	UINT  nIndex;               // ノード内の精査するインデックス数
};


struct NODE {					        // ノード
	NODE	*child[8];			        // 子ノード（８個分）
	BOX		box;				        // 領域
								        
	INDEX   index;                      // 領域内のポリゴンの頂点インデックス情報
	bool    bIndex;                     // 領域内にポリゴンが存在するか
	//bool	OnSelect;				    // セレクト中フラグ

	std::list<CGameObj*>   ObjList;     // ノードに登録されているオブジェクトのリスト
};

struct NODEINDEXWORK {
	std::vector<int> OldIndex;
	std::vector<int> CurIndex;
};

class Octree
{
private:
	NODE		m_RootNode;			     // ルートノード		     
	float       m_fRootNodeScale;        // ルートノードの大きさ
	float       m_fMinNodeLimit;         // ルートノードの最小限界
	float       m_fMinNodeScale;         // 最小ノードの大きさ
	int         m_nSpaceLevel;           // 空間レベル(ルートが0)
	std::vector<NODE*> m_NodesVector;    // 最小ノードのポインタvector

	

	// デバッグ用メンバ変数		   	     
	std::vector<int> m_DrawNodesIndexVector;        // 描画する最小ノードのインデックスvector（オブジェクトが登録されているノードのみ）
	std::vector<int> m_LandDrawNodesIndexVector;    // 描画する最小ノードのインデックスvector（地形ポリゴンを含むノードのみ）
	bool		m_bDrawObject;		     // Octree 描画フラグ(オブジェクトが登録されているノード)
	bool		m_bDrawLandOctree;		 // Octree 描画フラグ（地形ポリゴンが存在するノード）
	bool		m_bDrawRoot;		 // Octree 描画フラグ（ルートノード）
	bool		m_bDrawAll;		     // Octree 描画フラグ（全ての最小ノード）
	XMFLOAT3	m_SearchPoint;		     // 探索ポイント
	CMesh		m_meshBox;               // Octree 描画用メッシュ
	CMesh		m_meshSearchPoint;       // 探索ポイント可視化するメッシュ
private:
	void MakeLineVertex();                           // Octree&探索ポイント描画用頂点作成
	DWORD BitSeparateFor3D(BYTE n);                  // 3バイトごとに間隔を開ける関数
	DWORD Get3DMortonOrder(BYTE x, BYTE y, BYTE z);  // 8分木モートン順序算出関数
public:
	Octree();
	~Octree();


	//-------- 関数プロトタイプ
	void InitOctree(TAssimpVertex* pVertex, UINT* pIndex, UINT nIndex);
	void DeleteOctree(NODE* nodept);
	void ClearVector();                                  // 最小ノードvectorをクリア
	void MakeOctree(NODE* nodept, TAssimpVertex* pVertex);
	bool CheckNodeBoxHit(BOX *boxpt, TAssimpVertex* pVertex, INDEX* pIndex, INDEX _Index);
	void DrawOctreeBox(NODE* nodept);
	void DrawOctreeVector();
	void DrawOctreeLandVector();
	void DrawOctreeRootNode();
	void DrawOctreeAll();
	void DrawOctree();
	void InitSearchTree(NODE* nodept);
	NODE* SearchTree(NODE* nodept, XMFLOAT3* point);               // 八分木内をルートから順にたどり所属ノードを取り出す
	NODE* SearchVector(XMFLOAT3* point);                           // 線形八分木内から所属ノードを取り出す
	int   GetVectorNumber(XMFLOAT3* point);                        // 座標が所属する最小ノードのvectorにおけるインデックス番号を取得する
	bool  EraseAndPushObjInNode(NODEINDEXWORK* , CGameObj*);       // 最小ノードのオブジェクト所属リストに追加と、前回所属していたノードから省く
	bool  PushObjInNode(int, CGameObj*);                           // 最小ノードのオブジェクト所属リストに追加する
	bool  EraseObjInNode(int, CGameObj*);                          // 最小ノードのオブジェクト所属リストから省く
	NODE* GetRootNode() { return &m_RootNode; }
	NODE* GetNodeInVector(int nIndex);
	float GetMinNodeScale() { return m_fMinNodeScale; }
	void SetSearchPoint(XMFLOAT3 vPoint) { m_SearchPoint = vPoint; }
	XMFLOAT3* GetSearchPoint() { return &m_SearchPoint; }
	void SetDrawObjectFlg(bool bflg) { m_bDrawObject = bflg; }
	bool GetDrawObjectFlg() { return m_bDrawObject; }
	void SetDrawLandOctreeFlg(bool bflg) { m_bDrawLandOctree = bflg; }
	bool GetDrawLandOctreeFlg() { return m_bDrawLandOctree; }
	void SetDrawRootNodeFlg(bool bflg) { m_bDrawRoot = bflg; }
	bool GetDrawRootNodeFlg() { return m_bDrawRoot; }
	void SetDrawAllNodeFlg(bool bflg) { m_bDrawAll = bflg; }
	bool GetDrawAllNodeFlg() { return m_bDrawAll; }

	void SetCameraOctTree(CCamera* pCamera);
	void SetLightOctTree(CLight* pLight);

	
};