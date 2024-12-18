//=============================================================================
//
// タイトル画面クラス実装 [Title.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include"Title.h"
#include"Sound.h"
#include"Polygon.h"
#include"Fade.h"
#include"EffekseerManager.h"
#include"Text.h"
#include"CameraManager.h"

// 定数定義
namespace {
	
	
}

// コンストラクタ
CTitle::CTitle() : CScene()
{
	m_id = SCENE_TITLE;

	m_nCurMenue = STAGESELECT_MENUE;
	m_nCurOption = SOUND_OPTION;
	m_nCurInput = MOUSESENSI_X_INPUT;
	m_nCurSound = MASTER_SOUND;
	
	m_nBound = 0;

	m_pWave = nullptr;
}

// デストラクタ
CTitle::~CTitle()
{
}

// 初期化
bool CTitle::Init()
{
	// カメラ設定
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::STATIC_CAMERAMODE);
	GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::STATIC_CAMERAMODE);
	GetCameraManager()->GetCamera()->SetPos(200.0f, 300.0f, -200.0f);
	GetCameraManager()->GetCamera()->SetTargetObj(nullptr);
	GetCameraManager()->GetCamera()->SetTarget(0, 100, 0);

	// 基底シーン初期化
	if (!CScene::Init()) {
		return false;
	}
	
	// ***************** 
	// オブジェクト生成
	// *****************

	// スカイドーム設定
	GetCameraManager()->GetCamera()->SetSkyDome(CModel::GetAssimp(MT_SKY));
	
	// エフェクシア初期化
	m_pEffekseerManager = new CEffekseerManager(this);
	m_pEffekseerManager->Init();
	m_pEffekseerManager->SetCamera(GetCameraManager()->GetCamera());

	
	// 世界の壁用岩モデル
	const int nRockNum = 1;// 岩個数
	CModel* pWallRockModel[nRockNum];
	for (int i = 0; i < nRockNum; ++i)
	{
		pWallRockModel[i] = new CModel(this);
		pWallRockModel[i]->SetModel(MT_ROCK);
	}
	pWallRockModel[0]->SetStartPos(XMFLOAT3(-200.0f, -150.0f, 0.0f));
	pWallRockModel[0]->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pWallRockModel[0]->SetStartScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	


	CModel* pPlayerModel = new CModel(this);
	pPlayerModel->SetModel(MT_PALADIN);
	pPlayerModel->SetStartPos(XMFLOAT3(-200.0f, 0.0f, 0.0f));
	pPlayerModel->SetStartAngle(XMFLOAT3(0.0f, 90.0f, 0.0f));
	pPlayerModel->SetStartScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	pPlayerModel->SetCamera(GetCameraManager()->GetCamera());
	pPlayerModel->SetLight(GetLight());
	pPlayerModel->SetAnimIndex(0);
	pPlayerModel->SetAnimTime(0);

	// ************************* 
    // オブジェクト生成ここまで
    // *************************

	// 最初の全オブジェクト初期化
	if (FAILED(CGameObj::AwakeAll(m_pObj))) {
		return false;
	}

	// 全オブジェクト初期化
	if (FAILED(CGameObj::InitAll(m_pObj))) {
		return false;
	}
	
	// 水面生成,初期化
	m_pWave = new CWave(this);
	m_pWave->Init();


	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	// 画面ステート
	m_State = TitleState::Start;

	// 画面UIインスタンス初期化
	m_UIManager.Init();

	// BGM再生開始
	CSound::Play(BGM_TITLE);
	// マウスカーソルの非表示
	ShowCursor(FALSE);
	
	return true;
}

// 終了処理
void CTitle::Fin()
{
	
	// BGM再生停止
	CSound::Stop(BGM_TITLE);

	// 画面UIインスタンス終了
	m_UIManager.Fin();

	// 全オブジェクト終了処理
	CGameObj::FinAll(m_pObj);

	// 水面終了
	m_pWave->Fin();
	SAFE_DELETE(m_pWave);

	
	// エフェクシア終了
	m_pEffekseerManager->Uninit();
	delete m_pEffekseerManager;
	
	CScene::Fin();
}

// 更新
void CTitle::Update()
{
	// 全キャラ更新
	CGameObj::UpdateAll(m_pObj);

	// 水面更新
	m_pWave->Update();

	// 画面UIインスタンス更新
	m_UIManager.Update();

	// エフェクシア更新
	m_pEffekseerManager->Update();

}

// 描画
void CTitle::Draw()
{
	GetLight()->SetEnable(true);
	// 全キャラ描画
	CGameObj::DrawAll(m_pObj);

	// 水面描画
	m_pWave->Draw();

	// 画面UIインスタンス描画
	m_UIManager.Draw();

	// エフェクシア描画
	m_pEffekseerManager->Draw();

}
