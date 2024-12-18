#pragma once
#include "main.h"
#include <string>
#include <map>
#include <unordered_map>  
#include <vector>
#include "Camera.h"
using namespace std;         

typedef struct {
	bool     bDraw;       // 描画するフラグ
	string   text;        // 描画する文字列
	XMFLOAT2 vPos;        // スクリーン上に描画する位置
	XMFLOAT2 vScale;      // 大きさ
	XMFLOAT4 vColor;      // 色（α含む）
	int      nCnt;        // 描画時間(デフォルトで永続)
	bool     bSolid;      // 3D空間上に描画するか
	XMFLOAT3 vPosSolid;   // 3D空間上の位置
}TextParam;

class CText
{
private:
	// 英数字テクスチャ
	static ID3D11ShaderResourceView*	m_pTextureText;
	static unordered_map<int/*テキストインデックス*/, TextParam/*テキストパラメータ*/> m_TextMap;
	static vector<bool/*インデックス番号使用中フラグ*/> m_TextIndexVector;

	// 文字ごとのUV格納用
	static unordered_map<char/*入力文字*/, XMFLOAT2/*UV*/> m_UVMap;

	static CCamera* m_pCamera;
public:
	CText();
	~CText();

	static HRESULT Init();
	static void Uninit();
	static void Update();
	static void Draw();
	
	// UI用
	static int OutPutText(std::string text, XMFLOAT2 vPos, XMFLOAT2 vScale, XMFLOAT4 vColor = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), int nCnt = -1);
	// 3D空間UI用
	static int OutPutText(std::string text, XMFLOAT3 vPosSolid, XMFLOAT2 vScale, XMFLOAT4 vColor = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), int nCnt = -1);
	static int DeleteText(int nIndex);
	static void DeleteTextAll();

	static void SetPos(int nIndex, XMFLOAT2 vPos);
	static void SetScale(int nIndex, XMFLOAT2 vScale);
	static void SetColor(int nIndex, XMFLOAT4 vColor);


	static void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
private:
	static void UVSet();
};
