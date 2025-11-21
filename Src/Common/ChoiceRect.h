#pragma once

// 選択肢の当たり判定用の矩形構造体（全シーン共通）
struct ChoiceRect
{
    int left;
    int top;
    int right;
    int bottom;
};

enum class SceneState
{
	STORY,				// メッセージ表示
	QUESTION,		// 問題表示
	ANSWER_TALK, // 解答後の会話
	RESULT,			// 結果表示
	END,					// シーン終了 〇
	PAUSE,				// 一時停止 〇
};
