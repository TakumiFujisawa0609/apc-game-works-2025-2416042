#include "ResultScene.h"

ResultScene::ResultScene(void)
	: inputManager_(InputManager::GetInstance()),
	resultState_(ResultState::TAIL),
	resultSelectIndex_(0),
	afterTalkIndex_(-1),
	isLButtonDown_(false),
	resultDisplayed_(false),
	resultType_(0),
	justEnteredList_(true),
	currentBgIndex_(0),
	resultTailIndex_(0)
{
}

ResultScene::~ResultScene(void)
{
}

void ResultScene::Init(void)
{
	// 会話メッセージ
	resultTailMessages_ = {
		"これで全ての質問が終わったよ。",
		"どうだった？正直に答えることはできたかい？",
		"君の選択は全て記録され、ここから確認できるよ。",
		"それじゃあ、君の解答結果を見てみよう。",
	};

	// 背景画像群の読み込み（存在するものだけ）
	resultBgImages_.clear();
	const int maxBgCount = 10;
	for (int i = 0; i < maxBgCount; ++i) {
		std::string path = "Data/Image/Result/Title1" + std::to_string(i) + ".png";
		int handle = LoadGraph(path.c_str());
		if (handle != -1) resultBgImages_.push_back(handle);
	}
	currentBgIndex_ = 0;

	resultDisplayed_ = false;
	resultState_ = ResultState::TAIL;
	resultSelectIndex_ = 0;
	results_.clear();
	resultTailIndex_ = 0;
	choiceRects_.clear();
	justEnteredList_ = true;

	// 初期メッセージをセット（TAIL の最初の行）
	resultState_ = ResultState::TAIL;
	resultTailIndex_ = 0;
	if (!resultTailMessages_.empty()) msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
}

void ResultScene::AddResult(const ChoiceResult& result)
{
	results_.push_back(result);
}

void ResultScene::Update(void)
{
	// マウス表示
	SetMouseDispFlag(TRUE);

	// マウス座標・ボタン
	int mouseX = 0, mouseY = 0;
	GetMousePoint(&mouseX, &mouseY);
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = ((mouseButton & MOUSE_INPUT_LEFT) != 0) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT) != 0;

	// 初回表示時の処理（背景タイプ決定など）
	if (!resultDisplayed_) {
		resultDisplayed_ = true;
		resultType_ = DetermineResultType();
		if (!resultBgImages_.empty()) {
			currentBgIndex_ = resultType_ % resultBgImages_.size();
		}
	}

	switch (resultState_) {
	case ResultState::TAIL:
		// メッセージが流れている間は入力を無視
		if (!msg_.IsFinished()) break;

		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg) {
			++resultTailIndex_;
			if (resultTailIndex_ < static_cast<int>(resultTailMessages_.size())) {
				msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
			}
			else {
				// TAIL 終了。LIST へ移行（ここで初期化）
				resultState_ = ResultState::LIST;
				resultSelectIndex_ = 0;
				justEnteredList_ = true; // DRAW側で矩形作成後に入力を許可するための旗
				msg_.SetMessage("結果はこちら");
			}
		}
		break;

	case ResultState::LIST:
	{
		// LIST に入った直後は 1 フレームだけ入力を無視して
		// Draw 側で choiceRects_ を更新できる余地を与える
		if (justEnteredList_) {
			// ただし choiceRects_ が空の場合は Update 側で矩形を作る（Draw が呼ばれないパスに備える）
			if (choiceRects_.empty()) {
				// results_.size() に合わせて矩形を作る（Draw と同じロジック） 
				choiceRects_.clear();
				for (size_t i = 0; i < results_.size(); ++i) {
					int y = LIST_BASE_Y + static_cast<int>(i) * LIST_ITEM_HEIGHT;
					int rect_left = LIST_RECT_LEFT;
					int rect_top = y;
					int rect_right = LIST_RECT_RIGHT;
					int rect_bottom = y + LIST_ITEM_HEIGHT;
					choiceRects_.push_back({ rect_left, rect_top, rect_right, rect_bottom });
				}
				// 次へ進む項目矩形
				{
					int nextLeft = 730 - 100; // Draw と同じ算出
					int nextTop = NEXT_ITEM_Y;
					int nextRight = 730 + 300;
					int nextBottom = NEXT_ITEM_Y + LIST_ITEM_HEIGHT;
					choiceRects_.push_back({ nextLeft, nextTop, nextRight, nextBottom });
				}
			}
			// このフレームは入力を消化しない（安全化）
			justEnteredList_ = false;
			break;
		}

		// キー操作（W/S）
		int listSize = static_cast<int>(results_.size());
		int totalOptions = listSize + 1;

		if (inputManager_.IsTrgDown(KEY_INPUT_W)) {
			resultSelectIndex_ = (resultSelectIndex_ - 1 + totalOptions) % totalOptions;
		}
		if (inputManager_.IsTrgDown(KEY_INPUT_S)) {
			resultSelectIndex_ = (resultSelectIndex_ + 1) % totalOptions;
		}

		// マウスオーバー判定（choiceRects_ が未構築ならここで作る）
		if (choiceRects_.empty()) {
			choiceRects_.clear();
			for (size_t i = 0; i < results_.size(); ++i) {
				int y = LIST_BASE_Y + static_cast<int>(i) * LIST_ITEM_HEIGHT;
				choiceRects_.push_back({ LIST_RECT_LEFT, y, LIST_RECT_RIGHT, y + LIST_ITEM_HEIGHT });
			}
			choiceRects_.push_back({ 730 - 100, NEXT_ITEM_Y, 730 + 300, NEXT_ITEM_Y + LIST_ITEM_HEIGHT });
		}

		bool mouseOverAny = false;
		for (size_t i = 0; i < choiceRects_.size(); ++i) {
			const auto& r = choiceRects_[i];
			if (mouseX >= r.left && mouseX <= r.right && mouseY >= r.top && mouseY <= r.bottom) {
				// 範囲内。i が最後の要素なら「次へ進む」
				if ((int)i != resultSelectIndex_) resultSelectIndex_ = (int)i;
				mouseOverAny = true;
				break;
			}
		}

		// クリックで決定（キーボード or マウス）
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg) {
			// 保護：results_ が空の場合は「次へ進む」（index == listSize）扱いのみ許可
			if (listSize == 0) {
				// 唯一の選択は "次へ進む"
				if (resultSelectIndex_ >= 0 && resultSelectIndex_ == 0) {
					// ここは "次へ進む" として扱う (choiceRects_ の構成により index 0 が next になっている可能性は考慮済)
					// だが安全のため、listSize == 0 なら常に終了扱い
					GameScene::GetInstance().SetSceneState(SceneState::END);
					msg_.SetMessage("これで終了だよ。遊んでくれてありがとう！");
				}
			}
			else {
				// resultSelectIndex_ が一覧内か next か判定
				if (resultSelectIndex_ < listSize) {
					// 安全確認
					if (resultSelectIndex_ < 0 || resultSelectIndex_ >= listSize) {
						// 不正なら無視
						break;
					}
					// 詳細表示へ
					resultState_ = ResultState::DETAIL;
					const auto& rr = results_[resultSelectIndex_];
					std::string detailMsg =
						"【質問 " + std::to_string(resultSelectIndex_ + 1) + " 】\n\n" +
						rr.questionText + "\n\n" +
						"あなたの選択: " + rr.selectedChoiceText +
						"\n\nSpaceキーまたはクリックで一覧に戻る。";
					msg_.SetMessage(detailMsg);

					// アフタートーク未再生ならマーク
					if (!results_[resultSelectIndex_].afterTalkDone) {
						afterTalkIndex_ = resultSelectIndex_;
						results_[resultSelectIndex_].afterTalkDone = true;
					}
				}
				else if (resultSelectIndex_ == listSize) {
					// 「次へ進む」が選ばれた
					bool allDone = true;
					for (auto& r : results_) {
						if (!r.afterTalkDone) { allDone = false; break; }
					}
					if (allDone) {
						GameScene::GetInstance().SetSceneState(SceneState::END);
						msg_.SetMessage("これで終了だよ。遊んでくれてありがとう！");
					}
					else {
						msg_.SetMessage("まだ全部のアフタートークが終わっていないみたい。");
					}
				}
			}
		}

		break;
	}

	case ResultState::DETAIL:
		// メッセージ表示中は待つ
		if (!msg_.IsFinished()) break;

		// Space or click で戻る
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg) {
			resultState_ = ResultState::LIST;
			msg_.SetMessage("");
			// 戻った直後に LIST の矩形を再生成させる
			justEnteredList_ = true;
			choiceRects_.clear();
			resultSelectIndex_ = 0;
		}
		break;
	}
}

void ResultScene::Draw(void)
{
	// 背景
	if (!resultBgImages_.empty()) {
		DrawGraph(0, 0, resultBgImages_[currentBgIndex_], FALSE);
	}

	switch (resultState_) {
	case ResultState::TAIL:
		// TAIL 状態では msg_ を表示するだけ（msg_ は Init/Update でセットされる）
		msg_.Draw(165, 65);
		break;

	case ResultState::LIST:
	{
		// 枠
		DrawBox(160, 320, 1735, 1050, GetColor(255, 255, 255), TRUE);
		DrawBox(165, 325, 1730, 1045, GetColor(0, 0, 0), TRUE);

		SetFontSize(40);
		DrawFormatString(175, 65, GetColor(255, 255, 255), "結果はこちら。");
		DrawFormatString(175, 105, GetColor(255, 255, 255), "W/Sキーで選択し、Spaceキーで詳細を見れます。");

		// ここで矩形リストを改めて作成（Update 側でも作るが Draw 側で正確に作る）
		choiceRects_.clear();
		int baseY = LIST_BASE_Y;
		int listSize = static_cast<int>(results_.size());

		SetFontSize(100);
		for (int i = 0; i < listSize; ++i) {
			int y = baseY + i * LIST_ITEM_HEIGHT;
			int color = (i == resultSelectIndex_) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

			// 選択時の背景
			int rect_left = LIST_RECT_LEFT;
			int rect_top = y;
			int rect_right = LIST_RECT_RIGHT;
			int rect_bottom = y + LIST_ITEM_HEIGHT;

			if (i == resultSelectIndex_) {
				DrawBox(rect_left, rect_top, rect_right, rect_bottom, GetColor(50, 50, 50), TRUE);
			}

			std::string line = "問" + std::to_string(i + 1);
			DrawString(270, y, line.c_str(), color);
			if (i == resultSelectIndex_) DrawString(200, y, ">", color);

			choiceRects_.push_back({ rect_left, rect_top, rect_right, rect_bottom });
		}

		// 次へ進む
		SetFontSize(100);
		std::string nextText = "次へ進む";
		int nextX = 730;
		int nextY = NEXT_ITEM_Y;
		int nextWidth = GetDrawStringWidth(nextText.c_str(), (int)nextText.size());
		int nextLeft = nextX - 100;
		int nextRight = nextX + nextWidth + 40;
		int nextBottom = nextY + LIST_ITEM_HEIGHT;

		if (resultSelectIndex_ == listSize) {
			DrawBox(nextLeft, nextY, nextRight, nextBottom, GetColor(50, 50, 50), TRUE);
		}
		if (resultSelectIndex_ == listSize) DrawString(680, nextY, ">", GetColor(255, 255, 0));
		DrawString(nextX, nextY, nextText.c_str(), (resultSelectIndex_ == listSize) ? GetColor(255, 255, 0) : GetColor(255, 255, 255));

		choiceRects_.push_back({ nextLeft, nextY, nextRight, nextBottom });

		break;
	}

	case ResultState::DETAIL:
	{
		DrawBox(160, 320, 1735, 1050, GetColor(255, 255, 255), TRUE);
		DrawBox(165, 325, 1730, 1045, GetColor(0, 0, 0), TRUE);

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
	// 必要ならリソース解放（現状特に無し）
	for (int h : resultBgImages_) {
		if (h != -1) DeleteGraph(h);
	}
	resultBgImages_.clear();
}

void ResultScene::Transition(void)
{
	// GameScene を RESULT 状態にする（既存呼び出しと合わせる）
	GameScene::GetInstance().SetSceneState(SceneState::RESULT);

	resultDisplayed_ = false;
	resultState_ = ResultState::TAIL;
	resultTailIndex_ = 0;
	choiceRects_.clear();
	resultSelectIndex_ = 0;
	justEnteredList_ = true;
	// msg_ に最初の TAIL メッセージをセット
	if (!resultTailMessages_.empty()) msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
}

ResultScene& ResultScene::GetInstance(void)
{
	static ResultScene instance;
	return instance;
}

int ResultScene::DetermineResultType(void)
{
	int positive = 0, calm = 0, other = 0;
	for (auto& r : results_) {
		if (r.selectedChoiceText.find("パン") != std::string::npos) ++positive;
		else if (r.selectedChoiceText.find("ご飯") != std::string::npos) ++calm;
		else ++other;
	}
	if (positive > calm && positive > other) return 0;
	if (calm > positive && calm > other) return 1;
	return 2;
}
