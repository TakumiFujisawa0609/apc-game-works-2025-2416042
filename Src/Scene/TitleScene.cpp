#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:inputManager_(InputManager::GetInstance())
	, pauseScene_(PauseScene::GetInstance()),
	state_(TitleState::NORMAL),
	pauseSelect_(0),
	BgImage_(-1),
	bgmHandle_(-1),
	fontHandle_(-1),
	btnX_(0),
	btnY_(0),
	btnW_(0),
	btnH_(0),
	fontButton_(-1)
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	BgImage_ = LoadGraph((Application::PATH_IMAGE + "Title3.png").c_str());
	bgmHandle_ = LoadSoundMem((Application::PATH_DATA + "BGM/TitleScene.mp3").c_str());
	PlaySoundMem(bgmHandle_, DX_PLAYTYPE_LOOP);

	// フォント作成
	fontHandle_ = CreateFontToHandle(
		"源ノ明朝",       // フォント名
		190,				  // サイズ
		10,				  // 太さ
		DX_FONTTYPE_ANTIALIASING // アンチエイリアス
	);
	// ボタン用フォント作成
	fontButton_ = CreateFontToHandle(
		"源ノ明朝",       // フォント名
		68,				  // サイズ
		4,				  // 太さ
		DX_FONTTYPE_ANTIALIASING // アンチエイリアス
	);
	// ポーズボタン用フォント作成
	fontPause_ = CreateFontToHandle(
		"源ノ明朝",       // フォント名
		10,				  // サイズ
		2,				  // 太さ
		DX_FONTTYPE_ANTIALIASING // アンチエイリアス
	);

	// スタートボタン設定
	btnW_ = 700;
	btnH_ = 150;
	btnX_ = Application::SCREEN_SIZE_X / 2 - btnW_ / 2;
	btnY_ = Application::SCREEN_SIZE_Y * 2 / 3;

	// ポーズメニューボタン
	pauseX_ = 20;
	pauseY_ = 20;
	pauseW_ = 60;
	pauseH_ = 60;
}

void TitleScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	// マウスカーソルを表示
	SetMouseDispFlag(TRUE);

	// --- ポーズメニュー中の処理 ---
	if (state_ == TitleState::PAUSE)
	{
		pauseScene_.TitleUpdate();
		// ポーズメニュー終了判定
		if (ins.IsTrgDown(KEY_INPUT_TAB) || 
			ins.IsTrgDown(KEY_INPUT_ESCAPE))
		{
			state_ = TitleState::NORMAL;
		}
		return; // ポーズメニュー中は以降の処理を行わない
	}

   // 通常時（ここで初めて ESC を見る）
	if (ins.IsTrgDown(KEY_INPUT_ESCAPE) ||
		ins.IsTrgDown(KEY_INPUT_TAB))
	{
		state_ = TitleState::PAUSE;
		return;
	}

	// マウス位置取得
	int mx, my;
	GetMousePoint(&mx, &my);
	// ポーズボタン上にマウスがあるか判定
	bool onPause =
		mx >= pauseX_ && mx <= pauseX_ + pauseW_ &&
		my >= pauseY_ && my <= pauseY_ + pauseH_;

	if (onPause && InputManager::GetInstance().IsTrgMouseLeft())
	{
		state_ = TitleState::PAUSE;
		pauseScene_.OnEnter();
		return;
	}

	// ボタン上にマウスがあるか判定 
	bool onButton =
		mx >= btnX_ && mx <= btnX_ + btnW_ &&
		my >= btnY_ && my <= btnY_ + btnH_;

	// ボタンクリック またはスペースキーでゲームシーンに画面遷移
	if (onButton && ins.IsTrgMouseLeft() || ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		StopSoundMem(bgmHandle_);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}


}

void TitleScene::Draw(void)
{
	// --- 背景 ---
	DrawGraph(0, 0, BgImage_, TRUE);
#pragma region タイトルロゴ文字の描画
	// --- タイトルロゴ文字の描画 ---
	if (fontHandle_ != -1) {
		const char* titleText = "問いのロード";
		int x = Application::SCREEN_SIZE_X / 2;
		int y = Application::SCREEN_SIZE_Y / 4.5;
		int color = GetColor(255, 255, 0); // 黄色
		int textWidth = GetDrawStringWidthToHandle(titleText, strlen(titleText), fontHandle_);

		DrawStringToHandle(x - textWidth / 2, y, titleText, color, fontHandle_); // 中央揃え
		int drawX = x - textWidth / 2;

		// 文字色と枠色を宣言
		int textColor = GetColor(255, 215, 100);   // 本体：黄色
		int outlineColor = GetColor(0, 0, 0);    // 枠線：黒

		// 文字幅取得（中央揃え用）
		
		// 枠描画（上下左右＋斜め4方向）
		for (int dx = -2; dx <= 2; dx++) {
			for (int dy = -2; dy <= 2; dy++) {
				if (dx == 0 && dy == 0) continue; // 本体の位置は除く
				DrawStringToHandle(drawX + dx, y + dy, titleText, outlineColor, fontHandle_);
			}
		}
		// 本体文字描画
		DrawStringToHandle(drawX, y, titleText, textColor, fontHandle_);
	}
#pragma endregion

#pragma region スタートボタンの描画
	// ===== 「物語を始める」ボタン（白地） =====
	int mx, my;
	GetMousePoint(&mx, &my);

	bool onButton =
		mx >= btnX_ && mx <= btnX_ + btnW_ &&
		my >= btnY_ && my <= btnY_ + btnH_;

	// 枠サイズ
	const int outer = 5; // 白枠

	// 色
	int white = GetColor(210, 195, 160);
	int black = GetColor(30, 15, 10);
//	int textColor = GetColor(255, 255, 255);
	int textColor = GetColor(235, 225, 200);

	// 外枠（白）
	DrawBox(
		btnX_, btnY_,
		btnX_ + btnW_, btnY_ + btnH_,
		white, TRUE
	);

	// 内枠（黒）
	DrawBox(
		btnX_ + outer, btnY_ + outer,
		btnX_ + btnW_ - outer, btnY_ + btnH_ - outer,
		black, TRUE
	);

	// 文字
	const char* text = "物語へ進む";
	int textW = GetDrawStringWidthToHandle(text, strlen(text), fontButton_);
	int textX = btnX_ + btnW_ / 2 - textW / 2;
	int textY = btnY_ + btnH_ / 2 - 35;

	DrawStringToHandle(textX, textY, text, textColor, fontButton_);
#pragma endregion

#pragma region ポーズボタンの描画
	// ===== ポーズUI（左上） =====

// 半透明ON
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);

	// 背景
	DrawBox(
		pauseX_, pauseY_,
		pauseX_ + pauseW_, pauseY_ + pauseH_,
		GetColor(0, 0, 0), TRUE
	);

	// 半透明OFF
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 枠
	DrawBox(
		pauseX_, pauseY_,
		pauseX_ + pauseW_, pauseY_ + pauseH_,
		GetColor(180, 180, 180), FALSE
	);

	// 文字
	DrawStringToHandle(
		pauseX_ + pauseW_ / 2 - 6,
		pauseY_ + pauseH_ / 2 - 8,
		"||",
		GetColor(230, 230, 230),
		fontPause_
	);
#pragma endregion

	if (state_ == TitleState::PAUSE)
	{
		pauseScene_.TitleDraw();
	}
}

void TitleScene::Release(void)
{
	// 画像解放
	DeleteGraph(BgImage_);

	// BGM解放
	StopSoundMem(bgmHandle_);
	DeleteSoundMem(bgmHandle_);

	// フォント解放
	if (fontHandle_ != -1) {
		DeleteFontToHandle(fontHandle_);
		fontHandle_ = -1;
	}
	if (fontButton_ != -1) {
		DeleteFontToHandle(fontButton_);
		fontButton_ = -1;
	}
	if (fontPause_ != -1) {
		DeleteFontToHandle(fontPause_);
		fontPause_ = -1;
	}
}
