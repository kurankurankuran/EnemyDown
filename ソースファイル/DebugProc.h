//=============================================================================
//
// デバッグ表示クラス定義 [DebugProc.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "main.h"

class CDebugProc
{
private:
	static ID3D11ShaderResourceView* m_pTexture;	// テクスチャへのポインタ
	static char m_szDebug[8192];					// デバッグ情報
	static bool m_bHiragana;						// 平仮名フラグ

public:
	static HRESULT Init();
	static void Fin();
	static void Draw();

	static void Start(bool hiragana = false);
	static int Print(const char *fmt, ...);
	static bool SetHiragana(bool hiragana = true);
};
