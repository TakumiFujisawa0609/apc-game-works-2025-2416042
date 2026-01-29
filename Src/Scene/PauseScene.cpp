#include <DxLib.h>
#include <vector>
#include <string>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/QuestionManager.h"
#include "PauseScene.h"

PauseScene::PauseScene(void)
	:inputManager_(InputManager::GetInstance()),
	pauseSelectIndex_(0),
	isLButtonDown_(false),
	isResume_(false),
	fontHandle_(-1),
	ignoreInput_(true)
{
}

PauseScene::~PauseScene(void)
{
}

void PauseScene::Init(void)
{
	// フォント作成
	fontHandle_ = CreateFontToHandle(
		"源ノ明朝",       // フォント名
		80,				  // サイズ
		10,				  // 太さ
		DX_FONTTYPE_ANTIALIASING // アンチエイリアス
	);
	// フォント作成
	PauseHandle_ = CreateFontToHandle(
		"源ノ明朝",       // フォント名
		200,				  // サイズ
		10,				  // 太さ
		DX_FONTTYPE_ANTIALIASING // アンチエイリアス
	);
}

void PauseScene::Update(void)
{

}

void PauseScene::TitleUpdate(void)
{
	// マウスカーソルを表示
	SetMouseDispFlag(TRUE);

	// マウス座標とボタン状態を取得
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	int mouseButton = GetMouseInput();
	bool isLButtonNow = (mouseButton & MOUSE_INPUT_LEFT);

	// 
	if (ignoreInput_)
	{
		// マウス左ボタンが離されたら入力無視解除
		if (!isLButtonNow)
		{
			ignoreInput_ = false;
		}
		// 入力無視中は、マウスの「前回の状態」だけ更新して抜ける
		isLButtonDown_ = isLButtonNow;
		return;
	}
	// マウス左ボタンのトリガー判定
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);
	bool isMouseOverChoice = false;

	if (inputManager_.IsTrgDown(KEY_INPUT_W))
		pauseSelectIndex_ = (pauseSelectIndex_ - 1 + 2) % 2;
	if (inputManager_.IsTrgDown(KEY_INPUT_S))
		pauseSelectIndex_ = (pauseSelectIndex_ + 1) % 2;

	{
		int newSelected = pauseSelectIndex_;

		// マウスオーバー判定
		for (size_t i = 0; i < choiceRects_.size(); ++i) {
			const auto& rect = choiceRects_[i];
			if (mouseX >= rect.left && mouseX <= rect.right &&
				mouseY >= rect.top && mouseY <= rect.bottom)
			{
				newSelected = (int)i;
				isMouseOverChoice = true;
				break;
			}
		}

		if (isMouseOverChoice) {
			pauseSelectIndex_ = newSelected;
		}

		// マウスクリック決定処理
		if (isLButtonTrg && isMouseOverChoice) {
			goto PAUSE_DECISION;
		}
	}

	// 選択肢決定 (キーボード操作)
	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
	{
		goto PAUSE_DECISION;
	}

	// 決定ロジックの開始点としてラベルを定義
PAUSE_DECISION:

	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || (isLButtonTrg && isMouseOverChoice))
	{
		switch (pauseSelectIndex_)
		{
		case 0: SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE); break;
		case 1: DxLib_End(); break;
		}
	}
}


void PauseScene::GameUpdate(void)
{
	if (ignoreInput_)
	{
		if (!InputManager::GetInstance().IsTrgMouseLeft())
		{
			ignoreInput_ = false;
		}
		return;
	}

	// マウスカーソルを表示
	SetMouseDispFlag(TRUE);

	// マウス座標とボタン状態を取得
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

	if (inputManager_.IsTrgDown(KEY_INPUT_W))
		pauseSelectIndex_ = (pauseSelectIndex_ - 1 + 3) % 3;
	if (inputManager_.IsTrgDown(KEY_INPUT_S))
		pauseSelectIndex_ = (pauseSelectIndex_ + 1) % 3;

	{
		int newSelected = pauseSelectIndex_;
		bool isMouseOverChoice = false;

		// マウスオーバー判定
		for (size_t i = 0; i < choiceRects_.size(); ++i) {
			const auto& rect = choiceRects_[i];
			if (mouseX >= rect.left && mouseX <= rect.right &&
				mouseY >= rect.top && mouseY <= rect.bottom)
			{
				newSelected = (int)i;
				isMouseOverChoice = true;
				break;
			}
		}

		if (isMouseOverChoice) {
			pauseSelectIndex_ = newSelected;
		}

		// マウスクリック決定処理
		if (isLButtonTrg && isMouseOverChoice) {
			goto PAUSE_DECISION;
		}
	}

	// 選択肢決定 (キーボード操作)
	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
	{
		goto PAUSE_DECISION;
	}

	// 決定ロジックの開始点としてラベルを定義
	PAUSE_DECISION:

	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
	{
		switch (pauseSelectIndex_)
		{
		case 0: isResume_ = true; 
			break;
		case 1: SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE); 
			break;
		case 2: DxLib_End();
			break;
		}
	}
}


void PauseScene::ClearUpdate(void)
{
	if (ignoreInput_)
	{
		if (!InputManager::GetInstance().IsTrgMouseLeft())
		{
			ignoreInput_ = false;
		}
		return;
	}

	// マウスカーソルを表示
	SetMouseDispFlag(TRUE);

	// マウス座標とボタン状態を取得
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

	if (inputManager_.IsTrgDown(KEY_INPUT_W))
		pauseSelectIndex_ = (pauseSelectIndex_ - 1 + 2) % 2;
	if (inputManager_.IsTrgDown(KEY_INPUT_S))
		pauseSelectIndex_ = (pauseSelectIndex_ + 1) % 2;

	{
		int newSelected = pauseSelectIndex_;
		bool isMouseOverChoice = false;

		// マウスオーバー判定
		for (size_t i = 0; i < choiceRects_.size(); ++i) {
			const auto& rect = choiceRects_[i];
			if (mouseX >= rect.left && mouseX <= rect.right &&
				mouseY >= rect.top && mouseY <= rect.bottom)
			{
				newSelected = (int)i;
				isMouseOverChoice = true;
				break;
			}
		}

		if (isMouseOverChoice) {
			pauseSelectIndex_ = newSelected;
		}

		// マウスクリック決定処理
		if (isLButtonTrg && isMouseOverChoice) {
			goto PAUSE_DECISION;
		}
	}

	// 選択肢決定 (キーボード操作)
	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
	{
		goto PAUSE_DECISION;
	}

	// ★追加: 決定ロジックの開始点としてラベルを定義
PAUSE_DECISION:

	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
	{
		switch (pauseSelectIndex_)
		{
		case 0: SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE); break;
		case 1: DxLib_End(); break;
		}
	}
}

void PauseScene::Draw(void)
{
}

void PauseScene::TitleDraw(void)
{
	// ハンドルが死んでいたらその場で作る（念のためのガード） ---
	if (fontHandle_ == -1) {
		Init();
	}

	// 画面を少し暗くする
	DrawBox(0, 0, 1920, 1080, GetColor(30, 35, 45), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150); // 半透明黒
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0,0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// --- メニュータイトルとラインの描画 ---
	int titleX = 706;
	int titleY = 80;
	std::string titleText = "PAUSE";
	unsigned int titleColor = GetColor(255, 0, 0);

	// ロゴの描画
	DrawStringToHandle(titleX, titleY, titleText.c_str(), titleColor, PauseHandle_);

	// ロゴの下に線を引く
	int titleWidth = GetDrawStringWidthToHandle(titleText.c_str(), (int)titleText.size(), PauseHandle_);
	int lineY = titleY + 205; // 文字の下端に合わせて調整（サイズ200なので少し下めに）
	// ロゴの左端から右端まで線を引く（太さ5）
	DrawLine(titleX, lineY, titleX + titleWidth, lineY, titleColor, 15);
	// ------------------------------------

	const std::vector<std::string> PAUSE_MENU = { "タイトルに戻る", "ゲーム終了" };
	int startX = 670;
	int startY = 460;
	int spacing = 180;
	int choiceHeight = 100; // 文字の高さ+余裕

	//  矩形リストをクリアし、設定を初期化
	choiceRects_.clear();
	//SetFontSize(50);

	for (size_t i = 0; i < PAUSE_MENU.size(); i++)
	{
		int currentY = startY + (int)i * spacing;
		const std::string& text = PAUSE_MENU[i];

		int drawX = startX;

		int color = (i == pauseSelectIndex_) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

		// 選んでいるやつには矢印を表示
		if (i == pauseSelectIndex_)
		{
			drawX -= 50; // 選んでいるやつは少し左に出す
			// 左側に矢印を表示
			DrawStringToHandle(drawX - 95, currentY, "＞", color, fontHandle_);
		}

		// 文字列の幅を取得 (DxLib)
		int choiceWidth = GetDrawStringWidthToHandle(text.c_str(), (int)text.size(), fontHandle_);

		// 選択時の背景描画
		if (i == pauseSelectIndex_)
		{
			DrawBox(drawX - 10, currentY - 5, drawX + choiceWidth + 10, currentY + choiceHeight + 5, GetColor(50, 50, 50), TRUE);
		}

		// 描画
		DrawStringToHandle(drawX, currentY, text.c_str(), color, fontHandle_);

		// 矩形を保存
		choiceRects_.push_back({
			drawX - 10,
			currentY - 5,
			drawX + choiceWidth + 10,
			currentY + choiceHeight + 5
			});
	}
}


void PauseScene::GameDraw(void)
{
	if (fontHandle_ == -1) Init();

	// 背景
	DrawBox(0, 0, 1920, 1080, GetColor(30, 35, 45), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// タイトルロゴとライン
	int titleX = 706;
	int titleY = 80;
	std::string titleText = "PAUSE";
	unsigned int titleColor = GetColor(255, 0, 0);
	DrawStringToHandle(titleX, titleY, titleText.c_str(), titleColor, PauseHandle_);
	int titleWidth = GetDrawStringWidthToHandle(titleText.c_str(), (int)titleText.size(), PauseHandle_);
	int lineY = titleY + 205;
	DrawLine(titleX, lineY, titleX + titleWidth, lineY, titleColor, 15);

	// メニュー項目（3つ）
	const std::vector<std::string> PAUSE_MENU = { "ゲームに戻る", "タイトルに戻る", "ゲーム終了" };
	int startX = 670;
	int startY = 400; // 3項目あるので少し上に調整
	int spacing = 180;
	int choiceHeight = 100;

	choiceRects_.clear();

	for (size_t i = 0; i < PAUSE_MENU.size(); i++)
	{
		int currentY = startY + (int)i * spacing;
		const std::string& text = PAUSE_MENU[i];
		int drawX = startX;
		int color = (i == pauseSelectIndex_) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

		if (i == pauseSelectIndex_)
		{
			drawX -= 50;
			DrawStringToHandle(drawX - 95, currentY, "＞", color, fontHandle_);
		}

		int choiceWidth = GetDrawStringWidthToHandle(text.c_str(), (int)text.size(), fontHandle_);

		if (i == pauseSelectIndex_)
		{
			DrawBox(drawX - 10, currentY - 5, drawX + choiceWidth + 10, currentY + choiceHeight + 5, GetColor(50, 50, 50), TRUE);
		}

		DrawStringToHandle(drawX, currentY, text.c_str(), color, fontHandle_);

		choiceRects_.push_back({
			drawX - 10, currentY - 5,
			drawX + choiceWidth + 10, currentY + choiceHeight + 5
			});
	}
}

void PauseScene::ClearDraw(void)
{
	if (fontHandle_ == -1) Init();

	// 背景
	DrawBox(0, 0, 1920, 1080, GetColor(30, 35, 45), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// タイトルロゴとライン
	int titleX = 706;
	int titleY = 80;
	std::string titleText = "PAUSE";
	unsigned int titleColor = GetColor(255, 0, 0);
	DrawStringToHandle(titleX, titleY, titleText.c_str(), titleColor, PauseHandle_);
	int titleWidth = GetDrawStringWidthToHandle(titleText.c_str(), (int)titleText.size(), PauseHandle_);
	int lineY = titleY + 205;
	DrawLine(titleX, lineY, titleX + titleWidth, lineY, titleColor, 15);

	// メニュー項目（2つ）
	const std::vector<std::string> PAUSE_MENU = { "タイトルに戻る", "ゲーム終了" };
	int startX = 670;
	int startY = 460;
	int spacing = 180;
	int choiceHeight = 100;

	choiceRects_.clear();

	for (size_t i = 0; i < PAUSE_MENU.size(); i++)
	{
		int currentY = startY + (int)i * spacing;
		const std::string& text = PAUSE_MENU[i];
		int drawX = startX;
		int color = (i == pauseSelectIndex_) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

		if (i == pauseSelectIndex_)
		{
			drawX -= 50;
			DrawStringToHandle(drawX - 95, currentY, "＞", color, fontHandle_);
		}

		int choiceWidth = GetDrawStringWidthToHandle(text.c_str(), (int)text.size(), fontHandle_);

		if (i == pauseSelectIndex_)
		{
			DrawBox(drawX - 10, currentY - 5, drawX + choiceWidth + 10, currentY + choiceHeight + 5, GetColor(50, 50, 50), TRUE);
		}

		DrawStringToHandle(drawX, currentY, text.c_str(), color, fontHandle_);

		choiceRects_.push_back({
			drawX - 10, currentY - 5,
			drawX + choiceWidth + 10, currentY + choiceHeight + 5
			});
	}
}
void PauseScene::Release(void)
{
	// フォント解放
	if (fontHandle_ != -1) {
		DeleteFontToHandle(fontHandle_);
		fontHandle_ = -1;
	}
	if (PauseHandle_ != -1) {
		DeleteFontToHandle(PauseHandle_);
		PauseHandle_ = -1;
	}
}

bool PauseScene::IsResume(void)
{
	return isResume_;
}

void PauseScene::ResetResume(void)
{
	isResume_ = false;
}

void PauseScene::SetResume(bool resume)
{
	isResume_ = resume;
}

void PauseScene::OnEnter(void)
{
	ignoreInput_ = true;
	isLButtonDown_ = (GetMouseInput() & MOUSE_INPUT_LEFT);
}

PauseScene& PauseScene::GetInstance(void)
{
	// C++におけるシングルトンの典型的な実装
	static PauseScene instance;
	return instance;
}