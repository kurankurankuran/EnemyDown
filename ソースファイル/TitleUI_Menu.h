#pragma once
#include "TitleUI.h"

class CTitleUI_Menu : public CTitleUI
{
public:
	CTitleUI_Menu();

	void Update();
	void Draw();
	void MoveToTextPosEfk();
	void SetText();
private:
	enum class Menu_Select{
		STAGESELECT_SELECT = 0,
		OPTION_SELECT,

		MAX_SELECT
	};
};