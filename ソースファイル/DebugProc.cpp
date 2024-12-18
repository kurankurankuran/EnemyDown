//=============================================================================
//
// デバッグ表示クラス実装 [DebugProc.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include <stdarg.h>
#include <string.h>
#include "DebugProc.h"
#include "Polygon.h"
#include "Texture.h"

// マクロ定義
#define	TEXTURE_FILENAME	L"data/texture/PressStart2Ph_e.tga"
#define FONT_WIDTH			16
#define FONT_HEIGHT			16

// 静的メンバ
ID3D11ShaderResourceView* CDebugProc::m_pTexture;	// テクスチャへのポインタ
char CDebugProc::m_szDebug[8192];					// デバッグ情報
bool CDebugProc::m_bHiragana = false;				// 平仮名フラグ

// 初期化
HRESULT CDebugProc::Init()
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	// テクスチャの読み込み
	hr = CreateTextureFromFile(pDevice, TEXTURE_FILENAME, &m_pTexture);

	return hr;
}

// 終了処理
void CDebugProc::Fin()
{
	// テクスチャ解法
	SAFE_RELEASE(m_pTexture);
}

// 描画
void CDebugProc::Draw()
{
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMFLOAT2 vPos(SCREEN_WIDTH * -0.5f + FONT_WIDTH * 0.5f,
		SCREEN_HEIGHT * 0.5f - FONT_HEIGHT * 0.5f);
	CPolygon::SetTexture(m_pTexture);
	CPolygon::SetFrameSize(8.0f / 128.0f, 8.0f / 128.0f);
	CPolygon::SetSize(FONT_WIDTH, FONT_HEIGHT);
	for (char* pChr = &m_szDebug[0]; *pChr; ++pChr) {
		if (*pChr == '\n') {
			vPos.x = SCREEN_WIDTH * -0.5f + FONT_WIDTH * 0.5f;
			vPos.y -= FONT_HEIGHT;
			continue;
		}
		int nChr = (BYTE)*pChr;
		if (nChr == 0x0084) {
			nChr = (BYTE)*(pChr + 1);
			if (nChr >= 0x00c0 && nChr <= 0x00cf) {
				const XMFLOAT4 vColor[] = {
					XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
					XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f),
					XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f),
					XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f),
					XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f),
					XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f),
					XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f),
					XMFLOAT4(0.752941f, 0.752941f, 0.752941f, 1.0f),
					XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
					XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
					XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
					XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),
					XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
					XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f),
					XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f),
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				};
				CPolygon::SetColor(vColor[nChr & 0x0f]);
				++pChr;
				continue;
			}
		}
		CPolygon::SetPos(vPos.x, vPos.y);
		if (m_bHiragana &&
			(nChr >= 0xa6 && nChr <= 0xaf || nChr >= 0xb1 && nChr <= 0xdd))
			nChr ^= 0x20;
		CPolygon::SetUV((nChr & 15) / 16.0f, (nChr >> 4) / 16.0f);
		CPolygon::Draw(pDeviceContext);
		vPos.x += FONT_WIDTH;
	}
	// テクスチャ設定を元に戻す
	CPolygon::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	CPolygon::SetUV(0.0f, 0.0f);
	CPolygon::SetFrameSize(1.0f, 1.0f);

	SetBlendState(BS_NONE);
	SetZBuffer(true);
}

// 登録開始
void CDebugProc::Start(bool hiragana)
{
	SetHiragana(hiragana);
	m_szDebug[0] = '\0';
}

// 登録
int CDebugProc::Print(const char *fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	char aBuf[256];
	int nLen = _vsprintf_p(aBuf, sizeof(aBuf), fmt, list);
	va_end(list);

	// 連結
	if ((strlen(m_szDebug) + strlen(aBuf)) < sizeof(m_szDebug) - 1) {
		strcat_s(m_szDebug, sizeof(m_szDebug), aBuf);
	}

	return nLen;
}

// 平仮名設定
bool CDebugProc::SetHiragana(bool hiragana)
{
	bool prev = m_bHiragana;
	m_bHiragana = hiragana;
	return prev;
}
