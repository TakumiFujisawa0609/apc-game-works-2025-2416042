#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:inputManager_(InputManager::GetInstance())
	, pauseScene_(PauseScene::GetInstance())
{
	state_ = TitleState::NORMAL;
	pauseSelect_ = 0;
	imgTitle_ = -1;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	imgTitle_ = LoadGraph((Application::PATH_IMAGE + "Title1.png").c_str());
	bgmHandle_ = LoadSoundMem((Application::PATH_DATA + "BGM/TitleScene.mp3").c_str());
	PlaySoundMem(bgmHandle_, DX_PLAYTYPE_LOOP);
}

void TitleScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	// マウスカーソルを表示
	SetMouseDispFlag(TRUE);

	// Tabキーでポーズメニュー表示
	if (ins.IsTrgDown(KEY_INPUT_TAB) || ins.IsTrgDown(KEY_INPUT_ESCAPE))
	{
		state_ = TitleState::PAUSE; // ← 新しく追加した状態
		return;
	}

	// --- ポーズメニュー中の処理 ---
	if (state_ == TitleState::PAUSE)
	{
		pauseScene_.TitleUpdate();
		return; // ポーズメニュー中は以降の処理を行わない
	}
	

	// 全てのキーのうち、どれかを押したら画面遷移
	for (int key = 0; key < 256; key++) {
		if (ins.IsTrgDown(key) || ins.IsTrgMouseLeft()) {
			// BGM停止
			StopSoundMem(bgmHandle_);
			// ゲームシーンへ遷移
			SceneManager::GetInstance().ChangeScene(
				SceneManager::SCENE_ID::GAME);
			break; // 一つでも押されたら遷移
		}
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 2,
		1.0f, 0.0, imgTitle_, true);

	// ポーズメニューの操作ヒント
	SetFontSize(50);
	DrawFormatString(0, 1000, GetColor(255, 255, 0), "Tab/ESCでポーズメニュー");

	if (state_ == TitleState::PAUSE)
	{
		pauseScene_.TitleDraw();
	}
}

void TitleScene::Release(void)
{
	// 画像解放
	DeleteGraph(imgTitle_);

	// BGM解放
	StopSoundMem(bgmHandle_);
	DeleteSoundMem(bgmHandle_);
}
