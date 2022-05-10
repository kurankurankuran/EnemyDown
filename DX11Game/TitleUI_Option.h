#pragma once
#include "TitleUI.h"

class CTitleUI_Option : public CTitleUI
{
public:
	CTitleUI_Option();

	void Update();
	void Draw();
	void MoveToTextPosEfk();
	void SetText();
private:
	enum class Option_Select {
		INPUT_SELECT = 0,
		SOUND_SELECT,
		EXIT_SELECT,

		MAX_SELECT
	};

};