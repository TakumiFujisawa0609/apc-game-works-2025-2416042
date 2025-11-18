#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:inputManager_(InputManager::GetInstance())
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

	SetMouseDispFlag(TRUE);
}

void TitleScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();

	// Tabキーでポーズメニュー表示
	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		state_ = TitleState::PAUSE; // ← 新しく追加した状態
		return;
	}

	// --- ポーズメニュー中の処理 ---
	if (state_ == TitleState::PAUSE)
	{

		// 上下キーで選択移動（長押し対策で IsTrgDown 使用）
		if (ins.IsTrgDown(KEY_INPUT_W))
		{
			pauseSelect_ = (pauseSelect_ + (int)PauseMenu::COUNT - 1) % (int)PauseMenu::COUNT;
		}
		else if (ins.IsTrgDown(KEY_INPUT_S))
		{
			pauseSelect_ = (pauseSelect_ + 1) % (int)PauseMenu::COUNT;
		}

		// マウス位置取得
		int mx, my;
		GetMousePoint(&mx, &my);

		// 選択肢の座標（Draw と同じ数値）
		int selectX = 700;
		int selectY0 = 400;
		int selectY1 = 500;
		int selectHeight = 80; // 適当、高さ判定

		// マウスが「タイトルへ」の上にある
		if (mx >= selectX && mx <= selectX + 400 &&
			my >= selectY0 && my <= selectY0 + selectHeight)
		{
			pauseSelect_ = 0;
		}

		// マウスが「ゲーム終了」の上にある
		if (mx >= selectX && mx <= selectX + 400 &&
			my >= selectY1 && my <= selectY1 + selectHeight)
		{
			pauseSelect_ = 1;
		}

		// 決定
		if (ins.IsTrgDown(KEY_INPUT_SPACE) || ins.IsTrgMouseLeft())
		{
			if (pauseSelect_ == (int)PauseMenu::EXIT)
			{
				DxLib_End();
				exit(0);
			}
			else if (pauseSelect_ == (int)PauseMenu::CANCEL)
			{
				state_ = TitleState::NORMAL;
			}
		}

		return; // ポーズメニュー表示中は下の処理をスキップ
	}

	int mouse = GetMouseInput();  // 今のマウス状態を取得
	if (mouse & MOUSE_INPUT_LEFT)  // 左クリックされた？
	{
		StopSoundMem(bgmHandle_);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		return;
	}

	// 全てのキーのうち、どれかを押したら画面遷移
	for (int key = 0; key < 256; key++) {
		if (ins.IsTrgDown(key)) {
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
	DrawFormatString(0, 1000, GetColor(255, 255, 0), "Tabキーでポーズメニュー");

	if (state_ == TitleState::PAUSE)
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

void TitleScene::Release(void)
{
	// 画像解放
	DeleteGraph(imgTitle_);

	// BGM解放
	StopSoundMem(bgmHandle_);
	DeleteSoundMem(bgmHandle_);
}
