#pragma once
#include "SceneBase.h"
#include "PauseScene.h"
#include "../Application.h"
#include "../Manager/InputManager.h"

class SceneManager;

class TitleScene : public SceneBase
{
	enum  TitleState
	{
		NORMAL,
		PAUSE,
	};

public:
	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
	// 入力制御オブジェクト
	InputManager& inputManager_;
	// ポーズシーンオブジェクト
	PauseScene& pauseScene_;
	// シーンの状態
	TitleState state_ = TitleState::NORMAL;

	// ポーズメニューの選択肢
	int pauseSelect_;

	// 画像
	int BgImage_;
	// BGM
	int bgmHandle_;
	// フォントハンドル
	int fontHandle_;
	int fontButton_;
	int fontPause_;
	int fontEscape_;

	// ボタン変数
	int btnX_;
	int btnY_;
	int btnW_;
	int btnH_;

	// ポーズボタン
	int pauseX_;
	int pauseY_;
	int pauseW_;
	int pauseH_;

	// アニメーション用
	float buttonAnimTime_;   // 時間
	float buttonScale_;			 // 拡大率
};