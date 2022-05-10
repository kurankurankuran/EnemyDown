#pragma once
#include "TitleUI.h"

class CTitleUI_Sound : public CTitleUI
{
public:
	CTitleUI_Sound();

	void Update();
	void Draw();
	void MoveToTextPosEfk();
	void SetText();
private:

};