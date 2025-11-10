#pragma once
#include <DxLib.h>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib> // mblen
#include "../Manager/InputManager.h"

class Message
{
	// メッセージを改行するごとに分割する関数
	
	// マルチバイト文字列から、指定文字数ぶん取り出す

public:
	// コンストラクタ
	Message(void);

	// デストラクタ
	~Message(void);

	// 初期化処理
	void Init(void);
	// 文字列
	void SetMessage(const std::string& message);
	// 更新ステップ
	void Update(void);
	// 描画処理
	void Draw(int x, int y);
	// 解放処理
	void Release(void);
	// 全て表示の確認
	bool IsFinished(void) const;
	// 文章を全て表示
	void Skip(void);
	// メッセージがあるかどうか
	bool HasMessage() const;
	// カーソルの描画を有効/無効にする (追加)
    void SetDrawCursor(bool enable);
private:
	// メッセージを改行するごとに分割する関数
	std::vector<std::string> SplitMessage(const std::string& message);
	// マルチバイト文字列から、指定文字数ぶん取り出す
	std::string SubstrMBCS(const std::string& src, int charCount);

#pragma region 変数宣言
	InputManager& inputManager_;

	// 改行ごとに分割した文章
	std::vector<std::string> messageLines_; 

	// 全文
	std::string fullText_;
	// 表示中の文章
	std::string displayText_;

	// 現在表示してる文字のインデックス
	int charIndex_;

	// 現在表示してる文字数
	int charCount_;
	// 現在表示してる行数
	int currentLine_;
	// フレーム経過カウンタ
	int frameCount_;
	// 文字表示速度
	int charSpeed_;
	// 全て表示したか
	bool finished_;
	// 点滅用カウンタ
	int blinkCounter_;
	// 次の文章へ進むまでの待機カウンタ
	int waitCounter_;
	// 次の文章へ進むフラグ
	bool next_;
	// スキップフラグ
	bool skipped_;
	// カーソル描画フラグ
	bool drawCursor_;
	// メッセージがセットされたフレーム (追加)
	int startFrame_;
};

