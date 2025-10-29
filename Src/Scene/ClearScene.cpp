#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "ClearScene.h"

ClearScene::ClearScene(void)
	:inputManager_(InputManager::GetInstance())
{
	state_ = ClearState::NORMAL;
	pauseSelect_ = 0;
	imgGameClear_ = -1;
}

ClearScene::~ClearScene(void)
{
}

void ClearScene::Init(void)
{
	// 画像の読み込み
	imgGameClear_ = LoadGraph((Application::PATH_IMAGE + "a.png").c_str());
	// BGMの読み込みと再生
	 bgmHandle_ = LoadSoundMem((Application::PATH_DATA + "BGM/GameClear.mp3").c_str());
	 PlaySoundMem(bgmHandle_, DX_PLAYTYPE_LOOP);
}

void ClearScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();

	// Tabキーでポーズメニュー表示
	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		state_ = ClearState::PAUSE; // ← 新しく追加した状態
		return;
	}

	// --- ポーズメニュー中の処理 ---
	if (state_ == ClearState::PAUSE)
	{

		// 上下キーで選択移動（長押し対策で IsTrgDown 使用）
		if (ins.IsTrgDown(KEY_INPUT_W) || ins.IsTrgDown(KEY_INPUT_UP))
		{
			pauseSelect_ = (pauseSelect_ + (int)PauseMenu::COUNT - 1) % (int)PauseMenu::COUNT;
		}
		else if (ins.IsTrgDown(KEY_INPUT_S) || ins.IsTrgDown(KEY_INPUT_DOWN))
		{
			pauseSelect_ = (pauseSelect_ + 1) % (int)PauseMenu::COUNT;
		}

		// 決定
		if (ins.IsTrgDown(KEY_INPUT_RETURN) || ins.IsTrgDown(KEY_INPUT_SPACE))
		{
			if (pauseSelect_ == (int)PauseMenu::EXIT)
			{
				DxLib_End();
				exit(0);
			}
			else if (pauseSelect_ == (int)PauseMenu::CANCEL)
			{
				state_ = ClearState::NORMAL;
			}
		}

		return; // ポーズメニュー表示中は下の処理をスキップ
	}

	// 全てのキーのうち、どれかを押したら画面遷移
	for (int key = 0; key < 256; key++) {
		if (ins.IsTrgDown(key)) {
			StopSoundMem(bgmHandle_); // BGM停止
			SceneManager::GetInstance().ChangeScene(
				SceneManager::SCENE_ID::TITLE);
			break; // 一つでも押されたら遷移
		}
	}
}

void ClearScene::Draw(void)
{
	// 背景画像の描画
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 2,
		1.0f, 0.0, imgGameClear_, true);

	// ポーズメニューの操作ヒント
	SetFontSize(50);
	DrawFormatString(0, 1000, GetColor(0, 0, 0), "Tabキーでポーズメニュー");

	if (state_ == ClearState::PAUSE)
	{
		// 画面を少し暗くする
		DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150); // 半透明黒
		DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// ポーズメニューの操作ヒント
		SetFontSize(60);
		DrawFormatString(0, 1000, GetColor(255, 255, 0), "W/Sキーで操作、Spaceで選択");

		SetFontSize(100);
		// メニュータイトル
		DrawString(650, 300, "ポーズ中", GetColor(255, 255, 0));
		// 選択肢
		DrawString(700, 400, pauseSelect_ == 0 ? "> タイトルへ" : "  タイトルへ", GetColor(255, 255, 255));
		DrawString(700, 500, pauseSelect_ == 1 ? "> ゲーム終了" : "  ゲーム終了", GetColor(255, 255, 255));
	}
}

void ClearScene::Release(void)
{
	// 画像の解放
	DeleteGraph(imgGameClear_);
	// BGMの解放
	DeleteSoundMem(bgmHandle_);
	DeleteSoundMem(bgmHandle_);
}
