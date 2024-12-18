// BGM/SE��` [SoundData.h]

enum eBGM {
	BGM_TUTORIAL = 0, // �`���[�g���A��BGM 
	BGM_BATTLE,	    // �퓬BGM
	BGM_TITLE,		// �^�C�g��BGM
	BGM_STAGESELECT,		// �X�e�[�W�Z���N�gBGM
	BGM_GAMEOVER,		// �Q�[���I�[�o�[BGM
	BGM_GAMECLEAR,		// �Q�[���N���A�[BGM

	MAX_BGM
};

enum eSE {
	SE_SHOT = 0,	      // �e���ˉ�
	SE_SWORD_SLASH,       // ���U����
	SE_SWORD_HIT,         // ���U���q�b�g��
	SE_EXPLOSION,	      // ������
	SE_DEFEND,		      // �Փˉ�
	SE_ENEMY_DIE,         // �G�̎��S�� https://dova-s.jp/se/play344.html
	SE_PLAYER_DIE,        // �v���C���[���S��SE
	SE_GAMEOVER,	      // �Q�[���I�[�o�[BGM
	SE_DECID,	          // ���艹
	SE_DECID_SCENECHANGE, // �V�[���ؑփ{�^�����艹
	SE_MENUE,	          // ���j���[��
	SE_SELECT_DECID,      // �X�e�[�W�Z���N�g���艹
	SE_ACCEL,             // �A�N�Z����
	SE_PLAYER_DAMAGE,     // �v���C���[�_���[�W��

	MAX_SE
};
