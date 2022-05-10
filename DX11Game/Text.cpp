#include "Text.h"
#include "Texture.h"
#include "Polygon.h"

// 静的メンバ
ID3D11ShaderResourceView*	                                                    CText::m_pTextureText;
unordered_map<int/*テキストインデックス*/, TextParam/*テキストパラメータ*/>     CText::m_TextMap;
vector<bool/*インデックス番号使用中フラグ*/>                                    CText::m_TextIndexVector;
unordered_map<char/*入力文字*/, XMFLOAT2/*UV*/>                                 CText::m_UVMap;
CCamera*                                                                        CText::m_pCamera;

// 定数
namespace {
	const int     MAX_TEXT = 100;// 画面上に同時に描画可能なテキスト数（インデックスの管理上限数）
	const LPCWSTR g_pszPathTexText = L"data/texture/eisuuziTip.png";

	const int     TEXT_NUM_X = 26;
	const int     TEXT_NUM_Y = 3;
}

//********************
// コンストラクタ
//********************
CText::CText()
{
	m_pTextureText = nullptr;
	m_pCamera = nullptr;
}

//********************
// デストラクタ　
//********************
CText::~CText()
{

}

//********************
// テキスト初期化
//********************
HRESULT CText::Init()
{
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();

	// 文字テクスチャの一文字ごとのUVを設定
	UVSet();

	// テキストパラメータクリア
	m_TextMap.clear();

	// インデックス初期化
	for (int i = 0; i < MAX_TEXT; ++i) {
		m_TextIndexVector.push_back(false);
	}
	
	// テクスチャ読み込み
	// 英数字用
	hr = CreateTextureFromFile(
		pDevice,					   // デバイスへのポインタ
		g_pszPathTexText,		       // ファイルの名前
		&m_pTextureText);			   // 読み込むメモリー
	if (FAILED(hr)) {
		Uninit();
		return false;
	}

	return hr;
}

//********************
// テキスト終了処理
//********************
void CText::Uninit()
{
	// テクスチャ解放
	SAFE_RELEASE(m_pTextureText);
	// インデックスクリア
	m_TextIndexVector.clear();
	// テキストクリア
	m_TextMap.clear();
	// UVクリア
	m_UVMap.clear();
}

//********************
// テキスト更新
//********************
void CText::Update()
{
	// カウントが0より大きいパラメータ全てのカウント更新
	for (auto itr = m_TextMap.begin(); itr != m_TextMap.end();)
	{
		if (0 < itr->second.nCnt) {
			--itr->second.nCnt;
			if (itr->second.nCnt == 0)
			{
				// インデックス未使用に変更
				m_TextIndexVector[itr->first] = false;
				// テキスト削除
				itr = m_TextMap.erase(itr);
				continue;
			}

			if (m_pCamera) 
			{
				// ３D空間用テキストはスクリーン上の位置を更新
				if (itr->second.bSolid)
				{
					// 3D→2D変換
					XMMATRIX mProj = XMLoadFloat4x4(&m_pCamera->GetProjMatrix());
					XMMATRIX mView = XMLoadFloat4x4(&m_pCamera->GetViewMatrix());
					XMMATRIX mWorld = XMMatrixIdentity();
					XMVECTOR vecPos = XMVector3Project(
						XMLoadFloat3(&itr->second.vPosSolid),
						0.0f, 0.0f,
						SCREEN_WIDTH, SCREEN_HEIGHT,
						0.0f, 1.0f,
						mProj,
						mView,
						mWorld);

					XMFLOAT3 vScreenPos;
					XMStoreFloat3(&vScreenPos, vecPos);
					vScreenPos.x -= SCREEN_CENTER_X;
					vScreenPos.y -= SCREEN_CENTER_Y;

					itr->second.vPos.x = vScreenPos.x;
					itr->second.vPos.y = -vScreenPos.y;

					// 描画フラグ更新
					if (vScreenPos.z <= 1.0f)
					{
						itr->second.bDraw = true;
					}
					else
					{
						// 画面外
						itr->second.bDraw = false;
					}

				}
			}
		}
		++itr;
	}
}

//********************
// テキスト描画
//********************
void CText::Draw()
{
	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
	SetZWrite(false);				// Zバッファ更新しない

	ID3D11DeviceContext* pDC = GetDeviceContext();

	// Map全て精査
	for (auto itr = m_TextMap.begin(); itr != m_TextMap.end();)
	{
		// 描画フラグ
		if (itr->second.bDraw)
		{
			// 一文字ずつ描画
			for (unsigned int i = 0; i < itr->second.text.size(); ++i)
			{
				CPolygon::SetTexture(m_pTextureText);
				CPolygon::SetPos(itr->second.vPos.x + i * itr->second.vScale.x, itr->second.vPos.y);
				CPolygon::SetSize(itr->second.vScale.x, itr->second.vScale.y);
				CPolygon::SetColor(itr->second.vColor.x, itr->second.vColor.y, itr->second.vColor.z);
				CPolygon::SetAlpha(itr->second.vColor.w);
				CPolygon::SetFrameSize(1.0f / TEXT_NUM_X, 1.0f / TEXT_NUM_Y);
				XMFLOAT2 UV = XMFLOAT2(m_UVMap[itr->second.text[i]].x, m_UVMap[itr->second.text[i]].y);
				CPolygon::SetUV(m_UVMap[itr->second.text[i]].x, m_UVMap[itr->second.text[i]].y);
				CPolygon::Draw(pDC);
			}
		}
		++itr;
	}

	SetZWrite(true);							// Zバッファ更新する
	SetBlendState(BS_NONE);		                // アルファブレンド無効
}

//********************
// テキスト格納(2D用)
//********************
int CText::OutPutText(std::string text, XMFLOAT2 vPos, XMFLOAT2 vScale, XMFLOAT4 vColor, int nCnt)
{
	int nIndex = -1;
	for (int i = 0; i < MAX_TEXT; ++i)
	{
		if (!m_TextIndexVector[i])
		{
			nIndex = i;
			m_TextIndexVector[i] = true;
			break;
		}
	}
	// 全て使用済み
	if (nIndex < 0)return nIndex;

	TextParam _TextParam;
	_TextParam.bDraw = true;
	_TextParam.text = text;
	_TextParam.vPos = vPos;
	_TextParam.vScale = vScale;
	_TextParam.vColor = vColor;
	_TextParam.nCnt = nCnt;
	_TextParam.bSolid = false;
	_TextParam.vPosSolid = XMFLOAT3(0, 0, 0);// 使用しないが一応初期化
	m_TextMap[nIndex] = _TextParam;

	return nIndex;
}

//********************
// テキスト格納(3D空間UI用)
//********************
int CText::OutPutText(std::string text, XMFLOAT3 vPosSolid, XMFLOAT2 vScale, XMFLOAT4 vColor, int nCnt)
{
	int nIndex = -1;
	for (int i = 0; i < MAX_TEXT; ++i)
	{
		if (!m_TextIndexVector[i])
		{
			nIndex = i;
			m_TextIndexVector[i] = true;
			break;
		}
	}
	// 全て使用済み
	if (nIndex < 0)return nIndex;

	// 3D→2D変換
	XMMATRIX mProj = XMLoadFloat4x4(&m_pCamera->GetProjMatrix());
	XMMATRIX mView = XMLoadFloat4x4(&m_pCamera->GetViewMatrix());
	XMMATRIX mWorld = XMMatrixIdentity();
	XMVECTOR vecPos = XMVector3Project(
		XMLoadFloat3(&vPosSolid),
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		0.0f, 1.0f,
		mProj,
		mView,
		mWorld);

	XMFLOAT3 vScreenPos;
	XMStoreFloat3(&vScreenPos, vecPos);
	vScreenPos.x -= SCREEN_CENTER_X;
	vScreenPos.y -= SCREEN_CENTER_Y;

	TextParam _TextParam;
	_TextParam.bDraw = true;
	_TextParam.text = text;
	_TextParam.vPos = XMFLOAT2(vScreenPos.x, -vScreenPos.y);
	_TextParam.vScale = vScale;
	_TextParam.vColor = vColor;
	_TextParam.nCnt = nCnt;
	_TextParam.bSolid = true;
	_TextParam.vPosSolid = vPosSolid;

	m_TextMap[nIndex] = _TextParam;

	return nIndex;
}

//********************
// テキスト削除
//********************
int CText::DeleteText(int nIndex)
{
	if (nIndex < 0)return nIndex;
	auto itr = m_TextMap.find(nIndex);
	if (itr != m_TextMap.end())         // キーの要素が存在している場合
	{
		m_TextMap.erase(itr);
		m_TextIndexVector[nIndex] = false;

		return -1;
	}
	return nIndex;
}

void CText::DeleteTextAll()
{
	// インデックスクリア
	for (int i = 0; i < MAX_TEXT; ++i)
	{
    	m_TextIndexVector[i] = false;	
	}
	
	// テキストクリア
	m_TextMap.clear();
}

void CText::SetPos(int nIndex, XMFLOAT2 vPos)
{
	m_TextMap[nIndex].vPos = vPos;
}

void CText::SetScale(int nIndex, XMFLOAT2 vScale)
{
	m_TextMap[nIndex].vScale = vScale;
}

void CText::SetColor(int nIndex, XMFLOAT4 vColor)
{
	m_TextMap[nIndex].vColor = vColor;
}

//********************
// UVセッティング
//********************
void CText::UVSet()
{
	int j = 0;
	for (int i = 0; i < TEXT_NUM_X; ++i) {
		m_UVMap[('a' + i)] = XMFLOAT2((1.0f / TEXT_NUM_X) * i, (j * TEXT_NUM_X) / (float)(TEXT_NUM_X * TEXT_NUM_Y));
	}

	j++;
	for (int i = 0; i < TEXT_NUM_X; ++i)
	{
		m_UVMap[('A' + i)] = XMFLOAT2((1.0f / TEXT_NUM_X) * i, (j * TEXT_NUM_X) / (float)(TEXT_NUM_X * TEXT_NUM_Y));
	}

	j++;
	for (int i = 0; i < TEXT_NUM_X; ++i)
	{
		char test;
		test = (char)('0' + i - 16);
		m_UVMap[('0' + i - 16)] = XMFLOAT2((1.0f / TEXT_NUM_X) * i, (j * TEXT_NUM_X) / (float)(TEXT_NUM_X * TEXT_NUM_Y));
	}
	
	int test;
	test = 0;
}

