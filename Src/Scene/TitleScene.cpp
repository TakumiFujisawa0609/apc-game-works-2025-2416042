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
	// 画像読み込み
	BgImage_ = LoadGraph((Application::PATH_IMAGE + "Title3.png").c_str());
	
	// BGM読み込み・再生
	bgmHandle_ = LoadSoundMem((Application::PATH_DATA + "BGM/TitleScene.mp3").c_str());
	PlaySoundMem(bgmHandle_, DX_PLAYTYPE_LOOP);

	// 動画読み込み
	movieHandle_ = LoadGraph((Application::PATH_DATA + "Movie/問いのロード_紹介動画.mp4").c_str());

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

void TitleScene::Update(void)
{
	InputManager& ins = InputManager::GetInstance();
	SetMouseDispFlag(TRUE);

	// ===== 動画再生中の処理 =====
	if (state_ == TitleState::MOVIE)
	{
		// キー入力があったか、動画が終了(0)したら通常に戻る
		if (ins.IsTrgDown(KEY_INPUT_ESCAPE) || ins.IsTrgDown(KEY_INPUT_SPACE) ||
			ins.IsTrgMouseLeft() || GetMovieStateToGraph(movieHandle_) == 0)
		{
			PauseMovieToGraph(movieHandle_);  // 一時停止
			SeekMovieToGraph(movieHandle_, 0); // 先頭に戻す
			state_ = TitleState::NORMAL;
			idleTimer_ = 0;
			PlaySoundMem(bgmHandle_, DX_PLAYTYPE_LOOP); // BGM再開
		}
		return;
	}

	// ===== ポーズ中 =====
	if (state_ == TitleState::PAUSE)
	{
		pauseScene_.TitleUpdate();

		if (ins.IsTrgDown(KEY_INPUT_ESCAPE))
		{
			state_ = TitleState::NORMAL;
		}
		return;
	}

	// ===== 通常時 =====
	if (ins.IsTrgDown(KEY_INPUT_ESCAPE))
	{
		state_ = TitleState::PAUSE;
		pauseScene_.OnEnter();
		return;
	}

	// ===== 通常時の放置タイマー監視 =====
	int mx1, my1;
	GetMousePoint(&mx1, &my1);
	static int lastMx = 0, lastMy = 0;

	// マウス移動、または何らかのキー入力でタイマーリセット
	if (mx1 != lastMx || my1 != lastMy || CheckHitKeyAll() > 0)
	{
		idleTimer_ = 0;
	}
	else
	{
		idleTimer_++;
	}
	lastMx = mx1; lastMy = my1;

	// 一定時間経過で動画再生開始
	if (idleTimer_ >= IDLE_LIMIT)
	{
		state_ = TitleState::MOVIE;
		StopSoundMem(bgmHandle_);         // BGMを止める
		PlayMovieToGraph(movieHandle_);   // 動画再生開始
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


void TitleScene::Draw(void)
{
	// ===== 動画再生時 =====
	if (state_ == TitleState::MOVIE)
	{
		// 画面いっぱいに描画
		DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, movieHandle_, FALSE);
		return;
	}

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
		? GetColor(255, 255, 0)     // 赤（外枠）
		: GetColor(210, 195, 160);  // 通常

	int innerColor = GetColor(30, 15, 10); // 黒は固定

	int textColor = onButton
		? GetColor(255, 255, 200)
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
		int frameColor = onPause ? GetColor(255, 255, 0) : GetColor(235, 225, 200);
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
	
		DrawStringToHandle(tx, ty, pauseSymbol, GetColor(255, 255, 200), fontPause_);
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
	DeleteGraph(movieHandle_);

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
