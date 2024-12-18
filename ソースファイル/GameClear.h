//=============================================================================
//
// ゲームクリアー画面クラス定義 [GameClear.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "Scene.h"

// クラス定義
class CGameClear : public CScene
{
private:
	bool m_bStart;
	int m_nTimer;

public:
	CGameClear();
	virtual ~CGameClear();

	virtual bool Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();
};
