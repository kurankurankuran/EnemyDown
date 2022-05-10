////=============================================================================
////
//// ステージセレクト処理 [StageSelect.h]
//// Author : KAZUKI KURA
////
////=============================================================================
//#pragma once
//
//#include "Scene.h"
//
//// ポーズメニュー
//typedef enum
//{
//	/* ステージ追加の場合下に追記 */
//	TUTORIAL,
//	STAGE_1,	
//	STAGE_2,
//	STAGE_3,
//	//STAGE_4,
//	//STAGE_5,
//
//	MAX_STAGE_SELECT
//} STAGE_MENU;
//
////*****************************************************************************
//// プロトタイプ宣言
////*****************************************************************************
//
//class CStageSelect :public CScene
//{
//private:
//	//ID3D11ShaderResourceView* m_pTexBG; // 背景
//	ID3D11ShaderResourceView* m_pTexStage[MAX_STAGE_SELECT]; // ステージ番号文字
//
//	STAGE_MENU m_nCurrentSelect;
//
//	//bool m_bInputUpNow;
//	//bool m_bInputUpOld;
//	//bool m_bInputDownNow;
//	//bool m_bInputDownOld;
//
//	
//	int m_nSclCnt; // 拡縮タイマー
//	bool m_bScaleUp; // 拡大中フラグ
//	XMFLOAT3 m_vColor; // 枠色
//
//
//	CCamera m_camera;
//	CLight m_light;
//	
//public:
//
//	CStageSelect();
//	~CStageSelect();
//
//	virtual bool Init();
//	virtual void Fin();
//	virtual void Update();
//	virtual void Draw();
//
//	//void SetStageSelect(void);
//	//STAGE_MENU GetStageSelect(void);
//};