// プレイヤー [Player.h]
#pragma once
#include <vector>
#include "Model.h"
#include "WeaponManager.h"
#include "Land.h"
#include "AttackMethod.h"
#include<memory>


enum EParts {
	PARTS_BODY = 0,
	PARTS_CANON,


	MAX_PARTS

};



struct TPartsInfo {
	int nModel;
	int nParent;
	XMFLOAT3 vPos;
	XMFLOAT3 vAngle;
	XMFLOAT3 vScl;
	bool bUnit;
};

class CWeaponManager;
class CPlayer : public CModel
{
public:
	
private:
	// 武器
	std::unique_ptr<CAttackMethod> m_uqpAttackMethod;
	// ダッシュ可能回数
	int m_nDashNum;
	// ダッシュ移動量
	float m_fDashVol;
	// 地形
	CLand* m_pLand;
	// カメラ
	CCamera* m_pCamera;


	int m_nTimeCnt;   // 画面枠を赤くする用（仮）
	int m_nPlusCnt;   // 画面枠を赤くする用（仮）

public:
	CPlayer(CScene* pScene); // コンストラクタ
	virtual ~CPlayer();		 // デストラクタ
	virtual HRESULT Init();	 // 初期化
	virtual void Fin();		 // 終了処理
	virtual void Update();	 // 更新
	virtual void Draw();	 // 描画
	virtual void DrawUI();	 // UI描画

	//void ChangeWeapon();
	//XMFLOAT4X4 RotationModel(XMFLOAT3 vMove);
	void SetVisible(bool bVisible = true);

private:
	void AttackMethod(XMFLOAT4X4& mW);
	void MouseMove(XMFLOAT3& vMove, XMFLOAT4X4& mW);
	
	void DrawCurrentState(CharaState state);
	void AnimationUpdate();
};
