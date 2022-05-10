#include "TitleUI.h"
#include "Text.h"
#include "Scene.h"
#include "EffekseerManager.h"

// 静的メンバ
std::vector<int>  CTitleUI::m_DrawTextIndex;
int CTitleUI::m_nEfkHandle;

void CTitleUI::Init()
{
	// テキスト表示
	SetText();
	// 選択中光球生成
	m_nEfkHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_PLAYER_EFK), true, TIMER_MACHINGUN_GENERATER_PLAYER_EFK);
	// 光球位置変更
	MoveToTextPosEfk();
}

void CTitleUI::Fin()
{
	// 光球削除
	CEffekseerManager::Stop(m_nEfkHandle, true);
	// 現在のテキスト削除
	DeleteText();
}

void CTitleUI::MoveSelect()
{
	// 選択肢変更
	if (CInput::GetKeyTrigger(VK_W))
	{
		// カーソル移動サウンド
		CSound::Play(SE_MENUE);
		// カーソル箇所変更
		m_nCurSelect = (int)(m_nCurSelect - 1);
		if (m_nCurSelect < 0)
		{
			m_nCurSelect = 0;
		}
		// 光球位置変更
		MoveToTextPosEfk();
	}
	else if (CInput::GetKeyTrigger(VK_S))
	{
		// カーソル移動サウンド
		CSound::Play(SE_MENUE);
		// カーソル箇所変更
		m_nCurSelect = (int)(m_nCurSelect + 1);
		if (m_nMaxSelect <= m_nCurSelect)
		{
			m_nCurSelect = m_nMaxSelect - 1;
		}
		// 光球位置変更
		MoveToTextPosEfk();
	}
}

void CTitleUI::DeleteText()
{
	for (auto itr = m_DrawTextIndex.begin(); itr != m_DrawTextIndex.end();)
	{
		CText::DeleteText(*itr);
		++itr;
	}
	m_DrawTextIndex.clear();
}

void CTitleUI::SetEfkPos(XMFLOAT2 vPos)
{
	// 光源位置
	XMFLOAT2 EfkScreenPos(vPos.x, vPos.y/*850.0f, 625.0f*/);
	XMFLOAT3 EfkPos;
	CalcScreenToWorld(
		&EfkPos,
		EfkScreenPos.x, EfkScreenPos.y, 0.997f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		&CScene::GetCurrent()->GetCameraManager()->GetCamera()->GetViewMatrix(),
		&CScene::GetCurrent()->GetCameraManager()->GetCamera()->GetProjMatrix());
	XMFLOAT4X4 EfkMat;
	XMStoreFloat4x4(&EfkMat, XMMatrixIdentity());

	XMMATRIX mtxScl;
	mtxScl = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	XMStoreFloat4x4(&EfkMat, XMMatrixMultiply(XMLoadFloat4x4(&EfkMat), mtxScl));

	EfkMat._41 = EfkPos.x;
	EfkMat._42 = EfkPos.y;
	EfkMat._43 = EfkPos.z;

	CEffekseerManager::SetMatrix(m_nEfkHandle, true, EfkMat);
}


// スクリーン座標をワールド座標に変換
XMFLOAT3* CTitleUI::CalcScreenToWorld(
	XMFLOAT3* pout,
	float Sx,  // スクリーンX座標
	float Sy,  // スクリーンY座標
	float fZ,  // 射影空間でのZ値（0～1）
	float Screen_w,
	float Screen_h,
	XMFLOAT4X4* View,
	XMFLOAT4X4* Prj
) {
	// 各行列の逆行列を算出
	XMMATRIX InvView, InvPrj, InvViewport;
	InvView = XMMatrixInverse(nullptr, XMLoadFloat4x4(View));
	InvPrj = XMMatrixInverse(nullptr, XMLoadFloat4x4(Prj));
	XMFLOAT4X4 VP;
	XMStoreFloat4x4(&VP, XMMatrixIdentity());
	VP._11 = Screen_w / 2.0f; VP._22 = -Screen_h / 2.0f;
	VP._41 = Screen_w / 2.0f; VP._42 = Screen_h / 2.0f;
	InvViewport = XMMatrixInverse(nullptr, XMLoadFloat4x4(&VP));

	// 逆変換
	XMMATRIX tmp = InvViewport * InvPrj * InvView;
	XMStoreFloat3(pout, XMVector3TransformCoord(XMVectorSet(Sx, Sy, fZ, 1.0f), tmp));

	return pout;
}
