//=============================================================================
//
// ゲームシーンエディタークラス実装 [CGameEditor_Scene.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "GameEditor_Scene.h"
#include "DebugProc.h"
#include "Input.h"
#include "GameObj.h"
#include "Sound.h"
#include "Fade.h"
#include "EffekseerManager.h"
#include "Land.h"
#include "EnemyManager.h"
#include "DeathCube.h"
#include "Polygon.h"
#include "CreateObj.h"

// コンストラクタ
CGameEditor_Scene::CGameEditor_Scene() : CScene()
{
	m_id = SCENE_GAMEEDITOR;
	m_nBound = 0;
}

// デストラクタ
CGameEditor_Scene::~CGameEditor_Scene()
{
}

// 初期化
bool CGameEditor_Scene::Init()
{
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::EDITOR_CAMERAMODE);
	GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::EDITOR_CAMERAMODE);
	
	// 基底シーン初期化
	if (!CScene::Init()) {
		return false;
	}

	// エディターインスタンス生成初期化
	m_upEM = std::make_unique<CEditor_Manager>();
	m_upEM->Init();

	// エフェクシア初期化
	m_pEffekseerManager = new CEffekseerManager(this);
	m_pEffekseerManager->Init();
	m_pEffekseerManager->SetCamera(GetCameraManager()->GetCamera());

	// ***************** 
	// オブジェクト生成
	// *****************
	CGameObj* pChangeStartTransFormGameObj;
	// プレイヤー
	CScene::GetSaveManager()->AddSaveData(pChangeStartTransFormGameObj = CCreateObj::CreateObj(OI_PLAYER));
	// トランスフォーム
	pChangeStartTransFormGameObj->SetStartPos(XMFLOAT3(0, 0, 0));
	pChangeStartTransFormGameObj->SetStartAngle(XMFLOAT3(0, 0, 0));
	pChangeStartTransFormGameObj->SetStartScale(XMFLOAT3(1, 1, 1));

	// エネミーマネージャー
	CScene::GetSaveManager()->AddSaveData(new CEnemyManager(this));
	// 地形モデル
	CScene::GetSaveManager()->AddSaveData(CCreateObj::CreateObj(OI_LAND));
	// 崖モデル
	const int nCliff = 4;
	CGameObj* pCliff[nCliff];
	for (int i = 0; i < nCliff; ++i)
	{
		CScene::GetSaveManager()->AddSaveData(pCliff[i] = CCreateObj::CreateObj(OI_CLIFF));
		//pCliff[i]->SetModel(EModelType::MT_CLIFF);
		pCliff[i]->SetObjID(EObjID::OI_CLIFF);
		pCliff[i]->SetCenter(XMFLOAT3(0.0f, 0.0f, 0.0f));
		pCliff[i]->SetBBox(XMFLOAT3(300.f, 2000.f, 5000.0f));
		pCliff[i]->SetStartScale(XMFLOAT3(2, 2, 2));
	}
	pCliff[0]->SetStartPos(XMFLOAT3(2700.0f, -500.0f, 0.0f));
	pCliff[0]->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	
	pCliff[1]->SetStartPos(XMFLOAT3(-2700.0f, -500.0f, 0.0f));
	pCliff[1]->SetStartAngle(XMFLOAT3(0.0f, 180.0f, 0.0f));

	pCliff[2]->SetStartPos(XMFLOAT3(0.0f, -500.0f, 2700.0f));
	pCliff[2]->SetStartAngle(XMFLOAT3(0.0f,-90.0f, 0.0f));

	pCliff[3]->SetStartPos(XMFLOAT3(0.0f, -500.0f, -2700.0f));
	pCliff[3]->SetStartAngle(XMFLOAT3(0.0f, 90.0f, 0.0f));

	// 世界の壁用岩モデル
	const int nRockNum = 3;// 岩個数
	CGameObj* pWallRock[nRockNum];
	for (int i = 0; i < nRockNum; ++i)
	{
		CScene::GetSaveManager()->AddSaveData(pWallRock[i] = CCreateObj::CreateObj(OI_ROCK));
		//pWallRock[i]->SetModel(EModelType::MT_ROCK);
		pWallRock[i]->SetObjID(EObjID::OI_ROCK);
		pWallRock[i]->SetBBox(XMFLOAT3(150.f, 150.f, 150.0f));
		pWallRock[i]->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
		pWallRock[i]->SetStartScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		pWallRock[i]->SetStartPos(XMFLOAT3(0.0f, 0.0f, 0.0f));

	}
	pWallRock[0]->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pWallRock[0]->SetStartScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	pWallRock[0]->SetStartPos(XMFLOAT3(1000.0f, 0.0f, -1000.0f));

	pWallRock[1]->SetStartAngle(XMFLOAT3(10.0f, 10.0f, 0.0f));
	pWallRock[1]->SetStartScale(XMFLOAT3(3.0f, 3.0f, 3.0f));
	pWallRock[1]->SetStartPos(XMFLOAT3(0.0f, 0.0f, -1300.0f));

	pWallRock[2]->SetStartAngle(XMFLOAT3(20.0f, 0.0f, 20.0f));
	pWallRock[2]->SetStartScale(XMFLOAT3(2.0f, 2.0f, 2.0f));
	pWallRock[2]->SetStartPos(XMFLOAT3(-1500.0f, 0.0f, 1000.0f));

	// 触れたら死ぬキューブ
	CScene::GetSaveManager()->AddSaveData(pChangeStartTransFormGameObj = CCreateObj::CreateObj(OI_DEATHOBJ));
	pChangeStartTransFormGameObj->SetStartPos(XMFLOAT3(0.0f, -2000.0f, 0.0f));
	pChangeStartTransFormGameObj->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pChangeStartTransFormGameObj->SetStartScale(XMFLOAT3(10000.0f, 50.0f, 10000.0f));

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
	CGameObj* pTrgetObj = CScene::Find(GOT_PLAYER);
	GetCameraManager()->GetCamera()->SetTargetObj(pTrgetObj);
	//GetMiniMapCamera()->SetTargetObj(pTrgetObj);
	GetCameraManager()->GetCamera()->SetSkyDome(CModel::GetAssimp(MT_SKY));


	// BGM再生開始
	CSound::Play(BGM_BATTLE);

	// 画面中央にマウスカーソルを固定
	POINT ptCenter = GetScreenCenter();
	SetCursorPos(ptCenter.x, ptCenter.y);
	// マウスカーソルの非表示
	//ShowCursor(FALSE);
	
	return true;
}

// 終了処理
void CGameEditor_Scene::Fin()
{
	// 終了時点のトランスフォーム変数を保存(本来はエディターシーンでのセーブ時に決定されるべき)
	CGameObj* pNext;
	for (CGameObj* pObj = m_pObj; pObj; pObj = pNext) {
		pNext = pObj->GetNext();
		pObj->SetStartPos(pObj->GetTransform()->GetPos());
		pObj->SetStartAngle(pObj->GetTransform()->GetAngle());
		pObj->SetStartScale(pObj->GetTransform()->GetScale());
	}
	

	// セーブ
	CScene::GetSaveManager()->Save("data/SaveData/SaveData.txt");
	CScene::GetSaveManager()->ResetSaveData();

	// BGM再生停止
	CSound::Stop(BGM_BATTLE);

	// 全オブジェクト終了処理
	CGameObj::FinAll(m_pObj);

	m_meshLine.Fin();
	
	// エフェクシア終了
	m_pEffekseerManager->Uninit();
	SAFE_DELETE(m_pEffekseerManager);

	// エディター終了
	m_upEM->Fin();
	// ポーズ終了
	//GetPause()->Fin();
}

// 更新
void CGameEditor_Scene::Update()
{
	// エディター更新
	m_upEM->Update();
	
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
		//CGameObj::UpdateAll(m_pObj);
		// エフェクシア更新
		m_pEffekseerManager->Update();
		
		GetLight()->Update();
		m_fRotValue += LIGHT_DIR_ROTVALUE;
		
	}	

	if (CInput::GetKeyTrigger(VK_N))
	{
		// シーンチェンジ
		CFade::Out(SCENE_GAME);
	}
}

// 描画
void CGameEditor_Scene::Draw()
{
	// エディター描画
	m_upEM->Draw();
	// 全キャラ描画
	CGameObj::DrawAll(m_pObj);
	// エフェクシア描画
	m_pEffekseerManager->Draw();
}
