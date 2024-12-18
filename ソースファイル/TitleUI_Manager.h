#pragma once
#include "TitleUI.h"
#include <map>
#include<memory>

const float TITLE_WIDTH = 1120.0f;
const float TITLE_HEIGHT = 960.0f;
const XMFLOAT4 WAKU_COLOR = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);


class CTitleUI_Manager
{
public:
	CTitleUI_Manager();
	~CTitleUI_Manager();

	void Init();
	void Fin();
	void Update();
	void Draw();


	// 画面表示ステート列挙
	enum class TitleUIState {
		Start = 0,
		Menu,
		Option,
		Input,
		Sound,

		MaxState
	};
	static void ChangeState(TitleUIState State);

private:
	static TitleUIState m_CurTitleState;
	static std::shared_ptr<CTitleUI> m_pCurTitleUI;
	static std::map<TitleUIState, std::shared_ptr<CTitleUI>> m_empTitleStateToUI;
};