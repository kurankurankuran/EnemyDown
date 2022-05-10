#include"Camera_Static.h"
#include"Scene.h"
#include"GameObj.h"

namespace {
	
}

CCamera_Static::CCamera_Static() : CCamera()
{
	m_fAspectRatio = VIEW_ASPECT;		   	  // �c����
	m_fFovY = VIEW_ANGLE;					  // ����p(Degree)
	Init();                                   // ������
}

void CCamera_Static::Init()
{
	CCamera::Init();
}

void CCamera_Static::Update()
{
	// �|�[�Y��ԂȂ�X�V���Ȃ�
	if (CScene::GetPause()->GetPauseFlg())return;
	
	if (m_pTargetObj) {
		m_vTarget = m_pTargetObj->GetTransform()->GetPos();
	}

	CCamera::Update();
}