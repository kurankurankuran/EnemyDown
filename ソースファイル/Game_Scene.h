//=============================================================================
//
// �Q�[�� �N���X��` [Game.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "Scene.h"
#include "Mesh.h"

// �N���X��`
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
