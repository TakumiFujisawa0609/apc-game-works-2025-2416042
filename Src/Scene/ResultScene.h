#pragma once
#include <DxLib.h>
#include <string>
#include <vector>
#include "SceneBase.h"
#include "GameScene.h"
#include "../Object/Message.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/QuestionManager.h"

// 選択肢の矩形（GameScene 側と同様に定義）
struct ChoiceRect {
	int left, top, right, bottom;
};

enum class ResultState
{
	TAIL,   // 結果前の会話
	LIST,   // 一覧表示
	DETAIL, // 詳細表示
};

// 選択肢の結果
struct ChoiceResult {
	int questionIndex = -1;
	std::string questionText;
	std::string selectedChoiceText;
	bool afterTalkDone = false;
};

class ResultScene : public SceneBase
{
public:
	// コンストラクタ / デストラクタ
	ResultScene(void);
	~ResultScene(void);

	// ライフサイクル
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	// 他シーンからの移行トリガ
	void Transition(void);

	// シングルトン取得
	static ResultScene& GetInstance(void);

	// 外部から状態をセットする（既存コードで使われているため残す）
	void SetResultState(ResultState resultState) { resultState_ = resultState; }

	// 結果を外部から追加（GameScene などが使う）
	void AddResult(const ChoiceResult& result);

private:
	// 内部ユーティリティ
	int DetermineResultType(void);

	// インスタンス
	Message msg_;
	InputManager& inputManager_;
	// 状態管理
	ResultState resultState_;
	// 結果データ
	std::vector<ChoiceResult> results_;
	// 選択肢矩形群（マウス操作用）
	std::vector<ChoiceRect> choiceRects_;
	// リザルト前の会話メッセージ群
	std::vector<std::string> resultTailMessages_;
	// リザルト背景群
	std::vector<int> resultBgImages_;
	// 現在の背景インデックス
	int currentBgIndex_;
	// 選択中インデックス
	int resultSelectIndex_;
	// アフタートーク再生対象インデックス
	int afterTalkIndex_;
	// リザルト前会話の進行インデックス
	int resultTailIndex_;
	// マウス左ボタンダウン状態
	bool isLButtonDown_;
	// 結果表示済みフラグ
	bool resultDisplayed_;
	// 結果タイプ
	int resultType_;
	// 一覧に入った直後フラグ
	bool justEnteredList_;

	// 描画レイアウト定数（Draw と Update で一致させる）
	static constexpr int LIST_BASE_Y = 410;
	static constexpr int LIST_ITEM_HEIGHT = 100;
	static constexpr int LIST_RECT_LEFT = 180;
	static constexpr int LIST_RECT_RIGHT = 1730; // 枠いっぱいまで
	static constexpr int NEXT_ITEM_Y = 900;
};
