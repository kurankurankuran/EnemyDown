#pragma once
#include"Camera.h"
#include<map>
#include<memory>
class CCameraManager
{
public:
	enum class ECameraMode {
		STATIC_CAMERAMODE = 0,  // ��_�J����
		//STATIC_TITLE_CAMERAMODE,// ��_�J�����i�^�C�g���j
		FPS_CAMERAMODE,         // ��l�̃J����
		BULLET_CAMERAMODO,      // �o���b�g�^�C�����J����
		TPS_CAMERAMODE,         // �O�l�̃J����
		BULLET_FREE_CAMERAMODE, // �o���b�g�^�C�����J����(���R�ӏ�)
		EDITOR_CAMERAMODE,      // �G�f�B�^�[�J��������
		MINIMAP_CAMERAMODE,     // �~�j�}�b�v�J����

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
	ECameraMode m_eMode;					                                           // ���݂̃J�������[�h
	CCamera* m_pCamera;                                                        // ���ݎg�p���̃J����
	std::map<ECameraMode, std::shared_ptr<CCamera>> m_empECameraModeToCamera;  // �g�p����J�������X�g
};
