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
	fontButton_(-1),
	pauseX_(0),
	pauseY_(0),
	pauseW_(0),
	pauseH_(0),
	fontPause_(-1),
	fontEscape_(-1),
	buttonAnimTime_(0.0f),
	buttonScale_(1.0f)
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
		4,					  // 太さ
		DX_FONTTYPE_ANTIALIASING // アンチエイリアス
	);
	// ESC用フォント作成
	fontEscape_ = CreateFontToHandle(
		"源ノ明朝",       // フォント名
		20,				  // サイズ
		10,				  // 太さ
		DX_FONTTYPE_ANTIALIASING // アンチエイリアス
	);
	// ポーズボタン用フォント作成
	fontPause_ = CreateFontToHandle(
		"遊ゴシック",       // フォント名
		35,					 // サイズ
		9,						 // 太さ
		DX_FONTTYPE_ANTIALIASING // アンチエイリアス
	);
	

	// スタートボタン設定
	btnW_ = 700;
	btnH_ = 150;
	btnX_ = Application::SCREEN_SIZE_X / 2 - btnW_ / 2;
	btnY_ = Application::SCREEN_SIZE_Y * 2 / 3;

	// ポーズメニューボタン
	pauseX_ = 0;
	pauseY_ = 0;
	pauseW_ = 60;
	pauseH_ = 60;
}

//void TitleScene::Update(void)
//{
//	// シーン遷移
//	InputManager& ins = InputManager::GetInstance();
//	// マウスカーソルを表示
//	SetMouseDispFlag(TRUE);
//
//	// --- ポーズメニュー中の処理 ---
//	if (state_ == TitleState::PAUSE)
//	{
//		pauseScene_.TitleUpdate();
//		// ポーズメニュー終了判定
//		if (ins.IsTrgDown(KEY_INPUT_TAB) || 
//			ins.IsTrgDown(KEY_INPUT_ESCAPE))
//		{
//			state_ = TitleState::NORMAL;
//		}
//		return; // ポーズメニュー中は以降の処理を行わない
//	}
//
//   // 通常時（ここで初めて ESC を見る）
//	if (ins.IsTrgDown(KEY_INPUT_ESCAPE) ||
//		ins.IsTrgDown(KEY_INPUT_TAB))
//	{
//		state_ = TitleState::PAUSE;
//		pauseScene_.OnEnter();
//		return;
//	}
//
//	// マウス位置取得
//	int mx, my;
//	GetMousePoint(&mx, &my);
//	// ポーズボタン上にマウスがあるか判定
//	bool onPause =
//		mx >= pauseX_ && mx <= pauseX_ + pauseW_ &&
//		my >= pauseY_ && my <= pauseY_ + pauseH_;
//
//	if (onPause && ins.IsTrgMouseLeft())
//	{
//		state_ = TitleState::PAUSE;
//		pauseScene_.OnEnter();
//		return;
//	}
//
//	// ボタン上にマウスがあるか判定 
//	bool onButton =
//		mx >= btnX_ && mx <= btnX_ + btnW_ &&
//		my >= btnY_ && my <= btnY_ + btnH_;
//
//	// ボタンクリック またはスペースキーでゲームシーンに画面遷移
//	if (onButton && ins.IsTrgMouseLeft() || ins.IsTrgDown(KEY_INPUT_SPACE))
//	{
//		StopSoundMem(bgmHandle_);
//		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
//	}
//
//
//}
void TitleScene::Update(void)
{
	InputManager& ins = InputManager::GetInstance();
	SetMouseDispFlag(TRUE);

	// ===== ポーズ中 =====
	if (state_ == TitleState::PAUSE)
	{
		pauseScene_.TitleUpdate();

		if (ins.IsTrgDown(KEY_INPUT_TAB) ||
			ins.IsTrgDown(KEY_INPUT_ESCAPE))
		{
			state_ = TitleState::NORMAL;
		}
		return;
	}

	// ===== 通常時 =====
	if (ins.IsTrgDown(KEY_INPUT_ESCAPE) ||
		ins.IsTrgDown(KEY_INPUT_TAB))
	{
		state_ = TitleState::PAUSE;
		pauseScene_.OnEnter();
		return;
	}

	// ===== スタートボタン拡縮アニメ =====
	buttonAnimTime_ += 0.05f;   // 速度
	buttonScale_ = 1.0f + sinf(buttonAnimTime_) * 0.06f;

	// ===== マウス位置 =====
	int mx, my;
	GetMousePoint(&mx, &my);

	// ===== ポーズボタン判定 =====
	bool onPause =
		mx >= pauseX_ && mx <= pauseX_ + pauseW_ &&
		my >= pauseY_ && my <= pauseY_ + pauseH_;

	if (onPause && ins.IsTrgMouseLeft())
	{
		state_ = TitleState::PAUSE;
		pauseScene_.OnEnter();
		return;
	}

	// ===== 拡縮後サイズでボタン判定 =====
	int drawW = static_cast<int>(btnW_ * buttonScale_);
	int drawH = static_cast<int>(btnH_ * buttonScale_);
	int drawX = btnX_ + btnW_ / 2 - drawW / 2;
	int drawY = btnY_ + btnH_ / 2 - drawH / 2;

	bool onButton =
		mx >= drawX && mx <= drawX + drawW &&
		my >= drawY && my <= drawY + drawH;

	// ===== 遷移 =====
	if ((onButton && ins.IsTrgMouseLeft()) ||
		ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		StopSoundMem(bgmHandle_);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}


//void TitleScene::Draw(void)
//{
//	// --- 背景 ---
//	DrawGraph(0, 0, BgImage_, TRUE);
//#pragma region タイトルロゴ文字の描画
//	// --- タイトルロゴ文字の描画 ---
//	if (fontHandle_ != -1) {
//		const char* titleText = "問いのロード";
//		int x = Application::SCREEN_SIZE_X / 2;
//		int y = Application::SCREEN_SIZE_Y / 4.5;
//		int color = GetColor(255, 255, 0); // 黄色
//		int textWidth = GetDrawStringWidthToHandle(titleText, strlen(titleText), fontHandle_);
//
//		DrawStringToHandle(x - textWidth / 2, y, titleText, color, fontHandle_); // 中央揃え
//		int drawX = x - textWidth / 2;
//
//		// 文字色と枠色を宣言
//		int textColor = GetColor(255, 215, 100);   // 本体：黄色
//		int outlineColor = GetColor(0, 0, 0);    // 枠線：黒
//
//		// 文字幅取得（中央揃え用）
//		
//		// 枠描画（上下左右＋斜め4方向）
//		for (int dx = -2; dx <= 2; dx++) {
//			for (int dy = -2; dy <= 2; dy++) {
//				if (dx == 0 && dy == 0) continue; // 本体の位置は除く
//				DrawStringToHandle(drawX + dx, y + dy, titleText, outlineColor, fontHandle_);
//			}
//		}
//		// 本体文字描画
//		DrawStringToHandle(drawX, y, titleText, textColor, fontHandle_);
//	}
//#pragma endregion
//
//#pragma region スタートボタンの描画
//	// ===== 「物語を始める」ボタン（白地） =====
//	int mx, my;
//	GetMousePoint(&mx, &my);
//
//	bool onButton =
//		mx >= btnX_ && mx <= btnX_ + btnW_ &&
//		my >= btnY_ && my <= btnY_ + btnH_;
//
//	// 枠サイズ
//	const int outer = 5; // 白枠
//
//	// 色
//	int white = GetColor(210, 195, 160);
//	int black = GetColor(30, 15, 10);
////	int textColor = GetColor(255, 255, 255);
//	int textColor = GetColor(235, 225, 200);
//
//	// 外枠（白）
//	DrawBox(
//		btnX_, btnY_,
//		btnX_ + btnW_, btnY_ + btnH_,
//		white, TRUE
//	);
//
//	// 内枠（黒）
//	DrawBox(
//		btnX_ + outer, btnY_ + outer,
//		btnX_ + btnW_ - outer, btnY_ + btnH_ - outer,
//		black, TRUE
//	);
//
//	// 文字
//	const char* text = "物語へ進む";
//	int textW = GetDrawStringWidthToHandle(text, strlen(text), fontButton_);
//	int textX = btnX_ + btnW_ / 2 - textW / 2;
//	int textY = btnY_ + btnH_ / 2 - 35;
//
//	DrawStringToHandle(textX, textY, text, textColor, fontButton_);
//#pragma endregion
//
//#pragma region ポーズボタンの描画
//	// マウス判定（mx, myは上のボタン描画で取得済みのものを使用）
//	bool onPause = mx >= pauseX_ && mx <= pauseX_ + pauseW_ &&
//		my >= pauseY_ && my <= pauseY_ + pauseH_;
//
//	// 枠の太さ
//	const int thickness = 6;
//
//	// 1.外側の枠
//	int frameColor = onPause ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
//	DrawBox(pauseX_, pauseY_, pauseX_ + pauseW_, pauseY_ + pauseH_, frameColor, TRUE);
//
//	// 2. 内側の黒
//	DrawBox(
//		pauseX_ + thickness,
//		pauseY_ + thickness,
//		pauseX_ + pauseW_ - thickness,
//		pauseY_ + pauseH_ - thickness,
//		GetColor(0, 0, 0), TRUE
//	);
//
//	// 3. 文字
//	const char* pauseSymbol = "||";
//	int tw = GetDrawStringWidthToHandle(pauseSymbol, (int)strlen(pauseSymbol), fontPause_);
//
//	// 中央揃えの計算
//	int tx = pauseX_ + (pauseW_ - tw) / 2;
//	int ty = pauseY_ + (pauseH_ - 40) / 2;
//
//	// ESC文字
//	const char* escText = "ESC";
//	int escTw = GetDrawStringWidthToHandle(escText, (int)strlen(escText), fontEscape_);
//	int escTx = pauseX_ + (pauseW_ / 2) - (escTw / 2); // ボタンの中央に合わせる
//	int escTy = pauseY_ + pauseH_ + 5;
//
//	DrawStringToHandle(tx, ty, pauseSymbol, GetColor(255, 255, 255), fontPause_);
//	DrawStringToHandle(escTx, escTy, escText, GetColor(200, 200, 200), fontEscape_);
//#pragma endregion
//	if (state_ == TitleState::PAUSE)
//	{
//		pauseScene_.TitleDraw();
//	}
//}
void TitleScene::Draw(void)
{
	// ===== 背景 =====
	DrawGraph(0, 0, BgImage_, TRUE);

	// ===== タイトル文字（既存） =====
	const char* titleText = "問いのロード";
	int titleX = Application::SCREEN_SIZE_X / 2;
	int titleY = Application::SCREEN_SIZE_Y / 4.5;

	int titleW = GetDrawStringWidthToHandle(titleText, strlen(titleText), fontHandle_);
	int drawX = titleX - titleW / 2;

	for (int dx = -2; dx <= 2; dx++)
	{
		for (int dy = -2; dy <= 2; dy++)
		{
			if (dx == 0 && dy == 0) continue;
			DrawStringToHandle(drawX + dx, titleY + dy, titleText, GetColor(0, 0, 0), fontHandle_);
		}
	}

	DrawStringToHandle(drawX, titleY, titleText, GetColor(255, 215, 100), fontHandle_);

#pragma region スタートボタンの描画（拡縮＋ホバー色）

	int mx, my;
	GetMousePoint(&mx, &my);

	// --- マウスがボタン上か ---
	bool onButton =
		mx >= btnX_ && mx <= btnX_ + btnW_ &&
		my >= btnY_ && my <= btnY_ + btnH_;

	// --- 拡大縮小アニメ ---
	static float animTime = 0.0f;
	animTime += 0.05f;

	float scale = 1.0f + sinf(animTime) * 0.03f;
	if (onButton)
	{
		scale += 0.05f;
	}

	// --- 拡大後サイズ ---
	int drawW = static_cast<int>(btnW_ * scale);
	int drawH = static_cast<int>(btnH_ * scale);

	int drawBX = btnX_ + (btnW_ - drawW) / 2;
	int drawBY = btnY_ + (btnH_ - drawH) / 2;

	// --- 色 ---
	int outerColor = onButton
		? GetColor(220, 60, 60)     // 赤（外枠）
		: GetColor(210, 195, 160);  // 通常

	int innerColor = GetColor(30, 15, 10); // 黒は固定

	int textColor = onButton
		? GetColor(255, 200, 200)
		: GetColor(235, 225, 200);

	// --- 枠描画（順番が命） ---
	const int outer = 5;

	// ① 外枠（赤 or 白）
	DrawBox(
		drawBX, drawBY,
		drawBX + drawW, drawBY + drawH,
		outerColor,
		TRUE
	);

	// ② 内側（黒）
	DrawBox(
		drawBX + outer, drawBY + outer,
		drawBX + drawW - outer, drawBY + drawH - outer,
		innerColor,
		TRUE
	);

	// --- ③ 文字 ---
	const char* text = "物語へ進む";
	int textW = GetDrawStringWidthToHandle(text, (int)strlen(text), fontButton_);
	int textX = drawBX + drawW / 2 - textW / 2;
	int textY = drawBY + drawH / 2 - 35;

	DrawStringToHandle(
		textX,
		textY,
		text,
		textColor,
		fontButton_
	);

#pragma endregion



#pragma region ポーズボタンの描画

		// マウス判定（mx, myは上のボタン描画で取得済みのものを使用）
		bool onPause = mx >= pauseX_ && mx <= pauseX_ + pauseW_ &&
			my >= pauseY_ && my <= pauseY_ + pauseH_;
	
		// 枠の太さ
		const int thickness = 6;
	
		// 1.外側の枠
		int frameColor = onPause ? GetColor(220, 60, 60) : GetColor(255, 255, 255);
		DrawBox(pauseX_, pauseY_, pauseX_ + pauseW_, pauseY_ + pauseH_, frameColor, TRUE);
	
		// 2. 内側の黒
		DrawBox(
			pauseX_ + thickness,
			pauseY_ + thickness,
			pauseX_ + pauseW_ - thickness,
			pauseY_ + pauseH_ - thickness,
			GetColor(0, 0, 0), TRUE
		);
	
		// 3. 文字
		const char* pauseSymbol = "||";
		int tw = GetDrawStringWidthToHandle(pauseSymbol, (int)strlen(pauseSymbol), fontPause_);
	
		// 中央揃えの計算
		int tx = pauseX_ + (pauseW_ - tw) / 2;
		int ty = pauseY_ + (pauseH_ - 40) / 2;
	
		// ESC文字
		const char* escText = "ESC";
		int escTw = GetDrawStringWidthToHandle(escText, (int)strlen(escText), fontEscape_);
		int escTx = pauseX_ + (pauseW_ / 2) - (escTw / 2); // ボタンの中央に合わせる
		int escTy = pauseY_ + pauseH_ + 5;
	
		DrawStringToHandle(tx, ty, pauseSymbol, GetColor(255, 255, 255), fontPause_);
		DrawStringToHandle(escTx, escTy, escText, GetColor(200, 200, 200), fontEscape_);
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
	if (fontEscape_ != -1) {
		DeleteFontToHandle(fontEscape_);
		fontEscape_ = -1;
	}
}
