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

enum class ResultState
{
	TAIL,			// 変更: 結果後のメッセージ表示（会話）を追加
	LIST,			// 一覧表示
	DETAIL,		// 詳細表示
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
	// コンストラクタ
	ResultScene(void);
	// デストラクタ
	~ResultScene(void);
	// 初期化処理
	void Init(void) override;
	// 更新ステップ
	void Update(void) override;
	// 描画処理
	void Draw(void) override;
	// 解放処理
	void Release(void) override;

	// 移行処理
	void Transition(void);

	// インスタンスの取得
	static ResultScene& GetInstance(void);
	// シーンの状態を設定
	void SetResultState(ResultState resultState) { resultState_ = resultState; }
	// 結果をリストに追加
	void AddResult(const ChoiceResult& result);
private:
	// メッセージオブジェクト
	Message msg_;
	// 入力制御オブジェクト
	InputManager& inputManager_;

	// リザルトの状態
	ResultState resultState_;

	// リザルトリスト
	std::vector<ChoiceResult> results_;
	// 選択肢の描画矩形リスト
	std::vector<ChoiceRect> choiceRects_;

	// リザルト後のメッセージリスト
	std::vector<std::string> resultTailMessages_;
	// 背景画像を複数管理
	std::vector<int> resultBgImages_;
	int currentBgIndex_; // 現在の背景

	// 結果一覧の選択肢インデックス
	int resultSelectIndex_;
	// 解答後の会話の保管庫
	int afterTalkIndex_;
	// 現在表示しているリザルトメッセージの行インデックス
	int resultTailIndex_;

	// マウス左ボタンの押下状態
	bool isLButtonDown_;
	// リザルトの構造体
	bool resultDisplayed_;
	int resultType_;
	// 一覧表示に入った直後かどうか
	bool justEnteredList_;

	// 背景の切り替え
	int DetermineResultType(void);
};