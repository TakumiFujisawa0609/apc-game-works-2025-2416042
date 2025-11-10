#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "Message.h"

Message::Message(void)
	:inputManager_(InputManager::GetInstance())
{
}

Message::~Message(void)
{
}

void Message::Init(void)
{
	// 変数の初期化
	currentLine_ = 0;		// 現在の行数
	charCount_ = 0;		// 現在の文字数
	frameCount_ = 0;		// フレームカウント
	charSpeed_ = 1;		// 文字表示速度（フレーム数）
	finished_ = true;		// 全て表示したか
	blinkCounter_ = 0;		// 点滅用カウンタ
	skipped_ = false;
	drawCursor_ = true;	// カーソル表示の初期化
	startFrame_ = 0;		// 開始フレームの初期化
}

void Message::SetMessage(const std::string& message)
{
	// メッセージを分割して保存
	messageLines_ = SplitMessage(message);
	currentLine_ = 0;		// 現在の行数
	charCount_ = 0;		 // 現在の文字数
	frameCount_ = 0;		 // フレームカウント
	finished_ = false;		// 全て表示したか
	skipped_ = false;		// スキップフラグ


}

void Message::Update(void)
{
	if (finished_ || messageLines_.empty()) return;

	// スペースキーが押されたら全文を即表示
	if (!skipped_ && !finished_&&  inputManager_.IsTrgDown(KEY_INPUT_SPACE))
	{
		// 全ての行を最後まで表示状態にする
		currentLine_ = (int)messageLines_.size() - 1;
		charCount_ = (int)messageLines_.back().size();
		skipped_ = true;
		return;
	}

	if (finished_) return;

	// 一定時間ごとに1文字ずつ表示
	frameCount_++;
	if (frameCount_ >= charSpeed_) {
		frameCount_ = 0;

		// 文字がまだ残ってるなら次を表示
		if (charCount_ < (int)messageLines_[currentLine_].size()) 
		{
			charCount_++;
		}
		else if (currentLine_ + 1 < (int)messageLines_.size())
		{
			// 次の行に移る
			currentLine_++;
			charCount_ = 0;
		}
		else 
		{
			finished_ = true;
		}
	}

}

void Message::SetDrawCursor(bool enable)
{
	drawCursor_ = enable;
}


void Message::Draw(int x, int y)
{
	// フォントのサイズ
	SetFontSize(50);

	int drawX = x;
	int drawY = y;

	for (int line = 0; line <= currentLine_ && line < (int)messageLines_.size(); line++){
		std::string visibleText;
		if (line < currentLine_) {
			// すでに確定した行は全表示
			visibleText = messageLines_[line];
		}
		else {
			visibleText = SubstrMBCS(messageLines_[line], charCount_);
		}

		DrawFormatString(drawX, drawY, GetColor(255, 255, 255), visibleText.c_str());
			drawY += 60; // 行間
	}

	if (finished_ && drawCursor_) {
		// 点滅用にカウンタ
		blinkCounter_++;

		if ((blinkCounter_ / 30) % 2 == 0) {
			// 点滅表示
			SetFontSize(40);
			DrawFormatString(drawX + 1460, 250, GetColor(255, 255, 255), "SPACE");
			
			SetFontSize(50);
		}
	}
#pragma region デバック
//	// デバッグ表示
//	DrawFormatString(50, 400, GetColor(255, 0, 0),
//		"frame=%d  charCount=%d / %d",
//		frameCount_, charCount_, (int)messageLines_.size());
//#pragma endregion
}

void Message::Release(void)
{
}

bool Message::IsFinished(void) const
{
	return finished_;
}

void Message::Skip(void)
{
	// 全文を一気に表示する
	displayText_ = fullText_;
	charIndex_ = static_cast<int>(fullText_.size());
	// 表示完了フラグ
	finished_ = true;
}

bool Message::HasMessage() const
{
	return !messageLines_.empty();
}

std::vector<std::string> Message::SplitMessage(const std::string& message)
{
	std::vector<std::string> result;
	std::stringstream ss(message);
	std::string line;

	while (std::getline(ss, line, '\n')) {
		result.push_back(line);
	}
	return result;
}

std::string Message::SubstrMBCS(const std::string& src, int charCount)
{
	std::string result;
	const char* p = src.c_str();
	int len = (int)src.size();
	int i = 0;

	while (i < len && charCount > 0) {
		int bytes = mblen(p, MB_CUR_MAX);	// 1文字が何バイトか取得
		if (bytes <= 0) break;							// エラー or 終端
		result.append(p, bytes);						// そのバイト数ぶんコピー
		p += bytes;
		i += bytes;
		charCount--;
	}
	return result;
}
