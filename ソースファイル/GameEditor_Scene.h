//=============================================================================
//
// ゲームシーンエディタークラス定義 [GameEditor_Scene.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "Scene.h"
#include "Mesh.h"
#include "Editor_Manager.h"
#include<memory>
// クラス定義
class CGameEditor_Scene: public CScene
{
private:
	CMesh		m_meshLine;                  // 平行ライト方向描画用メッシュ
	float       m_fRotValue;                 // 平行ライト回転量
	std::unique_ptr<CEditor_Manager> m_upEM; // エディターマネージャー
	//CEventManager m_EventManager;
public:
	CGameEditor_Scene();
	virtual ~CGameEditor_Scene();

	virtual bool Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();
};
