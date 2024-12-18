//=============================================================================
//
// オブジェクト クラス定義 [GameObj.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "main.h"
#include "Transform.h"
#include "Box.h"		  // デバッグ表示用
#include "Sphere.h"		  // デバッグ表示用
#include "Octree.h"       // オクツリー構造体用
#include "SaveObjBase.h"  // データ保存用クラス 


// ********列挙型********
// タグ
enum EGameObjTag {		// オブジェクト型
	GOT_GAMEOBJ = 0,	// 型無し
	GOT_PLAYER,			// プレイヤー
	GOT_ENEMYMANAGER,   // エネミー管理
	GOT_ENEMY,			// 敵
	GOT_BOSS,			// ボス
	GOT_STAGESPHERE,    // ステージ境界球
	GOT_LAND,			// 地面
	GOT_DEATHOBJ,       // 死亡判定オブジェクト

	MAX_GAMEOBJTAG
};

// バウンディの種類
enum EBoundingShape { // デバッグ表示用境界形状
	BOUND_NONE = 0, // 無し
	BOUND_SPHERE, // 境界球
	BOUND_AABB, // 境界BOX(AABB)
	BOUND_OBB, // 境界BOX(OBB)
	MAX_BOUND
};

// ステート列挙
enum CharaState {
	Wait = 0,
	Walk,
	Dash,
	Jump,
	DashJump,
	HandRangeAttack,
	LongRangeAttack,
	Reload,
	ReloadWalk,
	HitBack,
	Dying
};

// *********構造体定義********
// ステータス
typedef struct
{
	int nMaxHP;// 最大体力
	int nHP;   // 現在体力
	int nATK;  // 物理攻撃力
	int nDEF;  // 物理防御力
	int nINT;  // 魔法攻撃力
	int nRES;  // 魔法防御力
}CharaStatus;




// クラス定義
class CScene;
class CGameObj : public CSaveObjBase
{
private:
	// 連結リスト用ポインタ
	CGameObj* m_pBack;		                     // 前のオブジェクト
	CGameObj* m_pNext;		                     // 次のオブジェクト
	static DATARECORD m_gDataRecord[];           // 保存するデータ配列
protected:
	// セーブ用メンバ変数
	XMFLOAT3 m_vStartPos;                        // スポーン位置
	XMFLOAT3 m_vStartAngle;                      // 回転量
	XMFLOAT3 m_vStartScale;                      // 拡縮


	CScene* m_pScene;		                     // 所属シーン
	EGameObjTag m_Tag;		                     // オブジェクト型
	CTransform m_Transform;                      // トランスフォーム
	bool m_bGround;                              // 地面設置フラグ
	bool m_bVisible;		                     // 表示フラグ
	CharaState m_State;                          // ステート
	CharaStatus m_Status;                        // ステータス
	float m_fHitStopTime;                        // ヒットストップ時間
	int m_nInvTime;                              // 無敵時間
	XMFLOAT3 m_vCenter;		                     // 境界ボックス(/球)中心座標
	XMFLOAT3 m_vBBox;		                     // 境界ボックス
	CBox* m_pBBox;			                     // 境界ボックス表示用メッシュ
	float m_fRadius;		                     // 境界球半径
	CSphere* m_pBSphere;	                     // 境界球表示用メッシュ
	EBoundingShape m_eBS;                        // 境界計上表示用フラグ
	NODEINDEXWORK m_NodeIndexWork;               // 所属ノード番号格納用
public:
	CGameObj(CScene* pScene);                    // コンストラクタ
	virtual ~CGameObj(void);                     // デストラクタ

	// *******４大処理関係*****
	virtual HRESULT Awake();                     // 最初の初期化
	virtual HRESULT Init();		                 // 初期化
	virtual void Update();		                 // 更新
	virtual void Draw();		                 // 不透明部分描画
	virtual void DrawAlpha();	                 // 半透明部分描画
	virtual void DrawUI();                       // UI描画
	virtual void Fin();			                 // 終了処理

	// *******連結リストを使用したオブジェクト全てに対する処理******
	static HRESULT AwakeAll(CGameObj* pTop);
	static HRESULT InitAll(CGameObj* pTop);
	static void UpdateAll(CGameObj* pTop);
	static void DrawAll(CGameObj* pTop);
	static void DrawUIAll(CGameObj* pTop);
	static void FinAll(CGameObj* pTop);

	// ********メンバ変数ゲッターセッター********
	// 初期座標・回転・拡縮
	XMFLOAT3 GetStartPos() { return m_vStartPos; }
	void SetStartPos(XMFLOAT3 vStartPos) { m_vStartPos = vStartPos; }
	XMFLOAT3 GetStartAngle() { return m_vStartAngle; }
	void SetStartAngle(XMFLOAT3 vStartAngle) { m_vStartAngle = vStartAngle; }
	XMFLOAT3 GetStartScale() { return m_vStartScale; }
	void SetStartScale(XMFLOAT3 vStartScale) { m_vStartScale = vStartScale; }
	// トランスフォーム
	CTransform* GetTransform() { return &m_Transform; }
	void SetTransform(CTransform TF) { m_Transform = TF; }
	// 接地フラグ
	bool GetOnGround() { return m_bGround; }
	void SetOnGround(bool bOn) { m_bGround = bOn; }
	// タグ
	void SetTag(EGameObjTag Tag) { m_Tag = Tag; }
	EGameObjTag GetTag() { return m_Tag; }
	// 連結リストポインタ
	void SetNext(CGameObj* pObj) { m_pNext = pObj; }
	CGameObj* GetNext() { return m_pNext; }
	void SetBack(CGameObj* pObj) { m_pBack = pObj; }
	CGameObj* GetBack() { return m_pBack; }
	// ステート
	void SetState(CharaState state) { m_State = state; }
	CharaState GetState(void) { return m_State; }
	// ステータス
	void SetStatus(CharaStatus charastatus) { m_Status = charastatus; }
	CharaStatus GetStatus(void) { return m_Status; }
	void AddHP(int nHP);
	void SetMaxHP(int nMaxHP) { m_Status.nMaxHP = nMaxHP; };
	void SetHP(int nHP);
	// 無敵時間
	void SetInvTime(int nIntTime) { m_nInvTime = nIntTime; }
	int GetInvTime(void) { return m_nInvTime; }
	// ヒットストップ時間
	void SetHitStopTime(float fTime) { m_fHitStopTime = fTime; if (m_fHitStopTime < 0)m_fHitStopTime = 0; }
	float GetHitStopTime() { return m_fHitStopTime; }
	// 可視判定
	virtual void SetVisible(bool bVisible = true) { m_bVisible = bVisible; }
	bool GetVisible() { return m_bVisible; }
	// バウンディボックス・スフィア関係
	void SetCenter(XMFLOAT3 vCenter) { m_vCenter = vCenter; }
	XMFLOAT3& GetCenter() { return m_vCenter; }
	void SetBBox(XMFLOAT3 vBBox);
	XMFLOAT3& GetBBox() { return m_vBBox; }
	void SetRadius(float fRadius);
	float GetRadius() { return m_fRadius; }
	// 当たり判定切替用
	void SetBSVisible(EBoundingShape eBS) { m_eBS = eBS; }
	CBox* GetBBOXInstance() { return m_pBBox; }
	// 所属ノード番号格納用
	NODEINDEXWORK* GetIndexWork() { return &m_NodeIndexWork; }

	// ***********当たり判定メソッド***********
	std::vector<CGameObj*>  CollisionOBB();            // OBB当たり判定(自分のノードに登録されているオブジェクトとの)
	virtual bool CollisionOBB(CGameObj* pOther);       // OBB当たり判定(ノードが重なっているオブジェクトとの)
	bool CollisionAABB(CGameObj* pOther);              // AABB当たり判定
	bool CollisionBS(CGameObj* pOther);                // BS当たり判定
	
	virtual DATARECORD *GetDataRecord() { 
		return m_gDataRecord; 
	}
};
