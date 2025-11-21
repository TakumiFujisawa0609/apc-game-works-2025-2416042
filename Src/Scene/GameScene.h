#pragma once
#include <string>
#include <vector>
#include "SceneBase.h"
#include "../Object/Message.h"
#include "../Manager/InputManager.h"
#include "../Manager/QuestionManager.h"
#include "../Common/ChoiceRect.h"


// 選択肢と次の質問番号のペア
struct Choice {
	std::string text;
	int nextIndex; // 次の質問番号（-1なら終了）
	int x, y;
	int count = 0;
};

// 質問と選択肢のセット
struct Question {
	std::string text;
	std::vector<Choice> choices;
};

// 解答後の会話
struct AfterTalk {
	std::vector<std::string> lines;
	int questionIndex;
	int choiceIndex;
};


class GameScene : public SceneBase
{
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

	// インスタンスの取得
	static GameScene& GetInstance(void);

	// シーンの状態を設定
	void SetSceneState(SceneState state) { state_ = state; }
	// メッセージを設定 
	void SetMessage(const std::string& msg);
private:
#pragma region 変数宣言
	// シーンの状態
	SceneState state_;
	// ポーズ前の状態を記録
	SceneState stateBeforePause_;

	// メッセージオブジェクト
	Message msg_;
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

	
	// 選択肢の描画矩形リスト
	std::vector<ChoiceRect> choiceRects_;

	// 解答後の会話メッセージ
	std::string talkMessage_;


	// ゲーム背景
	int gImage_;

	// 通常BGM
	int bgmHandle_;

	// ストーリーの保管庫
	int storyIndex_;
	// 問題の保管庫
	int questionIndex_;
	// 選択した選択肢の保管庫
	int selectedChoice_;
	// 解答後の会話の保管庫
	int afterTalkIndex_;

	// 一行ずつ表示するためのインデックス
	int currentLineIndex_;

	

	// 結果の表示のタイマー
	int resultTimer_;

	// 前の問い
	int prevQuestionIndex_;
	// 前の選択肢
	int prevSelectedChoice_;

	// マウス左クリックの前フレーム状態
	bool prevMouseLeft_;

	// 左右キーの押下状態
	bool leftPressed_;
	bool rightPressed_;
	// 上下キーの押下状態
	bool pauseDownPressed_;
	// ポーズ状態での操作
	bool pauseUpPressed_;
	// マウス左ボタンの押下状態
	bool isLButtonDown_;

	// 一時中断中の選択肢
	int pauseSelected_;

	// アフタートーク中かどうか	
	bool isAfterTalkActive_;
};