//=============================================================================
//
// 各種エフェクト処理 [EffectManager.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "main.h"
#include "GameObj.h"
#include "Mesh.h"
#include <vector>
#include <list>
//*****************************************************************************
// 定数定義
//*****************************************************************************
enum EffectType {
	EFFECT_EXPLOSION = 0,	// 爆発エフェクト
	EFFECT_SMOKE,	// 飛行機雲エフェクト
	
	MAX_EFFECTTYPE
};

struct EFFECT {
	bool        bLoop;  // ループの有無
	XMFLOAT3	vPos;	// 座標
	XMFLOAT2	vScl;	// 拡縮
	int			nType;	// エフェクト種別
	int			nAnim;	// アニメーション再生位置
	int			nCount;	// アニメーション フレーム数
	float		fAngle;	// 角度
	float		fZoom;	// 拡大率
	float       fAlpha; // 透過
};



class CEffectManager : public CGameObj,public CMesh
{
private:
	std::vector<ID3D11ShaderResourceView*> m_pTexture;	// テクスチャ
	static std::list<EFFECT>	m_Effect;	                // エフェクト情報

public:
	CEffectManager(CScene*);
	~CEffectManager();

	virtual HRESULT Init();		// 初期化
	virtual void Fin();			// 終了処理
	virtual void Update();		// 更新
	virtual void Draw();		// 不透明部分描画

	static void StartEffect(EffectType EffectType, XMFLOAT3 pos, XMFLOAT2 scl, bool bLoop = false);
};
