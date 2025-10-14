#pragma once
#include "SceneBase.h"
#include "../Manager/InputManager.h"

class SceneManager;

class ClearScene : public SceneBase
{
	enum  ClearState
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
#pragma region 前方宣言
	// コンストラクタ
	ClearScene(void);

	// デストラクタ
	~ClearScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
#pragma region 変数宣言
	// 入力制御オブジェクト
	InputManager& inputManager_;

	// シーンの状態
	ClearState state_ = ClearState::NORMAL;

	// ポーズメニューの選択肢
	int pauseSelect_;
	// ゲームクリア画像
	int imgGameClear_;

};

