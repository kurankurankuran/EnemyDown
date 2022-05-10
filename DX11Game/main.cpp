//=============================================================================
//
// メイン処理 [main.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "main.h"
#include "Model.h"
#include "Scene.h"
#include "Input.h"
#include "Sound.h"
#include "DebugProc.h"
#include "Polygon.h"
#include "Fade.h"
#include "Mesh.h"
#include "Text.h"
#include <crtdbg.h>
#include "TextureData.h"



//-------- ライブラリのリンク
#pragma comment(lib, "winmm")
#pragma comment(lib, "imm32")
#pragma comment(lib, "d3d11")




//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		_T("AppClass")			// ウインドウのクラス名
#define WINDOW_NAME		_T("EnemyDown")		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
HRESULT Init(HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
HWND						g_hWnd;					// メイン ウィンドウ ハンドル
HINSTANCE					g_hInst;				// インスタンス ハンドル

ID3D11Device*				g_pDevice;				// デバイス
ID3D11DeviceContext*		g_pDeviceContext;		// デバイス コンテキスト
IDXGISwapChain*				g_pSwapChain;			// スワップチェーン
ID3D11RenderTargetView*		g_pRenderTargetView;	// フレームバッファ
ID3D11Texture2D*			g_pDepthStencilTexture;	// Zバッファ用メモリ
ID3D11DepthStencilView*		g_pDepthStencilView;	// Zバッファ
UINT						g_uSyncInterval = 0;	// 垂直同期 (0:無, 1:有)
ID3D11RasterizerState*		g_pRs[MAX_CULLMODE];	// ラスタライザ ステート
ID3D11RasterizerState*		g_pRsWire;	            // ラスタライザ ステート(ワイヤ－フレーム)
int                         g_nCurCullMode;         // 現在のカリング設定番号
ID3D11BlendState*			g_pBlendState[MAX_BLENDSTATE];// ブレンド ステート
ID3D11DepthStencilState*	g_pDSS[2];				// Z/ステンシル ステート

int							g_nCountFPS;			// FPSカウンタ

POINT                       g_ptCenter;             // ディスプレイ上のウィンドウ中央座標
POINT                       g_ptClientCenter;       // ディスプレイ上のウィンドウ中央座標のクライアント座標

bool                        g_bWindowActive;        // ウィンドウがアクティブならTrue
unsigned int                g_nFPSRate;             // FPSレート


ID3D11Texture2D*            g_pMiniMapRTex;                 // ミニマップ用テクスチャ
ID3D11ShaderResourceView*   g_pMiniMapRSRV;                 // ミニマップ用シェーダリソース
ID3D11RenderTargetView*     g_pMiniMapRTV;                 // ミニマップ用レンダーターゲット

ID3D11Texture2D*            g_pMiniMapDSTex;               // ミニマップ用デプスステンシルテクスチャ
ID3D11ShaderResourceView*   g_pMiniMapDSSRV;               // ミニマップ用デプスステンシルシェーダリソース
ID3D11DepthStencilView*     g_pMiniMapDSV;                 // ミニマップ用デプスステンシルビュー
//=============================================================================
// メイン関数
//=============================================================================
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);	// 未使用宣言
	UNREFERENCED_PARAMETER(lpCmdLine);		// 未使用宣言

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRM)),
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		nullptr,
		CLASS_NAME,
		nullptr
	};
	MSG msg;
	
	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
		MessageBox(NULL, _T("COMの初期化に失敗しました。"), _T("error"), MB_OK);
		return -1;
	}

	// インスタンス ハンドル保存
	g_hInst = hInstance;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域サイズからウィンドウ サイズ算出
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION
		| WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
	DWORD dwExStyle = 0;
	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// ウィンドウの作成
	g_hWnd = CreateWindowEx(dwExStyle,
		CLASS_NAME,
		WINDOW_NAME,
		dwStyle,
		CW_USEDEFAULT,		// ウィンドウの左座標
		CW_USEDEFAULT,		// ウィンドウの上座標
		rc.right - rc.left,	// ウィンドウ横幅
		rc.bottom - rc.top,	// ウィンドウ縦幅
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	// フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	// 各種ウィンドウ中央座標を初期化
	RECT rect;
	GetWindowRect(g_hWnd, &rect);
	g_ptCenter.x = (rect.right - rect.left) / 2 + rect.left;
	g_ptCenter.y = (rect.bottom - rect.top) / 2 + rect.top;
	g_ptClientCenter = g_ptCenter;
	ScreenToClient(g_hWnd, &g_ptClientCenter);
	
	// FPSレート初期化
	g_nFPSRate = 60;


	// ミニマップ用変数初期化
	g_pMiniMapRTex = nullptr;
	g_pMiniMapRSRV = nullptr;
	g_pMiniMapRTV = nullptr;

	// DirectXの初期化(ウィンドウを作成してから行う)
	if (FAILED(Init(g_hWnd, true))) {
		return -1;
	}

	// メッセージループ
	for (;;) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				// PostQuitMessage()が呼ばれたらループ終了
				break;
			} else {
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			// FPS表示更新
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 500) {	// 0.5秒ごとに実行
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}
			// FPS設定
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / g_nFPSRate)) {
				dwExecLastTime = dwCurrentTime;
				// 更新処理
				Update();
				if (0 < CScene::GetTimeSlowCnt())
				{
					g_nFPSRate = 20;
					CScene::SetTimeSlowCnt(CScene::GetTimeSlowCnt() - 1);
					if (CScene::GetTimeSlowCnt() <= 0)
					{
						g_nFPSRate = 60;
						CScene::SetTimeSlowCnt(0);
					}
				}
			}
			// 描画処理
			Draw();
			dwFrameCount++;
		}
	}

	// タイマ設定を元に戻す
	timeEndPeriod(1);

	// 終了処理
	Uninit();

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, g_hInst);

	// COM終了処理
	CoUninitialize();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:					//----- ウィンドウが生成された
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	case WM_DESTROY:				//----- ウィンドウ破棄指示がきた
		PostQuitMessage(0);				// システムにスレッドの終了を要求
		break;
	case WM_KEYDOWN:				//----- キーボードが押された
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]キーが押された
			PostMessage(hWnd, WM_CLOSE, 0, 0);	// [x]が押されたように振舞う
			return 0;
		}
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16;			// [Alt]+[Enter]時のBEEPを抑止
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// WM_CREATEメッセージハンドラ
//=============================================================================
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	// クライアント領域サイズをSCREEN_WIDTH×SCREEN_HEIGHTに再設定.
	RECT rcClnt;
	GetClientRect(hWnd, &rcClnt);
	rcClnt.right -= rcClnt.left;
	rcClnt.bottom -= rcClnt.top;
	if (rcClnt.right != SCREEN_WIDTH || rcClnt.bottom != SCREEN_HEIGHT) {
		RECT rcWnd;
		GetWindowRect(hWnd, &rcWnd);
		SIZE sizeWnd;
		sizeWnd.cx = (rcWnd.right - rcWnd.left) - rcClnt.right + SCREEN_WIDTH;
		sizeWnd.cy = (rcWnd.bottom - rcWnd.top) - rcClnt.bottom + SCREEN_HEIGHT;
		SetWindowPos(hWnd, nullptr, 0, 0, sizeWnd.cx, sizeWnd.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	// IME無効化
	ImmAssociateContext(hWnd, nullptr);

	return 0;	// -1を返すとCreateWindow[Ex]が失敗する.
}

//=============================================================================
// バックバッファ生成
//=============================================================================
HRESULT CreateBackBuffer(void)
{
	// レンダーターゲットビュー生成
	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	// Zバッファ用テクスチャ生成
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = SCREEN_WIDTH;
	td.Height = SCREEN_HEIGHT;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	HRESULT hr = g_pDevice->CreateTexture2D(&td, nullptr, &g_pDepthStencilTexture);
	if (FAILED(hr)) {
		return hr;
	}

	// Zバッファターゲットビュー生成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilTexture,
		&dsvd, &g_pDepthStencilView);
	if (FAILED(hr)) {
		return hr;
	}

	// 各ターゲットビューをレンダーターゲットに設定
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (float)SCREEN_WIDTH;
	vp.Height = (float)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pDeviceContext->RSSetViewports(1, &vp);




	//****************************************************************
	// ミニマップ用レンダーターゲットビュー作成
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM/*DXGI_FORMAT_R32_FLOAT*/;
	UINT width = 1024; 
	UINT height = 1024;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.BindFlags |= D3D11_BIND_RENDER_TARGET;

	// テクスチャ作成
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = nullptr/*pData*/;
	data.SysMemPitch = desc.Width * 4;
	g_pDevice->CreateTexture2D(&desc, /*pData ? &data :*/ nullptr, &g_pMiniMapRTex);

	// 設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// 生成
	g_pDevice->CreateShaderResourceView(g_pMiniMapRTex, &srvDesc, &g_pMiniMapRSRV);

	// 設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	g_pDevice->CreateRenderTargetView(g_pMiniMapRTex, &rtvDesc, &g_pMiniMapRTV);
	//**********************************************************************
	// デプスステンシルビュー作成
	// テクスチャ設定
	//CreateDepthStencil
	bool useStencil = false;
	format = DXGI_FORMAT_R8G8B8A8_UNORM/*useStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS*/;
	width = 1024;
	height = 1024;
	//MakeTexDesc
	desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	//
	desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	// CreateResource
	// ステンシル使用判定
	useStencil = false;

	// リソース生成
	desc.Format = useStencil ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
	// CreateResource
	// テクスチャ作成
	/*D3D11_SUBRESOURCE_DATA*/ data = {};
	data.pSysMem = nullptr/*pData*/;
	data.SysMemPitch = desc.Width * 4;
	g_pDevice->CreateTexture2D(&desc, /*pData ? &data :*/ nullptr, &g_pMiniMapDSTex);

	// 設定
	/*D3D11_SHADER_RESOURCE_VIEW_DESC */srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// 生成
	g_pDevice->CreateShaderResourceView(g_pMiniMapDSTex, &srvDesc, &g_pMiniMapDSSRV);
	//
	// 設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = useStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	// 生成
	g_pDevice->CreateDepthStencilView(g_pMiniMapRTex, &dsvDesc, &g_pMiniMapDSV);
	//

	return S_OK;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = bWindow;

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &scd,
		&g_pSwapChain, &g_pDevice, nullptr, &g_pDeviceContext);
	if (FAILED(hr)) {
		return hr;
	}

	// バックバッファ生成
	hr = CreateBackBuffer();
	if (FAILED(hr)) {
		return hr;
	}


	// ラスタライズ設定
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;	// カリング無し(両面描画)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[0]);
	rd.CullMode = D3D11_CULL_FRONT;	// 前面カリング(裏面描画)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[1]);
	rd.CullMode = D3D11_CULL_BACK;	// 背面カリング(表面描画)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[2]);
	g_pDeviceContext->RSSetState(g_pRs[2]);
	g_nCurCullMode = 2;

	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	hr = g_pDevice->CreateRasterizerState(&rd, &g_pRsWire);
	if (FAILED(hr)) { return hr; }

	// ブレンド ステート生成
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = FALSE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[0]);
	// ブレンド ステート生成 (アルファ ブレンド用)
	//BlendDesc.AlphaToCoverageEnable = TRUE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[1]);
	// ブレンド ステート生成 (加算合成用)
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[2]);
	// ブレンド ステート生成 (減算合成用)
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[3]);
	SetBlendState(BS_ALPHABLEND);

	// 深度ステンシルステート生成
	CD3D11_DEFAULT def;
	CD3D11_DEPTH_STENCIL_DESC dsd(def);
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	g_pDevice->CreateDepthStencilState(&dsd, &g_pDSS[0]);
	CD3D11_DEPTH_STENCIL_DESC dsd2(def);
	dsd2.DepthEnable = FALSE;
	g_pDevice->CreateDepthStencilState(&dsd2, &g_pDSS[1]);

	// ポリゴン表示初期化
	hr = CPolygon::Init(g_pDevice);
	if (FAILED(hr))
		return hr;

#ifdef _DEBUG
	// デバッグ文字列表示初期化
	hr = CDebugProc::Init();
	if (FAILED(hr))
		return hr;
#endif // _DEBUG

	

	// メッシュ用シェーダ初期化
	hr = CMesh::InitShader();
	if (FAILED(hr))
		return hr;
	// ミニマップ用シェーダ初期化
	hr = CBox::InitShader();
	if (FAILED(hr))
		return hr;

	// 入力処理初期化
	hr = CInput::Init();
	if (FAILED(hr))
		return hr;

	// UIテキスト処理初期化
	hr = CText::Init();
	
	// モデル初期化
	hr = CModel::LoadAll();
	if (FAILED(hr))
		return hr;

	// サウンド初期化
	CSound::Init();

	// フェード初期化
	CFade::Init();

	//// 爆発エフェクト初期化
	//hr = CExplosion::InitShader();
	//if (FAILED(hr))
	//	return hr;

	// シーン初期化
	if (!CScene::InitAll())
		return E_FAIL;

	return hr;
}

//=============================================================================
// バックバッファ解放
//=============================================================================
void ReleaseBackBuffer()
{

	if (g_pDeviceContext) {
		g_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	}



	SAFE_RELEASE(g_pMiniMapDSSRV);
	SAFE_RELEASE(g_pMiniMapDSTex);
	SAFE_RELEASE(g_pMiniMapDSV);
	SAFE_RELEASE(g_pMiniMapRSRV);
	SAFE_RELEASE(g_pMiniMapRTex);
	SAFE_RELEASE(g_pMiniMapRTV);



	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pDepthStencilTexture);
	SAFE_RELEASE(g_pRenderTargetView);
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// シーン終了処理
	CScene::FinAll();

	// 爆発エフェクト終了処理
	//CExplosion::FinShader();

	// サウンド終了処理
	CSound::Fin();

	// モデル解放処理
	CModel::ReleaseAll();

	// UIテキスト終了処理
	CText::Uninit();
	
	// 入力処理終了処理
	CInput::Fin();

	// ミニマップ用シェーダ終了処理
	CBox::FinShader();
	// メッシュ用シェーダ終了処理
	CMesh::FinShader();

	// テクスチャ解放処理
	CTextureData::ReleaseDataAll();

#ifdef _DEBUG
	// デバッグ文字列表示終了処理
	CDebugProc::Fin();
#endif

	// ポリゴン表示終了処理
	CPolygon::Fin();

	// 深度ステンシルステート解放
	for (int i = 0; i < _countof(g_pDSS); ++i) {
		SAFE_RELEASE(g_pDSS[i]);
	}

	// ブレンド ステート解放
	for (int i = 0; i < MAX_BLENDSTATE; ++i) {
		SAFE_RELEASE(g_pBlendState[i]);
	}

	// ラスタライザ ステート解放
	for (int i = 0; i < MAX_CULLMODE; ++i) {
		SAFE_RELEASE(g_pRs[i]);
	}

	

	// バックバッファ解放
	ReleaseBackBuffer();

	// スワップチェーン解放
	SAFE_RELEASE(g_pSwapChain);

	// デバイス コンテキストの開放
	SAFE_RELEASE(g_pDeviceContext);

	// デバイスの開放
	SAFE_RELEASE(g_pDevice);
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// ウィンドウのアクティブ状態を判断
	HWND hWnd;
	hWnd = GetActiveWindow();
	if (g_hWnd == hWnd) g_bWindowActive = true;
	else g_bWindowActive = false;


	// ウィンドウの各種中央座標を更新
	RECT rect;
	GetWindowRect(g_hWnd, &rect);
	g_ptCenter.x = (rect.right - rect.left) / 2 + rect.left;
	g_ptCenter.y = (rect.bottom - rect.top) / 2 + rect.top;
	g_ptClientCenter = g_ptCenter;
	ScreenToClient(g_hWnd, &g_ptClientCenter);

	if (g_bWindowActive) {
		// 入力処理更新
		CInput::Update();	// 必ずUpdate関数の先頭で実行.
	}

#ifdef _DEBUG
	// カリング切替
	if (CInput::GetKeyTrigger(VK_F1))
	{
		SetCullMode(0);// カリング無し(両面描画)
	}
	else if (CInput::GetKeyTrigger(VK_F2))
	{
		SetCullMode(1);// 前面カリング(裏面描画)
	}
	else if (CInput::GetKeyTrigger(VK_F3))
	{
		SetCullMode(2);// 背面カリング(表面描画)
	}
	else if (CInput::GetKeyTrigger(VK_F4))
	{
		WireFrameDraw();// ワイヤーフレーム
	}



	// デバッグ文字列設定
	CDebugProc::Start();
	CDebugProc::Print("FPS:%d\n\n", g_nCountFPS);
#endif

	

	// シーン更新
	CScene::UpdateAll();

	// UIテキスト更新
	CText::Update();

	// フェード更新
	CFade::Update();

	// サウンド更新
	CSound::Update();	// 必ずUpdate関数の末尾で実行.
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	
	// シーン描画
	CScene::DrawAll();

	// UIテキスト描画
	CText::Draw();

	// フェード描画
	CFade::Draw();

	


#ifdef _DEBUG
	// デバッグ文字列表示
	CDebugProc::Draw();
#endif
	// バックバッファとフロントバッファの入れ替え
	g_pSwapChain->Present(g_uSyncInterval, 0);
}

//=============================================================================
// メイン ウィンドウ ハンドル取得
//=============================================================================
HWND GetMainWnd()
{
	return g_hWnd;
}

//=============================================================================
// インスタンス ハンドル取得
//=============================================================================
HINSTANCE GetInstance()
{
	return g_hInst;
}

//=============================================================================
// デバイス取得
//=============================================================================
ID3D11Device* GetDevice()
{
	return g_pDevice;
}

//=============================================================================
// デバイス コンテキスト取得
//=============================================================================
ID3D11DeviceContext* GetDeviceContext()
{
	return g_pDeviceContext;
}

//=============================================================================
// 深度バッファ有効無効制御
//=============================================================================
void SetZBuffer(bool bEnable)
{
	g_pDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[1], 0);
}

//=============================================================================
// 深度バッファ更新有効無効制御
//=============================================================================
void SetZWrite(bool bEnable)
{
	g_pDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[0], 0);
}

//=============================================================================
// ブレンド ステート設定
//=============================================================================
void SetBlendState(int nBlendState)
{
	if (nBlendState >= 0 && nBlendState < MAX_BLENDSTATE) {
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		g_pDeviceContext->OMSetBlendState(g_pBlendState[nBlendState], blendFactor, 0xffffffff);
	}
}

//=============================================================================
// カリング設定
//=============================================================================
void SetCullMode(int nCullMode)
{
	if (nCullMode >= 0 && nCullMode < MAX_CULLMODE) {
		g_pDeviceContext->RSSetState(g_pRs[nCullMode]);
		g_nCurCullMode = nCullMode;
	}
}


//=============================================================================
// ワイヤーフレーム設定
//=============================================================================
void WireFrameDraw()
{
	g_pDeviceContext->RSSetState(g_pRsWire);
	g_nCurCullMode = 3;
}

//=============================================================================
// カリング設定番号取得
//=============================================================================
int GetCurCullMode()
{
	return g_nCurCullMode;
}

//=============================================================================
// ディスプレイ上のウィンドウ中央座標取得
//=============================================================================
POINT GetScreenCenter()
{
	return g_ptCenter;
}

//=============================================================================
// ディスプレイ上のウィンドウ中央座標のクライアント座標取得
//=============================================================================
POINT GetScreenCenterClient()
{
	return g_ptClientCenter;
}

//=============================================================================
// ウィンドウのアクティブ状態を取得
//=============================================================================
bool GetWindowActive()
{
	return g_bWindowActive;
}

void SetDefaultRender()
{
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (float)SCREEN_WIDTH;
	vp.Height = (float)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pDeviceContext->RSSetViewports(1, &vp);
}

void SetShadowRender()
{
	g_pDeviceContext->OMSetRenderTargets(1, &g_pMiniMapRTV, g_pMiniMapDSV);
	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (float)1024;
	vp.Height = (float)1024;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pDeviceContext->RSSetViewports(1, &vp);
}

