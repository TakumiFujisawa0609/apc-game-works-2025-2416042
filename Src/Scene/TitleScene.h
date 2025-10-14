#pragma once
#include "SceneBase.h"
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

	enum  PauseMenu
	{
		CANCEL,
		EXIT,
		COUNT
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

	// シーンの状態
	TitleState state_ = TitleState::NORMAL;

	// ポーズメニューの選択肢
	int pauseSelect_;

	// 画像
	int imgTitle_;

};