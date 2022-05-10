//=============================================================================
//
// ゲーム クラス定義 [Game.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "Scene.h"
#include "Mesh.h"

// クラス定義
class CGame_Scene : public CScene
{
private:

public:
	CGame_Scene();
	virtual ~CGame_Scene();

	virtual bool Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();

	
	
};
