#pragma once
#include <string>
#include <vector>
#include "../Object/Message.h"
#include "../Manager/InputManager.h"
#include "../Manager/QuestionManager.h"
#include "SceneBase.h"



enum class SceneState
{
	STORY,				// メッセージ表示
	QUESTION,		// 問題表示
	AFTER_TALK, // 解答後の会話
	RESULT,			// 結果表示
	END,					// シーン終了
	PAUSE,				// 一時停止
};

// 選択肢と次の質問番号のペア
struct Choice {
	std::string text;
	int nextIndex; // 次の質問番号（-1なら終了）
	int x, y;
	int count = 0;
	int imageHandle = -1;
};

// 選択肢の描画矩形を保存するための構造体
struct ChoiceRect {
	int left, top, right, bottom;
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

// 選択肢の結果
struct ChoiceResult {
	int questionIndex = -1;
	int selectedChoiceIndex = -1;
	std::string questionText;
	std::string selectedChoiceText;
	bool afterTalkDone = false;
};

class GameScene : public SceneBase
{
	// リザルト
	enum class ResultState
	{
		TAIL,			// 変更: 結果後のメッセージ表示（会話）を追加
		LIST,			// 一覧表示
		DETAIL,		// 詳細表示
	};

public:
#pragma region 定数宣言
	// ========================
	// 吹き出し
	// ========================
	// 白枠吹き出し
	static const int WHITE_LEFT = 15;			// 左端
	static const int WHITE_TOP = 800;			// 上端
	static const int WHITE_RIGHT = 1900;	// 右端
	static const int WHITE_BOTTOM = 1065;	// 下端
	// 黒枠吹き出し
	static const int BLACK_LEFT = 20;			// 左端 
	static const int BLACK_TOP = 805;			// 上端
	static const int BLACK_RIGHT = 1895;		// 右端
	static const int BLACK_BOTTOM = 1060;	// 下端
	// ==========================

	// =======================
	// 選択肢
	// =======================
	// 左側の選択肢開始位置(白枠)
	static const int CHOICE_WHITE_LEFT = 225;		// 左端
	static const int CHOICE_WHITE_TOP = 90;			// 上端
	static const int CHOICE_WHITE_RIGHT = 840;		// 右端
	static const int CHOICE_WHITE_BOTTOM = 720;	// 下端
	// 左側の選択肢開始位置(黒枠)
	static const int CHOICE_BLACK_LEFT = 230;		// 左端
	static const int CHOICE_BLACK_TOP = 95;			// 上端
	static const int CHOICE_BLACK_RIGHT = 835;		// 右端
	static const int CHOICE_BLACK_BOTTOM = 715;	// 下端
	// 右側の選択肢開始位置(白枠)
	static const int CHOICE2_WHITE_LEFT = 1180;	// 左端
	static const int CHOICE2_WHITE_TOP = 490;		// 上端
	static const int CHOICE2_WHITE_RIGHT = 1595;	// 右端
	static const int CHOICE2_WHITE_BOTTOM = 880;// 下端
	// 右側の選択肢開始位置(黒枠)
	static const int CHOICE2_BLACK_LEFT = 1185;	// 左端
	static const int CHOICE2_BLACK_TOP = 495;		// 上端
	static const int CHOICE2_BLACK_RIGHT = 1590;	// 右端
	static const int CHOICE2_BLACK_BOTTOM = 875;// 下端
	// =======================

	// =======================
	// 結果表示(アフタートーク)
	// =======================
	// 白枠
	static const int RESULT_WHITE_LEFT = 55;			// 左端
	static const int RESULT_WHITE_TOP = 380;			// 上端
	static const int RESULT_WHITE_RIGHT = 1855;	// 右端
	static const int RESULT_WHITE_BOTTOM = 1050;// 下端
	// 黒枠
	static const int RESULT_BLACK_LEFT = 60;			// 左端
	static const int RESULT_BLACK_TOP = 385;			// 上端
	static const int RESULT_BLACK_RIGHT = 1850;	// 右端
	static const int RESULT_BLACK_BOTTOM = 1045;	// 下端
	// =======================

	// =======================
	// 結果表示(一覧・詳細)
	// =======================
	// 白枠
	static const int LIST_WHITE_LEFT = 55;				// 左端
	static const int LIST_WHITE_TOP = 320;				// 上端
	static const int LIST_WHITE_RIGHT = 1855;		// 右端
	static const int LIST_WHITE_BOTTOM = 1050;		// 下端
	// 黒枠
	static const int LIST_BLACK_LEFT = 60;				// 左端
	static const int LIST_BLACK_TOP = 325;				// 上端
	static const int LIST_BLACK_RIGHT = 1850;		// 右端
	static const int LIST_BLACK_BOTTOM = 1045;		// 下端
	// =======================
	// 
	// =======================

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
	// SceneStateごとの更新処理
	void StoryUpdate(void);
	void QuestionUpdate(void);
	void AfterTalkUpdate(void);
	void ResultUpdate(void);
	void EndUpdate(void);
	void PauseUpdate(void);

	// ResultStateごとの更新処理
	void TailUpdate(void);
	void ListUpdate(void);
	void DetailUpdate(void);

	// 描画処理
	void Draw(void) override;

	// SceneStateごとの描画処理
	void QuestionDraw(void);
	void AfterTalkDraw(void);
	void ResultDraw(void);
	void EndDraw(void);
	void PauseDraw(void);

	// ResultStateごとの描画処理
	void TailDraw(void);
	void ListDraw(void);
	void DetailDraw(void);

	// 選択肢の描画
	void DrawChoices(const std::vector<Choice>& choices, int cursorIndex, bool showPercent);
	// 割合を横棒グラフで描画
	void DrawPercentageBar(int x, int y, int width, int height, float percent, int barColor);
	// 解放処理
	void Release(void) override;
	// 次の問いをセット
	void NextQuestion(int nextIndex_);

	// インスタンスの取得
	static GameScene& GetInstance(void);
	// シーンの状態を設定
	void SetSceneState(SceneState state) { state_ = state; }


private:
#pragma region 変数宣言
	// シーンの状態
	SceneState state_;
	// ポーズ前の状態を記録
	SceneState stateBeforePause_;
	// リザルトの状態
	ResultState resultState_;
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
	// リザルトリスト
	std::vector<ChoiceResult> results_;
	// リザルト後のメッセージリスト
	std::vector<std::string> resultTailMessages_;
	// 選択肢の描画矩形リスト
	std::vector<ChoiceRect> choiceRects_;

	// 解答後の会話メッセージ
	std::string talkMessage_;

	// 背景画像を複数管理
	std::vector<int> resultBgImages_;
	int currentBgIndex_; // 現在の背景
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
	// 
	int detailIndex_ = -1;

	// 一行ずつ表示するためのインデックス
	int currentLineIndex_;
	// 結果一覧の選択肢インデックス
	int resultSelectIndex_;
	// 現在表示しているリザルトメッセージ行のインデックス
	int resultTailIndex_;

	// 前の問い
	int prevQuestionIndex_;
	// 前の選択肢
	int prevSelectedChoice_;

	// 結果の表示のタイマー
	int resultTimer_;

	// 左右キーの押下状態
	bool leftPressed_;
	bool rightPressed_;
	// 上下キーの押下状態
	bool pauseDownPressed_;
	// ポーズ状態での操作
	bool pauseUpPressed_;
	// マウス左クリックの前フレーム状態
	bool prevMouseLeft_;
	// マウス左ボタンの押下状態
	bool isLButtonDown_;

	// 一時中断中の選択肢
	int pauseSelected_;

	// アフタートーク中かどうか	
	bool isAfterTalkActive_;

	// リザルトの構造体
	bool resultDisplayed_;
	int resultType_;

	// ポーズ用バックアップ
	int pauseStoryIndex_;
	int pauseQuestionIndex_;
	int pauseSelectedChoice_;
	std::string pauseMessage_;

	// 一覧表示に入った直後かどうか
	bool justEnteredList_;
	// 答えた質問の数
	int answeredCount_;
	bool skipListInput_;

	// フォントハンドル
	int fontHandle_;
};