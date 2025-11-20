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
	isLButtonDown_(false)
{
}

PauseScene::~PauseScene(void)
{
}

void PauseScene::Init(void)
{
}

void PauseScene::Update(void)
{
	
}

void PauseScene::GameUpdate(void)
{
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

	// ★追加: 決定ロジックの開始点としてラベルを定義
	PAUSE_DECISION:

	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
	{
		switch (pauseSelectIndex_)
		{
		case 0:SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME); break;
		case 1: SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE); break;
		case 2: DxLib_End(); break;
		}
	}
}

void PauseScene::TitleUpdate(void)
{
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

void PauseScene::ClearUpdate(void)
{
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

void PauseScene::GameDraw(void)
{
	// 画面を少し暗くする
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150); // 半透明黒
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	SetFontSize(100);
	// メニュータイトル
	DrawString(700, 300, "ポーズ中", GetColor(255, 255, 0));
	const std::vector<std::string> PAUSE_MENU = { "ゲームに戻る", "タイトルに戻る", "ゲーム終了" };
	int startX = 710;
	int startY = 500;
	int spacing = 100;
	int choiceHeight = 60; // 文字の高さ+余裕

	// ★追加: 矩形リストをクリアし、設定を初期化
	choiceRects_.clear();
	SetFontSize(50);

	for (size_t i = 0; i < PAUSE_MENU.size(); i++)
	{
		int currentY = startY + (int)i * spacing;
		int color = (i == pauseSelectIndex_) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
		const std::string& text = PAUSE_MENU[i];

		// 文字列の幅を取得 (DxLib)
		int choiceWidth = GetDrawStringWidth(text.c_str(), (int)text.size());

		// 選択時の背景描画
		if (i == pauseSelectIndex_)
		{
			DrawBox(startX - 10, currentY - 5, startX + choiceWidth + 10, currentY + choiceHeight + 5, GetColor(50, 50, 50), TRUE);
		}

		// 描画
		DrawString(startX, currentY, text.c_str(), color);

		// ★追加: 矩形を保存
		choiceRects_.push_back({
			startX - 10,
			currentY - 5,
			startX + choiceWidth + 10,
			currentY + choiceHeight + 5
			});
	}

	// ポーズメニューの操作ヒント
	SetFontSize(60);
	DrawFormatString(0, 1000, GetColor(255, 255, 0), "W/Sキー or マウスで操作、Spaceまたはクリックで選択");

}

void PauseScene::TitleDraw(void)
{
	// 画面を少し暗くする
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150); // 半透明黒
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	SetFontSize(100);
	// メニュータイトル
	DrawString(700, 300, "ポーズ中", GetColor(255, 255, 0));
	const std::vector<std::string> PAUSE_MENU = { "タイトルに戻る", "ゲーム終了" };
	int startX = 710;
	int startY = 500;
	int spacing = 100;
	int choiceHeight = 60; // 文字の高さ+余裕

	// ★追加: 矩形リストをクリアし、設定を初期化
	choiceRects_.clear();
	SetFontSize(50);

	for (size_t i = 0; i < PAUSE_MENU.size(); i++)
	{
		int currentY = startY + (int)i * spacing;
		int color = (i == pauseSelectIndex_) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
		const std::string& text = PAUSE_MENU[i];

		// 文字列の幅を取得 (DxLib)
		int choiceWidth = GetDrawStringWidth(text.c_str(), (int)text.size());

		// 選択時の背景描画
		if (i == pauseSelectIndex_)
		{
			DrawBox(startX - 10, currentY - 5, startX + choiceWidth + 10, currentY + choiceHeight + 5, GetColor(50, 50, 50), TRUE);
		}

		// 描画
		DrawString(startX, currentY, text.c_str(), color);

		// ★追加: 矩形を保存
		choiceRects_.push_back({
			startX - 10,
			currentY - 5,
			startX + choiceWidth + 10,
			currentY + choiceHeight + 5
			});
	}

	// ポーズメニューの操作ヒント
	SetFontSize(60);
	DrawFormatString(0, 1000, GetColor(255, 255, 0), "W/Sキー or マウスで操作、Spaceまたはクリックで選択");

}

void PauseScene::ClearDraw(void)
{
	// 画面を少し暗くする
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150); // 半透明黒
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	SetFontSize(100);
	// メニュータイトル
	DrawString(700, 300, "ポーズ中", GetColor(255, 255, 0));
	const std::vector<std::string> PAUSE_MENU = { "タイトルに戻る", "ゲーム終了" };
	int startX = 710;
	int startY = 500;
	int spacing = 100;
	int choiceHeight = 60; // 文字の高さ+余裕

	// ★追加: 矩形リストをクリアし、設定を初期化
	choiceRects_.clear();
	SetFontSize(50);

	for (size_t i = 0; i < PAUSE_MENU.size(); i++)
	{
		int currentY = startY + (int)i * spacing;
		int color = (i == pauseSelectIndex_) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
		const std::string& text = PAUSE_MENU[i];

		// 文字列の幅を取得 (DxLib)
		int choiceWidth = GetDrawStringWidth(text.c_str(), (int)text.size());

		// 選択時の背景描画
		if (i == pauseSelectIndex_)
		{
			DrawBox(startX - 10, currentY - 5, startX + choiceWidth + 10, currentY + choiceHeight + 5, GetColor(50, 50, 50), TRUE);
		}

		// 描画
		DrawString(startX, currentY, text.c_str(), color);

		// ★追加: 矩形を保存
		choiceRects_.push_back({
			startX - 10,
			currentY - 5,
			startX + choiceWidth + 10,
			currentY + choiceHeight + 5
			});
	}

	// ポーズメニューの操作ヒント
	SetFontSize(60);
	DrawFormatString(0, 1000, GetColor(255, 255, 0), "W/Sキー or マウスで操作、Spaceまたはクリックで選択");

}

void PauseScene::Release(void)
{
}

PauseScene& PauseScene::GetInstance(void)
{
	// C++におけるシングルトンの典型的な実装
	static PauseScene instance;
	return instance;
}