//=============================================================================
//
// �Q�[���V�[���G�f�B�^�[�N���X��` [GameEditor_Scene.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "Scene.h"
#include "Mesh.h"
#include "Editor_Manager.h"
#include<memory>
// �N���X��`
class CGameEditor_Scene: public CScene
{
private:
	CMesh		m_meshLine;                  // ���s���C�g�����`��p���b�V��
	float       m_fRotValue;                 // ���s���C�g��]��
	std::unique_ptr<CEditor_Manager> m_upEM; // �G�f�B�^�[�}�l�[�W���[
	//CEventManager m_EventManager;
public:
	CGameEditor_Scene();
	virtual ~CGameEditor_Scene();

	virtual bool Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();
};
