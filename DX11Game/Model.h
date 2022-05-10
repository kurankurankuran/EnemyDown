//=============================================================================
//
// モデル クラス定義 [Model.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "GameObj.h"
#include "AssimpModel.h"

#define PLAYER_BS_RAD (200.0f)
#define PLAYER_CENTER_X (0.0f)
#define PLAYER_CENTER_Y (0.0f)
#define PLAYER_CENTER_Z (0.0f)

#define ENEMY_BS_RAD (50.0f)
#define ENEMY_CENTER_X (0.0f)
#define ENEMY_CENTER_Y (0.0f)
#define ENEMY_CENTER_Z (0.0f)

#define MISSILE_BS_RAD (50.0f)
#define MISSILE_CENTER_X (0.0f)
#define MISSILE_CENTER_Y (0.0f)
#define MISSILE_CENTER_Z (0.0f)

enum EModelType {
	MT_PALADIN = 0, // 騎士
	MT_MUTANT,	    // ミュータント
	MT_WIZARD,	    // 魔法使い
	MT_MARIA,	    // マリア
	MT_SPHERE,		// 敵球体
	MT_SKY,		    // スカイドーム
	MT_LAND,		// 地面
	MT_CUBE,        // キューブ
	MT_ROCK,		// 岩
	MT_CLIFF,       // 崖

	MAX_MODEL
};

//// ステート列挙
//enum CharaState {
//	Wait = 0,
//	Walk,
//	Dash,
//	Jump,
//	DashJump,
//	Attack,
//	LongRangeAttack,
//	Reload,
//	ReloadWalk,
//	HitBack,
//	Dying
//};

class CLand;
class CModel : public CGameObj
{
private:
	static CAssimpModel* m_pModels[MAX_MODEL];
	CAssimpModel* m_pModel;
	EModelType m_ModelType;
protected:
	//// ステート
	//CharaState m_State;
	// 地形
	CLand* m_pLand;
	// 死亡エフェクト再生時間タイマー
	int m_nDethEffectTimer; 
	// 現在のアニメーション番号
	int m_nAnimStack;
	// 次のアニメーション番号
	int m_nNextAnimStack;
	// アニメーション時間
	double m_dAnimTime;
public:
	// ********コンストラクタ・デストラクタ*******
	CModel(CScene* pScene);
	virtual ~CModel();

	// ********モデルデータ読み込み・解放処理*********
	static HRESULT LoadAll();
	static void ReleaseAll();

	// ********４大処理*******
	virtual HRESULT Awake();
	virtual HRESULT Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();
	virtual void DrawAlpha();

	// *********ゲッターセッター********
	// モデルデータ
	void SetModel(EModelType model);
	EModelType GetModelType(void) { return m_ModelType; }
	// Asiimpデータ
	static CAssimpModel* GetAssimp(EModelType model);
	//// ステート
	//void SetState(CharaState state) { m_State = state; }
	//CharaState GetState(void) { return m_State; }
	// 被ダメージ・死亡演出開始(GameObjTagからエフェクトを自動選択)
	void StartHitMotion();
	void StartDeath();       
	// カメラ・ライト
	void SetCamera(CCamera* camera) { m_pModel->SetCamera(camera); }
	void SetLight(CLight* light) { m_pModel->SetLight(light); }
	// アニメ関係
	void SetAnimIndex(int nAnimIndex) { m_pModel->SetAnimIndex(nAnimIndex); }
	void SetAnimTime(double dTime) { m_pModel->SetAnimTime(dTime); }
	double GetAnimDuration(int nAnimIndex = -1/* -1:現在のアニメ */) { return m_pModel->GetAnimDuration(nAnimIndex); }
	int GetAnimStack(void) { return m_nAnimStack; }
	int GetNextAnimStack(void) { return m_nNextAnimStack; }
	double GetAnimTime(void) { return m_dAnimTime; }
	XMFLOAT4X4 GetBoneGlobalMatrix(std::string BoneName);
	
	// *********当たり判定**********
	// 衝突した相手のOBBの外側に押し出す
	std::vector<CGameObj*> CollisionOBBPlane(float* fAngle = nullptr, XMFLOAT3* newPos = nullptr);
	// OBB当たり判定(ノードが重なっているオブジェクトとの)
	bool CollisionOBB(CGameObj* pOther);
	// 当たり判定まとめ
	void Collision();    
};