#include"Camera_Static.h"
#include"Scene.h"
#include"GameObj.h"

namespace {
	
}

CCamera_Static::CCamera_Static() : CCamera()
{
	m_fAspectRatio = VIEW_ASPECT;		   	  // 縦横比
	m_fFovY = VIEW_ANGLE;					  // 視野角(Degree)
	Init();                                   // 初期化
}

void CCamera_Static::Init()
{
	CCamera::Init();
}

void CCamera_Static::Update()
{
	// ポーズ状態なら更新しない
	if (CScene::GetPause()->GetPauseFlg())return;
	
	if (m_pTargetObj) {
		m_vTarget = m_pTargetObj->GetTransform()->GetPos();
	}

	CCamera::Update();
}