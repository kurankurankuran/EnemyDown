// BGM/SE定義 [SoundData.h]

enum eBGM {
	BGM_TUTORIAL = 0, // チュートリアルBGM 
	BGM_BATTLE,	    // 戦闘BGM
	BGM_TITLE,		// タイトルBGM
	BGM_STAGESELECT,		// ステージセレクトBGM
	BGM_GAMEOVER,		// ゲームオーバーBGM
	BGM_GAMECLEAR,		// ゲームクリアーBGM

	MAX_BGM
};

enum eSE {
	SE_SHOT = 0,	      // 弾発射音
	SE_SWORD_SLASH,       // 剣攻撃音
	SE_SWORD_HIT,         // 剣攻撃ヒット音
	SE_EXPLOSION,	      // 爆発音
	SE_DEFEND,		      // 衝突音
	SE_ENEMY_DIE,         // 敵の死亡時 https://dova-s.jp/se/play344.html
	SE_PLAYER_DIE,        // プレイヤー死亡時SE
	SE_GAMEOVER,	      // ゲームオーバーBGM
	SE_DECID,	          // 決定音
	SE_DECID_SCENECHANGE, // シーン切替ボタン決定音
	SE_MENUE,	          // メニュー音
	SE_SELECT_DECID,      // ステージセレクト決定音
	SE_ACCEL,             // アクセル音
	SE_PLAYER_DAMAGE,     // プレイヤーダメージ音

	MAX_SE
};
