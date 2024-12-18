#include"CameraManager.h"
#include"Camera_Static.h"
#include"Camera_TPS.h"
#include"Camera_FPS.h"
#include"Camera_Editor.h"
#include"MiniMapCamera.h"
#include "Input.h"

CCameraManager::CCameraManager()
{
	m_pCamera = nullptr;
}

CCameraManager::~CCameraManager()
{
	m_empECameraModeToCamera.clear();
}

bool CCameraManager::Init()
{
	if (!m_pCamera)
	{
		AddCamera(ECameraMode::STATIC_CAMERAMODE);
		ChangeCamera(ECameraMode::STATIC_CAMERAMODE);
		return false;
	}
	m_pCamera->Init();
	
	return true;
}

void CCameraManager::Update()
{
	// キーによってカメラモード切替
	if (CInput::GetKeyRelease(VK_0))
		ChangeCamera(ECameraMode::STATIC_CAMERAMODE);
	if (CInput::GetKeyRelease(VK_1))
		ChangeCamera(ECameraMode::FPS_CAMERAMODE);
	if (CInput::GetKeyRelease(VK_2))
		ChangeCamera(ECameraMode::TPS_CAMERAMODE);
	if (CInput::GetKeyRelease(VK_3))
		ChangeCamera(ECameraMode::EDITOR_CAMERAMODE);
	if (CInput::GetKeyRelease(VK_4))
		ChangeCamera(ECameraMode::BULLET_CAMERAMODO);
	if (CInput::GetKeyRelease(VK_5))
		ChangeCamera(ECameraMode::BULLET_FREE_CAMERAMODE);
	if (CInput::GetKeyRelease(VK_6))
		ChangeCamera(ECameraMode::MINIMAP_CAMERAMODE);	

	m_pCamera->Update();
}


// カメラ設定
void CCameraManager::ChangeCamera(ECameraMode eCameraMode)
{
	auto itr = m_empECameraModeToCamera.find(eCameraMode);
	if (itr != m_empECameraModeToCamera.end())
	{
		if (m_pCamera)
		{
			m_pCamera->Fin();
		}
		m_eMode = eCameraMode;
		m_pCamera = m_empECameraModeToCamera[eCameraMode].get();
	}
}

std::shared_ptr<CCamera> CCameraManager::GetMiniMapCamera()
{
	auto itr = m_empECameraModeToCamera.find(ECameraMode::MINIMAP_CAMERAMODE);
	if (itr != m_empECameraModeToCamera.end())
	{
		return m_empECameraModeToCamera[ECameraMode::MINIMAP_CAMERAMODE];
	}
	else
	{
		return nullptr;
	}
}

void CCameraManager::AddCamera(ECameraMode eCameraMode)
{
	auto itr = m_empECameraModeToCamera.find(eCameraMode);
	if (itr == m_empECameraModeToCamera.end())
	{
		switch (eCameraMode)
		{
		case ECameraMode::STATIC_CAMERAMODE:
			m_empECameraModeToCamera[ECameraMode::STATIC_CAMERAMODE] = std::make_shared<CCamera_Static>();
			break;
		case ECameraMode::TPS_CAMERAMODE:
			m_empECameraModeToCamera[ECameraMode::TPS_CAMERAMODE] = std::make_shared<CCamera_TPS>();
			break;
		case ECameraMode::FPS_CAMERAMODE:
			m_empECameraModeToCamera[ECameraMode::FPS_CAMERAMODE] = std::make_shared<CCamera_FPS>();
			break;
		case ECameraMode::EDITOR_CAMERAMODE:
			m_empECameraModeToCamera[ECameraMode::EDITOR_CAMERAMODE] = std::make_shared<CCamera_Editor>();
			break;
		case ECameraMode::MINIMAP_CAMERAMODE:
			m_empECameraModeToCamera[ECameraMode::MINIMAP_CAMERAMODE] = std::make_shared<CMiniMapCamera>();
			break;
		default:
			break;
		}
	}
}