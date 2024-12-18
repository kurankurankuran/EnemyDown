//=============================================================================
//
// エディターシーン クラス実装 [CEditor_Scene.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include"Game_Scene.h"
#include"DebugProc.h"
#include"Input.h"
#include"GameObj.h"
#include"Sound.h"
#include"Fade.h"
#include"Pause.h"
#include"EffekseerManager.h"
#include"Polygon.h"
#include"CameraManager.h"


// コンストラクタ
CGame_Scene::CGame_Scene() : CScene()
{
	m_id = SCENE_GAME;

	m_nBound = 0;
}

// デストラクタ
CGame_Scene::~CGame_Scene()
{
}

// 初期化
bool CGame_Scene::Init()
{
	// 使用するカメラを追加
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::TPS_CAMERAMODE);
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::FPS_CAMERAMODE);
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::MINIMAP_CAMERAMODE);
	GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::TPS_CAMERAMODE);
	// 基底シーン初期化
	if (!CScene::Init()) {
		return false;
	}

	// ポーズ初期化
	GetPause()->Init();
	GetPause()->SetCamera(GetCameraManager()->GetCamera());

	// エフェクシア初期化
	m_pEffekseerManager = new CEffekseerManager(this);
	m_pEffekseerManager->Init();
	m_pEffekseerManager->SetCamera(GetCameraManager()->GetCamera());

	// ***************** 
	// オブジェクト生成
	// *****************
	CScene::GetSaveManager()->Load("data/SaveData/SaveData.txt");
	
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
	
	
	// 各種カメラにターゲットオブジェクトを設定
	CGameObj* pTargetObj = CScene::Find(GOT_PLAYER);
	GetCameraManager()->GetCamera()->SetTargetObj(pTargetObj);
	GetCameraManager()->GetMiniMapCamera()->Init();
	GetCameraManager()->GetMiniMapCamera()->SetTargetObj(pTargetObj);
	//GetMiniMapCamera()->SetTargetObj(pTrgetObj);
	// スカイドーム設定
	//GetCameraManager()->GetCamera()->SetSkyDome(CModel::GetAssimp(MT_SKY));
	GetCameraManager()->GetMiniMapCamera()->SetSkyDome(CModel::GetAssimp(MT_SKY));
	//SetMiniMapCamera(&m_MiniMapCamera);

	// BGM再生開始
	CSound::Play(BGM_BATTLE);

	// 画面中央にマウスカーソルを固定
	CInput::SetRockCursorFlag(true);
	POINT ptCenter = GetScreenCenter();
	SetCursorPos(ptCenter.x, ptCenter.y);
	// マウスカーソルの非表示
	ShowCursor(FALSE);
	
	return true;
}

// 終了処理
void CGame_Scene::Fin()
{
	// BGM再生停止
	CSound::Stop(BGM_BATTLE);

	// 全オブジェクト終了処理
	CGameObj::FinAll(m_pObj);

	// エフェクシア終了
	m_pEffekseerManager->Uninit();
	SAFE_DELETE(m_pEffekseerManager);

	// ポーズ終了
	GetPause()->Fin();

	// ミニマップ終了
	//m_MiniMapCamera.Fin();
	//SetMiniMapCamera(nullptr);
}

// 更新
void CGame_Scene::Update()
{
	GetPause()->Update();
	
	// 境界球/境界ボックス表示切替
	if (CInput::GetKeyTrigger(VK_B)) {
		CGameObj* pObj = Find();
		m_nBound = (m_nBound + MAX_BOUND - 1) % MAX_BOUND;
		while (pObj) {
			switch (pObj->GetTag()) {
			case GOT_LAND:
				// 描画しない
				break;
			default:
				pObj->SetBSVisible((EBoundingShape)m_nBound);
				break;
			}
			pObj = Find(-1, pObj);
		}
	}

	if (!GetPause()->GetPauseFlg() && CFade::Get() == FADE_NONE) {
		// 全キャラ更新
		CGameObj::UpdateAll(m_pObj);
		// エフェクシア更新
		m_pEffekseerManager->Update();
		
		//m_light.Update();
		GetLight()->Update();
		
	}

	GetCameraManager()->GetMiniMapCamera()->Update();
	//m_MiniMapCamera.Update();

	// デバッグ表示
	CDebugProc::Print("*** ｷｮｳｶｲ ﾋｮｳｼﾞ ***\n");
	CDebugProc::Print("None/Sphere/AABB/OBB:[B]\n");
	CDebugProc::Print("\n");
}

// 描画
void CGame_Scene::Draw()
{
	// 全キャラ描画
	CGameObj::DrawAll(m_pObj);
	// エフェクシア描画
	m_pEffekseerManager->Draw();
	// 全キャラUI描画
	CGameObj::DrawUIAll(m_pObj);

	// ミニマップ描画
	GetCameraManager()->GetMiniMapCamera()->Clear();
	GetCameraManager()->GetMiniMapCamera()->Draw();
	/*m_MiniMapCamera.Clear();
	m_MiniMapCamera.Draw();*/
	//GetMiniMapCamera()->Draw();

	// ポーズ描画
	GetPause()->Draw();
}
