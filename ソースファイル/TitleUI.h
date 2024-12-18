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
	// �X�N���[�����W�����[���h���W�ɕϊ�
	static XMFLOAT3* CalcScreenToWorld(
		XMFLOAT3* pout,
		float Sx,  // �X�N���[��X���W
		float Sy,  // �X�N���[��Y���W
		float fZ,  // �ˉe��Ԃł�Z�l�i0�`1�j
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
