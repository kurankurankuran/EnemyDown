////=============================================================================
////
//// ステージセレクト処理 [StageSelect.cpp]
//// Author : KAZUKI KURA
////
////=============================================================================
//
//#include "main.h"
//#include "Texture.h"
//#include "StageSelect.h"
//#include "input.h"
//#include "fade.h"
//#include "scene.h"
//#include "polygon.h"
//#include "debugproc.h"
//#include "Sound.h"
////#include "EnemyManager_StageSelect.h"
//#include "EffectManager.h"
//#include "EffekseerManager.h"
//
////*****************************************************************************
//// マクロ定義
////*****************************************************************************
//
//namespace {
//	const int SCALE_RATE = 1;             // 拡縮速度
//	const float COLOR_RATE = 0.01f;       // 色変化速度
//	const float SELECT_INTERVAL = 100.0f; // 枠間の距離間隔
//	const float SELECT_SIZE_X = 180.0f;   // 画像の横サイズ
//	const float SELECT_SIZE_Y = 300.0f;   // 画像の縦サイズ
//	const float FRAME_WIDTH = 20.0f;      // 枠の幅サイズ
//
//	const int MAX_SCALE_UP = 50;       // 膨張時最大
//	const int MIN_SCALE_DOWN = -10;    // 縮小時最小
//
//	//const LPCWSTR g_FileNameBG = L"data/texture/TitleName.png";
//	const LPCWSTR g_FileNameStage[MAX_STAGE_SELECT] =
//	{
//		L"data/texture/SELECT_TUTORIAL.png",		// ステージ１
//		L"data/texture/SELECT_STAGE_1.png",		// ステージ２
//		L"data/texture/SELECT_STAGE_2.png",		// ステージ３
//		L"data/texture/SELECT_STAGE_3.png",		// ステージ３
//		//L"data/texture/TitleName.png",		// ステージ３
//		//L"data/texture/TitleName.png",		// ステージ３
//	};
//}
//
//
////*****************************************************************************
//// グローバル変数
////*****************************************************************************
//
//CStageSelect::CStageSelect()
//{
//	m_id = SCENE_SELECT;
//
//	m_nCurrentSelect = TUTORIAL;
//	/*m_bInputUpNow = false;
//	m_bInputUpOld = false;
//	m_bInputDownNow = false;
//	m_bInputDownOld = false;*/
//
//	//m_pTexBG = nullptr;
//
//	for (int i = 0; i < MAX_STAGE_SELECT; ++i) {
//		m_pTexStage[i] = nullptr;
//	}
//}
//
//CStageSelect::~CStageSelect()
//{
//
//}
//
////=============================================================================
//// ステージセレクトの初期化処理
////=============================================================================
//bool CStageSelect::Init()
//{
//	if(!CScene::Init()) {
//		return false;
//	}
//
//	m_nSclCnt = 0; // 拡縮タイマー
//	m_bScaleUp = true; // 拡大中フラグ
//	m_vColor = XMFLOAT3(1.0f, 0.0f, 0.0f); // 枠色
//
//
//	HRESULT hr = S_OK;
//	ID3D11Device* pDevice = GetDevice();
//
//	// テクスチャ読み込み
//	/*hr = CreateTextureFromFile(pDevice, g_FileNameBG, &m_pTexBG);
//	if (FAILED(hr))
//	{
//		return hr;
//	}*/
//	
//	// ステージ選択のテクスチャ読み込み
//	for (int i = 0; i < MAX_STAGE_SELECT; ++i)
//	{
//		hr = CreateTextureFromFile(pDevice,					// デバイスへのポインタ
//			g_FileNameStage[i],		// ファイルの名前
//			&m_pTexStage[i]);					// 読み込むメモリー
//		if (FAILED(hr))
//		{
//			return hr;
//		}
//	}
//
//
//	// オブジェクト生成
//	m_camera.Init();
//	m_camera.SetSkyDome(CModel::GetAssimp(MT_SKY));
//	SetCamera(&m_camera);
//
//	m_light.Init();
//	SetLight(&m_light);
//
//
//	// エフェクシア初期化
//	m_pEffekseerManager = new CEffekseerManager(this);
//	m_pEffekseerManager->Init();
//	m_pEffekseerManager->SetCamera(&m_camera);
//
//	
//	new CEffectManager(this);
//	// エネミー
//	/*CEnemyManager* pEnemyManager = new CEnemyManager_StageSelect(this);
//	pEnemyManager->SetTarget(m_pStageSphere);*/
//	// 全オブジェクト初期化
//	if (FAILED(CGameObj::InitAll(m_pObj))) {
//		return false;
//	}
//
//	
//	// BGM再生開始
//	CSound::Play(BGM_STAGESELECT);
//
//
//	return hr;
//}
//
//
////=============================================================================
//// ステージセレクトの終了処理
////=============================================================================
//void CStageSelect::Fin()
//{
//	//SAFE_RELEASE(m_pTexBG);
//	for (int i = 0; i < MAX_STAGE_SELECT; ++i) {
//		SAFE_RELEASE(m_pTexStage[i]);
//	}
//
//
//	
//
//	// BGM再生停止
//	CSound::Stop(BGM_STAGESELECT);
//
//	// 全オブジェクト終了処理
//	CGameObj::FinAll(m_pObj);
//
//
//	// エフェクシア終了
//	m_pEffekseerManager->Uninit();
//	delete m_pEffekseerManager;
//}
//
//
////=============================================================================
//// ステージセレクトの更新処理
////=============================================================================
//void CStageSelect::Update()
//{
//	/***** 選択されたステージ画像の拡大縮小 *****/
//	if (m_bScaleUp)
//	{
//		m_nSclCnt += SCALE_RATE;
//		if (m_nSclCnt >= MAX_SCALE_UP)
//		{
//			m_bScaleUp = false;
//		}
//	}
//	else
//	{
//		m_nSclCnt -= SCALE_RATE;
//		if (m_nSclCnt <= MIN_SCALE_DOWN)
//		{
//			m_bScaleUp = true;
//		}
//	}
//
//	/***** 選択された枠(背景画像)の表示*****/
//	if (m_vColor.y <= 0.0f && m_vColor.z <= 1.0f)
//	{
//		m_vColor.z-= COLOR_RATE;
//		m_vColor.x += COLOR_RATE;
//		if (m_vColor.z <= 0.0f)
//		{
//			m_vColor.z = 0.0f;
//		}
//		if (m_vColor.x >= 1.0f)
//		{
//			m_vColor.x = 1.0f;
//		}
//	}
//	if (m_vColor.x <= 1.0f && m_vColor.z <= 0.0f)
//	{
//		m_vColor.x -= COLOR_RATE;
//		m_vColor.y += COLOR_RATE;
//		if (m_vColor.x <= 0.0f)
//		{
//			m_vColor.x = 0.0f;
//		}
//		if (m_vColor.y >= 1.0f)
//		{
//			m_vColor.y = 1.0f;
//		}
//	}
//	if (m_vColor.x <= 0.0f && m_vColor.y <= 1.0f)
//	{
//		m_vColor.y -= COLOR_RATE;
//		m_vColor.z += COLOR_RATE;
//		if (m_vColor.y <= 0.0f)
//		{
//			m_vColor.y = 0.0f;
//		}
//		if (m_vColor.z >= 1.0f)
//		{
//			m_vColor.z = 1.0f;
//		}
//	}
//
//	/*m_bInputDownOld = m_bInputDownNow;
//	m_bInputUpOld = m_bInputUpNow;*/
//
//	if (CFade::Get() != FADE_NONE) return;
//	if (CInput::GetKeyTrigger(VK_A) /*|| m_bInputUpNow && !m_bInputUpOld*/)
//	{
//		CSound::Play(SE_MENUE);
//		m_nCurrentSelect = (STAGE_MENU)((m_nCurrentSelect + MAX_STAGE_SELECT - 1) % MAX_STAGE_SELECT);
//
//	}
//	else if (CInput::GetKeyTrigger(VK_D) /*|| m_bInputDownNow && !m_bInputDownOld*/)
//	{
//		CSound::Play(SE_MENUE);
//		m_nCurrentSelect = (STAGE_MENU)((m_nCurrentSelect + 1) % MAX_STAGE_SELECT);
//
//	}
//
//
//	if (CInput::GetKeyRelease(VK_SPACE)) {
//		CSound::Play(SE_SELECT_DECID);
//		switch (m_nCurrentSelect)
//		{
//		case TUTORIAL:
//			CFade::Out(SCENE_TUTORIAL);
//			break;
//		case STAGE_1:
//			CFade::Out(SCENE_GAME_1);
//			break;
//		case STAGE_2:
//			CFade::Out(SCENE_GAME_2);
//			break;
//		case STAGE_3:
//			CFade::Out(SCENE_GAME_3);
//			break;
//		default:
//			break;
//		}
//	}
//
//
//	// 全キャラ更新
//	CGameObj::UpdateAll(m_pObj);
//
//	// エフェクシア更新
//	m_pEffekseerManager->Update();
//}
//
//
////=============================================================================
//// ステージセレクトの描画処理
////=============================================================================
//void CStageSelect::Draw()
//{
//
//	// 全キャラ描画
//	CGameObj::DrawAll(m_pObj);
//
//	// エフェクシア描画
//	m_pEffekseerManager->Draw();
//
//
//	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
//
//	// 半透明部分を描画
//	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
//	SetZWrite(false);
//
//
//	//// 背景描画
//	//CPolygon::SetTexture(m_pTexBG);
//	//CPolygon::SetPos(0.0f, 0.0f);
//	//CPolygon::SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
//	//CPolygon::SetFrameSize( 1.0f, 1.0f);
//	//CPolygon::SetUV(0.0f, 0.0f);
//	//CPolygon::Draw(pDeviceContext);
//
//	// 選択画面描画
//	int i;
//	if (MAX_STAGE_SELECT % 2) {
//		// 奇数
//		float fPosX;
//		for (i = 0; i < MAX_STAGE_SELECT; ++i) {
//
//			fPosX = (i - MAX_STAGE_SELECT / 2) * SELECT_SIZE_X;
//			fPosX += (i - MAX_STAGE_SELECT / 2) * SELECT_INTERVAL;
//
//			if (i == m_nCurrentSelect) {
//				// 枠
//				CPolygon::SetTexture(nullptr);
//				CPolygon::SetPos(fPosX, 0.0f);
//				CPolygon::SetSize(SELECT_SIZE_X + m_nSclCnt + FRAME_WIDTH, 100.0f + m_nSclCnt + FRAME_WIDTH);
//				CPolygon::SetColor(m_vColor.x, m_vColor.y, m_vColor.z);
//				CPolygon::SetAlpha(1.0f);
//				CPolygon::Draw(pDeviceContext);
//
//				// ステージ画像
//				CPolygon::SetTexture(m_pTexStage[i]);
//				CPolygon::SetPos(fPosX, 0.0f);
//				CPolygon::SetSize(SELECT_SIZE_X + m_nSclCnt, SELECT_SIZE_Y  + m_nSclCnt);
//				CPolygon::SetColor(1.0f, 1.0f, 1.0f);
//				CPolygon::SetAlpha(1.0f);
//				CPolygon::Draw(pDeviceContext);
//			}
//			else
//			{
//				// ステージ画像
//				CPolygon::SetTexture(m_pTexStage[i]);
//				CPolygon::SetPos(fPosX, 0.0f);
//				CPolygon::SetSize(SELECT_SIZE_X, SELECT_SIZE_Y);
//				CPolygon::SetColor(1.0f, 1.0f, 1.0f);
//				CPolygon::SetAlpha(1.0f);
//				CPolygon::Draw(pDeviceContext);
//			}
//		}
//		
//
//		
//	}
//	else
//	{
//		// 偶数
//
//
//		for (int i = 0; i < MAX_STAGE_SELECT; ++i) {
//			float fPosX;
//			fPosX = (i - MAX_STAGE_SELECT / 2) * SELECT_SIZE_X;
//			fPosX += (i - MAX_STAGE_SELECT / 2) * SELECT_INTERVAL;
//			fPosX += (SELECT_SIZE_X + SELECT_INTERVAL) / 2.0f;
//
//			if (i == m_nCurrentSelect) {
//				// 枠
//				CPolygon::SetTexture(nullptr);
//				CPolygon::SetPos(fPosX, 0.0f);
//				CPolygon::SetSize(SELECT_SIZE_X + m_nSclCnt + FRAME_WIDTH, 100.0f + m_nSclCnt + FRAME_WIDTH);
//				CPolygon::SetColor(m_vColor.x, m_vColor.y, m_vColor.z);
//				CPolygon::SetAlpha(1.0f);
//				CPolygon::Draw(pDeviceContext);
//
//				// ステージ画像
//				CPolygon::SetTexture(m_pTexStage[i]);
//				CPolygon::SetPos(fPosX, 0.0f);
//				CPolygon::SetSize(SELECT_SIZE_X + m_nSclCnt, SELECT_SIZE_Y + m_nSclCnt);
//				CPolygon::SetColor(1.0f, 1.0f, 1.0f);
//				CPolygon::SetAlpha(1.0f);
//				CPolygon::Draw(pDeviceContext);
//			}
//			else {
//				// ステージ画像
//				CPolygon::SetTexture(m_pTexStage[i]);
//				CPolygon::SetPos(fPosX, 0.0f);
//				CPolygon::SetSize(SELECT_SIZE_X, SELECT_SIZE_Y);
//				CPolygon::SetColor(1.0f, 1.0f, 1.0f);
//				CPolygon::SetAlpha(1.0f);
//				CPolygon::Draw(pDeviceContext);
//			}
//		}
//
//		
//	}
//	
//	
//
//
//	SetZWrite(true);							// Zバッファ更新する
//	SetBlendState(BS_NONE);		// アルファブレンド無効
//}
