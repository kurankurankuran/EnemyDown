#pragma once
#include "TitleUI.h"

class CTitleUI_Input : public CTitleUI
{
public:
	CTitleUI_Input();

	void Update();
	void Draw();
	void MoveToTextPosEfk();
	void SetText();

private:

};