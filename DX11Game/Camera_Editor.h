#pragma once
#include"Camera.h"

class CCamera_Editor : public CCamera
{
public:
	CCamera_Editor();

	void Init();
	void Fin();
	void Update();

	bool GetMoveFlag() { return m_bMove; }
private:
	float m_fSpeed;
	float m_fPosToTargetLength;
	static bool m_bMove;
	//int m_nEditorReticleIndex;
};
