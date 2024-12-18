#pragma once
#include "main.h"
#include <vector>

class CTitleUI
{
public:
	virtual void Init();
	virtual void Fin();
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void MoveToTextPosEfk() = 0;
	virtual void SetText() = 0;
	void MoveSelect();
	static void DeleteText();
	static void SetEfkPos(XMFLOAT2 vPos);
	// スクリーン座標をワールド座標に変換
	static XMFLOAT3* CalcScreenToWorld(
		XMFLOAT3* pout,
		float Sx,  // スクリーンX座標
		float Sy,  // スクリーンY座標
		float fZ,  // 射影空間でのZ値（0～1）
		float Screen_w,
		float Screen_h,
		XMFLOAT4X4* View,
		XMFLOAT4X4* Prj
	);

protected:
	static std::vector<int> m_DrawTextIndex;
	static int m_nEfkHandle;
	int m_nCurSelect;
	int m_nMaxSelect;
private:
};
