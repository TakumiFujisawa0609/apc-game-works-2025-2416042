#pragma once
#include <string>
#include <vector>
#include "SceneBase.h"
#include "../Object/Message.h"
#include "../Manager/InputManager.h"
#include "../Manager/QuestionManager.h"

// 選択肢と次の質問番号のペア
struct Choice {
	std::string text;
	int nextIndex; // 次の質問番号（-1なら終了）
	int x, y;
	int count = 0;
};

struct Question {
	std::string text;
	std::vector<Choice> choices;
};

struct AfterTalk {
	std::string text;
	int questionIndex;
	int choiceIndex;
};


class GameScene : public SceneBase
{
	enum class SceneState
	{
		STORY,				// メッセージ表示
		QUESTION,		// 問題表示
		ANSWER_TALK, // 解答後の会話
		RESULT,			// 結果表示
		END,					// シーン終了
		PAUSE,				// 一時停止
	};


public:
	static const int START_Y = 740;			  // 吹き出し内の縦位置
	static const int SPACING = 660;			  // 選択肢同士の間隔
	static const int SCREEN_W = 1920;      // 画面幅

#pragma region 前方宣言
	// コンストラクタ
	GameScene(void);
	// デストラクタ
	~GameScene(void);

	// 初期化処理
	void Init(void) override;
	void ManagerInit(void);
	// 更新ステップ
	void Update(void) override;
	// 描画処理
	void Draw(void) override;
	// 選択肢の描画
	void DrawChoices(const std::vector<Choice>& choices, int cursorIndex, bool showPercent);
	// 解放処理
	void Release(void) override;

	// 次の問いをセット
	void NextQuestion(int nextIndex_);

private:
#pragma region 変数宣言
	// メッセージオブジェクト
	Message msg_;
	// シーンの状態
	SceneState state_;
	// ポーズ前の状態を記録
	SceneState stateBeforePause_;
	
	// 入力制御オブジェクト
	InputManager& inputManager_;
	// 質問管理オブジェクト
	QuestionManager questionManager_;

	// 文章リスト
	std::vector<std::string> story_; 
	// 問題リスト
	std::vector<Question> questions_;
	// 解答後の会話リスト
	std::vector<AfterTalk> afterTalks_;

	// 選択肢と結果のログ
	std::vector<std::pair<std::string, std::string>> resultLog_;

	// ゲーム背景
	int gImage_;

	// ストーリーの保管庫
	int storyIndex_;
	// 問題の保管庫
	int questionIndex_;
	// 選択した選択肢の保管庫
	int selectedChoice_;
	// 解答後の会話の保管庫
	int afterTalkIndex_;
	// ポーズメニューの選択肢インデックス
	int pauseSelectIndex_;

	// 結果の表示のタイマー
	int resultTimer_;

	// 左右キーの押下状態
	bool leftPressed_;
	bool rightPressed_;

	// 前の問い
	int prevQuestionIndex_;
	// 前の選択肢
	int prevSelectedChoice_;

	// ポーズ状態での操作
	bool pauseUpPressed_;
	// 一時中断中の選択肢
	int pauseSelected_;
	
};