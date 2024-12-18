//=============================================================================
//
// エディター操作基底インスタンス クラス定義 [ES_Base.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
							
class CES_Base
{
public:
	CES_Base();
	~CES_Base();

	virtual void Init();
	virtual void Fin();
	virtual void Update() = 0;
	virtual void Draw() = 0;
private:

};		