#pragma once

#include "GameObj.h"



class CCreateObj
{
private:

public:
	CCreateObj();
	~CCreateObj();

	static CGameObj* CreateObj(EObjID eObjID);
	//static CGameObj* CreateObj(EModelType eModelType);
};
