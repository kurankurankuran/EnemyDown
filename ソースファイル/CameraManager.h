#pragma once
#include"Camera.h"
#include<map>
#include<memory>
class CCameraManager
{
public:
	enum class ECameraMode {
		STATIC_CAMERAMODE = 0,  // 定点カメラ
		//STATIC_TITLE_CAMERAMODE,// 定点カメラ（タイトル）
		FPS_CAMERAMODE,         // 一人称カメラ
		BULLET_CAMERAMODO,      // バレットタイム風カメラ
		TPS_CAMERAMODE,         // 三人称カメラ
		BULLET_FREE_CAMERAMODE, // バレットタイム風カメラ(自由箇所)
		EDITOR_CAMERAMODE,      // エディターカメラ操作
		MINIMAP_CAMERAMODE,     // ミニマップカメラ

		MAX_CAMERAMODE
	};
public:

	CCameraManager();
	~CCameraManager();

	bool Init();
	void Update();
	
	ECameraMode GetMode() { return m_eMode; }
	CCamera* GetCamera() { return m_pCamera; }
	void ChangeCamera(ECameraMode);
	void AddCamera(ECameraMode);                 
	std::shared_ptr<CCamera> GetMiniMapCamera();
private:
	ECameraMode m_eMode;					                                           // 現在のカメラモード
	CCamera* m_pCamera;                                                        // 現在使用中のカメラ
	std::map<ECameraMode, std::shared_ptr<CCamera>> m_empECameraModeToCamera;  // 使用するカメラリスト
};
