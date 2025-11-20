#include "ResultScene.h"

ResultScene::ResultScene(void)
	:inputManager_(InputManager::GetInstance()),
	resultState_(ResultState::TAIL), // 変更: 初期状態をTAILに設定
	resultSelectIndex_(0),
	afterTalkIndex_(-1),
	isLButtonDown_(false),
	resultDisplayed_(false),
	resultType_(0),
	justEnteredList_(true)
{
}

ResultScene::~ResultScene(void)
{
}

void ResultScene::Init(void)
{
	resultTailMessages_ = {
	"これで全ての質問が終わったよ。",
	"どうだった？正直に答えることはできたかい？",
	"君の選択は全て記録され、ここから確認できるよ。",
	"それじゃあ、君の解答結果を見てみよう。",
	};

	// --- リザルト背景群の読み込み ---
	resultBgImages_.clear();
	const int maxBgCount = 10; // 将来的に10枚くらい用意してもOK
	for (int i = 0; i < maxBgCount; ++i)
	{
		std::string path = "Data/Image/Result/Title1" + std::to_string(i) + ".png";
		int handle = LoadGraph(path.c_str());
		if (handle != -1)
		{
			resultBgImages_.push_back(handle);
		}
	}
	currentBgIndex_ = 0;

	resultDisplayed_ = false;

	// リザルト関連の初期化
	resultState_ = ResultState::LIST;
	resultSelectIndex_ = 0;
	results_.clear();
	resultTailIndex_ = 0;
}

void ResultScene::Update(void)
{
	// マウスカーソルを表示
	SetMouseDispFlag(TRUE);

	// マウス座標とボタン状態を取得
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

	if (!resultDisplayed_)
	{
		resultDisplayed_ = true;
		resultType_ = DetermineResultType();

		// 背景を切り替え（存在する数以内で）
		if (!resultBgImages_.empty()) {
			currentBgIndex_ = resultType_ % resultBgImages_.size();
		}
	}

	switch (resultState_)
	{
	case ResultState::TAIL: // 変更: 新しいTAIL状態の処理
		if (!msg_.IsFinished()) break;

		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
		{
			resultTailIndex_++;
			if (resultTailIndex_ < static_cast<int>(resultTailMessages_.size()))
			{
				// 次の行へ
				msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
			}
			else
			{
				// 会話終了 -> LISTへ
				resultState_ = ResultState::LIST;
				justEnteredList_ = true; // LISTに入ったことをマーク
				msg_.SetMessage("結果はこちら");
			}
		}
		break;

	case ResultState::LIST:
	{
		// 変更: TAILで会話を終えたため、ここではメッセージを表示しない
		if (justEnteredList_) {
			justEnteredList_ = false;  // 一度だけ実行
		}

		int listSize = static_cast<int>(results_.size());
		int totalOptions = listSize + 1;

		// W/Sキーで選択肢移動
		if (inputManager_.IsTrgDown(KEY_INPUT_W))
			resultSelectIndex_ = (resultSelectIndex_ - 1 + totalOptions) % totalOptions;
		if (inputManager_.IsTrgDown(KEY_INPUT_S))
			resultSelectIndex_ = (resultSelectIndex_ + 1) % totalOptions;

		// マウス操作による選択肢変更・決定
		{
			int mouseX, mouseY;
			GetMousePoint(&mouseX, &mouseY);
			bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_; // Update冒頭で取得した変数を使用

			int newSelected = resultSelectIndex_;
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
				// ★修正点: 選択インデックスが変更されたときのみ代入する
				if (resultSelectIndex_ != newSelected) {
					resultSelectIndex_ = newSelected;
				}
			}

			// マウスクリック決定処理
			if (isLButtonTrg && isMouseOverChoice) {
				goto RESULT_DECISION;
			}
		}

		// 選択肢決定 (キーボード操作)
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
		{
			goto RESULT_DECISION;
		}

		// ★追加: 決定ロジックの開始点としてラベルを定義
	RESULT_DECISION:

		// Spaceキーで決定
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
		{
			if (resultSelectIndex_ < listSize)
			{
				// 詳細表示へ
				if (resultSelectIndex_ < 0 || resultSelectIndex_ >= listSize) {
					// 不正なインデックスの場合は何もしないか、エラーログを出す
					break;
				}
				resultState_ = ResultState::DETAIL;

				std::string detailMsg =
					"【質問 " + std::to_string(resultSelectIndex_ + 1) + " 】\n\n" +
					results_[resultSelectIndex_].questionText + "\n\n" +
					"あなたの選択: " + results_[resultSelectIndex_].selectedChoiceText +
					"\n\nSpaceキーまたはクリックで一覧に戻る。";

				msg_.SetMessage(detailMsg);

				// アフタートーク未再生なら再生
				if (!results_[resultSelectIndex_].afterTalkDone)
				{
					afterTalkIndex_ = resultSelectIndex_; // ← 再生対象を記録
					results_[resultSelectIndex_].afterTalkDone = true;
				}
			}
			else if (resultSelectIndex_ == listSize)
			{
				// まだアフタートークが終わっていないものがあるかチェック
				bool allDone = true;
				for (auto& r : results_)
				{
					if (!r.afterTalkDone)
					{
						allDone = false;
						break;
					}
				}

				if (allDone)
				{
					GameScene::GetInstance().SetSceneState(SceneState::END);
					msg_.SetMessage("これで終了だよ。遊んでくれてありがとう！");
				}
			}
		}
		break;
	}

	case ResultState::DETAIL:
		// メッセージが流れている間は入力を受け付けない
		if (!msg_.IsFinished()) break;

		// Spaceで一覧へ戻る
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
		{
			resultState_ = ResultState::LIST;
			msg_.SetMessage("");
		}
		break;
	}

}

void ResultScene::Draw(void)
{
	switch (resultState_)
	{
	case ResultState::TAIL:
		break;

	case ResultState::LIST:
	{
		// LIST/DETAIL状態でのみ、結果一覧/詳細の大きな枠を描画
		DrawBox(160, 320, 1735, 1050, GetColor(255, 255, 255), true);
		DrawBox(165, 325, 1730, 1045, GetColor(0, 0, 0), true);

		SetFontSize(40);
		DrawFormatString(175, 65, GetColor(255, 255, 255),
			"結果はこちら。");
		DrawFormatString(175, 105, GetColor(255, 255, 255),
			"W/Sキーで選択し、Spaceキーで詳細を見れます。");

		/*SetFontSize(36);
		DrawString(816, 345, "【全問解答結果】", GetColor(255, 255, 0));
		DrawLine(160, 390, 1735, 390, GetColor(255, 255, 255));*/

		// ★追加: マウス用矩形リストをクリア
		choiceRects_.clear();

		// 共通の描画設定
		int text_size_a = 100; // SetFontSize(60) の高さ
		int text_size_b = 100; // SetFontSize(70) の高さ

		SetFontSize(32);
		int baseY = 410;

		// 回答リストの表示
		for (size_t i = 0; i < results_.size(); i++)
		{
			int y = baseY + (int)i * 100;
			SetFontSize(100);
			int color = (i == resultSelectIndex_) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

			std::string line = "問" + std::to_string(i + 1);
			// 描画
			DrawString(270, y, line.c_str(), color);
			if (i == resultSelectIndex_) DrawString(200, y, ">", color);

			// マウス当たり判定用の矩形を保存
			// [問X]の左端(210)と > の左端(180)を考慮し、幅はリスト枠(1730)いっぱいまで利用

			int rect_left = 180;
			int rect_top = y;
			int rect_right = 450; // 大きな枠の右端を使用
			int rect_bottom = y + text_size_a;

			// 選択時の背景描画 (カーソルがマウスで動くよう、DrawBoxも追加します)
			if (i == resultSelectIndex_)
			{
				DrawBox(rect_left, rect_top, rect_right, rect_bottom, GetColor(50, 50, 50), TRUE);
			}
			// DrawStringの再描画（背景の上に文字が来るように）
			DrawString(270, y, line.c_str(), color);
			if (i == resultSelectIndex_) DrawString(200, y, ">", color);

			choiceRects_.push_back({ rect_left, rect_top, rect_right, rect_bottom });
		}

		// 次へ進む選択肢
		int nextY = 900;
		SetFontSize(100);
		std::string nextText = "次へ進む";
		int nextX = 730;
		int nextWidth = GetDrawStringWidth(nextText.c_str(), (int)nextText.size());
		int nextColor = (resultSelectIndex_ == (int)results_.size()) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
		// 選択時の背景描画
		if (resultSelectIndex_ == (int)results_.size()) DrawString(680, nextY, ">", nextColor);

		// マウス当たり判定用の矩形を保存
		int rect_left = nextX - 100; // > の位置(830)を考慮
		int rect_top = nextY;
		int rect_right = nextX + nextWidth + 40;
		int rect_bottom = nextY + text_size_b;

		// 選択時の背景描画
		if (resultSelectIndex_ == (int)results_.size())
		{
			DrawBox(rect_left, rect_top, rect_right, rect_bottom, GetColor(50, 50, 50), TRUE);
		}

		// 描画
		DrawString(nextX, nextY, nextText.c_str(), nextColor);
		if (resultSelectIndex_ == (int)results_.size()) DrawString(680, nextY, ">", nextColor);

		choiceRects_.push_back({ rect_left, rect_top, rect_right, rect_bottom });
		break;
	}

	case ResultState::DETAIL:
	{
		// LIST/DETAIL状態でのみ、結果一覧/詳細の大きな枠を描画
		DrawBox(160, 320, 1735, 1050, GetColor(255, 255, 255), true);
		DrawBox(165, 325, 1730, 1045, GetColor(0, 0, 0), true);

		SetFontSize(36);
		DrawString(816, 345, "【全問解答結果】", GetColor(255, 255, 0));
		DrawLine(160, 390, 1735, 390, GetColor(255, 255, 255));

		msg_.Draw(170, 430);
		break;
	}
	}

}

void ResultScene::Release(void)
{
}

void ResultScene::Transition(void)
{
	GameScene::GetInstance().SetSceneState(SceneState::RESULT);
	resultDisplayed_ = false;
	resultState_ = ResultState::TAIL;
	resultTailIndex_ = 0;
	msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
}

ResultScene& ResultScene::GetInstance(void)
{
	// C++におけるシングルトンの典型的な実装
	static ResultScene instance;
	return instance;
}

void ResultScene::AddResult(const ChoiceResult& result)
{
	results_.push_back(result);
}

int ResultScene::DetermineResultType(void)
{
	// 結果タイプを柔軟に判定（サンプルロジック）
	int positive = 0;
	int calm = 0;
	int other = 0;

	for (auto& r : results_) {
		if (r.selectedChoiceText.find("パン") != std::string::npos) positive++;
		else if (r.selectedChoiceText.find("ご飯") != std::string::npos) calm++;
		else other++;
	}

	if (positive > calm && positive > other) return 0;
	if (calm > positive && calm > other) return 1;
	return 2;
}