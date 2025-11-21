#include "ResultScene.h"

// ------------------------------------------------
// プライベートユーティリティ関数の追加
// ------------------------------------------------

// LIST状態の選択肢と「次へ進む」ボタンの矩形を生成
void ResultScene::CreateChoiceRects(int listSize)
{
	choiceRects_.clear();
	// 質問一覧の矩形
	for (int i = 0; i < listSize; ++i) {
		int y = LIST_BASE_Y + i * LIST_ITEM_HEIGHT;
		int rect_left = LIST_RECT_LEFT;
		int rect_top = y;
		int rect_right = LIST_RECT_RIGHT;
		int rect_bottom = y + LIST_ITEM_HEIGHT;
		// 描画よりも少し外側まで当たり判定を広げる
		choiceRects_.push_back({ rect_left - 10, rect_top, rect_right + 10, rect_bottom });
	}

	// 次へ進む項目矩形 (Draw() の描画位置に合わせて広めに設定)
	int nextLeft = 600;
	int nextRight = 1300;
	int nextTop = NEXT_ITEM_Y;
	int nextBottom = NEXT_ITEM_Y + LIST_ITEM_HEIGHT;
	choiceRects_.push_back({ nextLeft, nextTop, nextRight, nextBottom });
}


// ------------------------------------------------
// 既存メソッドの修正
// ------------------------------------------------

ResultScene::ResultScene(void)
	: inputManager_(InputManager::GetInstance()),
	resultState_(ResultState::TAIL),
	resultSelectIndex_(0),
	afterTalkIndex_(-1),
	isLButtonDown_(false),
	resultDisplayed_(false),
	resultType_(0),
	// justEnteredList_(true), // 削除
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

	// 初期メッセージ設定
	if (!resultTailMessages_.empty()) {
		msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
	}
}


void ResultScene::Update(void)
{
	// マウス表示
	SetMouseDispFlag(TRUE);

	// メッセージ更新（TAIL/DETAIL状態でのみ必要）
	msg_.Update();

	// マウス座標・ボタン
	int mouseX = 0, mouseY = 0;
	GetMousePoint(&mouseX, &mouseY);
	int mouseButton = GetMouseInput();
	// isLButtonTrg: 押下された瞬間
	bool isLButtonTrg = ((mouseButton & MOUSE_INPUT_LEFT) != 0) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT) != 0;

	// 初回表示時の処理
	if (!resultDisplayed_) {
		resultDisplayed_ = true;
		resultType_ = DetermineResultType();
		if (!resultBgImages_.empty()) {
			currentBgIndex_ = resultType_ % resultBgImages_.size();
		}
	}

	switch (resultState_) {
	case ResultState::TAIL:
		// メッセージが最後まで流れるのを待つ
		// ここで msg_.IsFinished() を使うことで、メッセージの表示完了を正確にチェック
		if (!msg_.IsFinished()) break;

		// スペースキーまたはクリックで次のメッセージへ
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg) {
			++resultTailIndex_;
			if (resultTailIndex_ < static_cast<int>(resultTailMessages_.size())) {
				// 次の会話メッセージをセット
				msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
			}
			else {
				// TAIL 終了。LIST へ移行
				resultState_ = ResultState::LIST;
				resultSelectIndex_ = 0;
				// 矩形を生成
				CreateChoiceRects(static_cast<int>(results_.size()));
				msg_.SetMessage("結果はこちら。"); // LIST状態でのメッセージをセット
			}
		}
		break;

	case ResultState::LIST:
	{
		int listSize = static_cast<int>(results_.size());
		int totalOptions = listSize + 1; // 質問一覧 + 「次へ進む」

		// LISTに入った直後、まだ矩形がない場合のために再チェック
		if (choiceRects_.empty() && listSize > 0) { // 結果が0個の場合は矩形を作らない
			CreateChoiceRects(listSize);
		}

		// --- カーソル移動（W/Sキー） ---
		if (inputManager_.IsTrgDown(KEY_INPUT_W)) {
			resultSelectIndex_ = (resultSelectIndex_ - 1 + totalOptions) % totalOptions;
		}
		if (inputManager_.IsTrgDown(KEY_INPUT_S)) {
			resultSelectIndex_ = (resultSelectIndex_ + 1) % totalOptions;
		}

		// --- マウスオーバー判定 ---
		int newSelectIndex = resultSelectIndex_;
		bool mouseOverAny = false;
		for (size_t i = 0; i < choiceRects_.size(); ++i) {
			const auto& r = choiceRects_[i];
			if (mouseX >= r.left && mouseX <= r.right && mouseY >= r.top && mouseY <= r.bottom) {
				newSelectIndex = (int)i;
				mouseOverAny = true;
				break;
			}
		}

		if (mouseOverAny) {
			resultSelectIndex_ = newSelectIndex;
		}

		// --- クリック/スペースで決定 ---
		// マウスで決定する場合、マウスオーバーしていることを条件に追加
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || (isLButtonTrg && mouseOverAny)) {

			// 選択肢（質問）が選ばれた場合 (最後の「次へ進む」ではない)
			if (resultSelectIndex_ < listSize) {
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
				results_[resultSelectIndex_].afterTalkDone = true;
			}
			// 「次へ進む」が選ばれた場合
			else if (resultSelectIndex_ == listSize) {
				bool allDone = true;
				for (auto& r : results_) {
					if (!r.afterTalkDone) { allDone = false; break; }
				}
				// 全てのアフタートークが終わった、または結果データが空の場合
				if (allDone || results_.empty()) {
					GameScene::GetInstance().SetSceneState(SceneState::END);
					// GameSceneのメッセージオブジェクトにも終了メッセージをセット
					GameScene::GetInstance().SetMessage("これで終了だよ。遊んでくれてありがとう！");
				}
				else {
					msg_.SetMessage("まだ全部のアフタートークが終わっていないみたい。");
					// 選択インデックスを「次へ進む」以外にリセット
					resultSelectIndex_ = 0;
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
			msg_.SetMessage("結果はこちら。");
			// 選択をLISTの先頭にリセット
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
		// TAIL 状態では msg_ を表示するだけ (GameSceneの吹き出し位置に合わせて描画)
		msg_.Draw(165, 65);
		break;

	case ResultState::LIST:
	{
		// 枠
		DrawBox(160, 320, 1735, 1050, GetColor(255, 255, 255), TRUE);
		DrawBox(165, 325, 1730, 1045, GetColor(0, 0, 0), TRUE);

		// ヘッダーテキスト
		SetFontSize(40);
		DrawFormatString(175, 65, GetColor(255, 255, 255), "結果はこちら。");
		DrawFormatString(175, 105, GetColor(255, 255, 255), "W/Sキーまたはマウスで選択し、Space/クリックで詳細を見れます。");

		// LISTのメッセージ (TAILの会話終了後に表示されるメッセージ)
		SetFontSize(50);
		msg_.Draw(175, 160);

		int baseY = LIST_BASE_Y;
		int listSize = static_cast<int>(results_.size());

		SetFontSize(60);

		// 質問一覧の描画
		for (int i = 0; i < listSize; ++i) {
			int y = baseY + i * LIST_ITEM_HEIGHT;
			int color = (i == resultSelectIndex_) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
			int baseColor = (i == resultSelectIndex_) ? GetColor(50, 50, 50) : GetColor(0, 0, 0);

			// 選択時の背景
			if (i == resultSelectIndex_ && i < choiceRects_.size()) {
				const auto& rect = choiceRects_[i];
				// 矩形が生成されている前提で描画
				DrawBox(rect.left, rect.top, rect.right, rect.bottom, baseColor, TRUE);
			}

			// 既読マーク
			if (results_[i].afterTalkDone) {
				DrawString(200, y, "★", GetColor(0, 255, 0));
			}

			// 質問番号
			std::string line = "問" + std::to_string(i + 1) + ": ";
			DrawString(270, y, line.c_str(), color);

			// 質問テキスト（省略表示）
			std::string qText = results_[i].questionText;
			size_t newlinePos = qText.find('\n');
			if (newlinePos != std::string::npos) {
				qText = qText.substr(0, newlinePos);
			}
			if (qText.size() > 25) {
				qText = qText.substr(0, 25) + "...";
			}
			DrawString(450, y, qText.c_str(), color);
		}

		// 次へ進む
		SetFontSize(80);
		std::string nextText = "次へ進む";
		int nextX = 730;
		int nextY = NEXT_ITEM_Y;
		int listIndex = listSize; // 「次へ進む」のインデックス

		int nextColor = (listIndex == resultSelectIndex_) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
		int nextBaseColor = (listIndex == resultSelectIndex_) ? GetColor(50, 50, 50) : GetColor(0, 0, 0);

		// 選択時の背景
		if (listIndex == resultSelectIndex_ && listIndex < choiceRects_.size()) {
			const auto& rect = choiceRects_[listIndex];
			DrawBox(rect.left, rect.top, rect.right, rect.bottom, nextBaseColor, TRUE);
		}

		if (listIndex == resultSelectIndex_) DrawString(680, nextY, ">", nextColor);
		DrawString(nextX, nextY, nextText.c_str(), nextColor);

		break;
	}

	case ResultState::DETAIL:
	{
		DrawBox(160, 320, 1735, 1050, GetColor(255, 255, 255), TRUE);
		DrawBox(165, 325, 1730, 1045, GetColor(0, 0, 0), TRUE);

		SetFontSize(48);
		DrawString(750, 345, "【質問詳細】", GetColor(255, 255, 0));
		DrawLine(160, 390, 1735, 390, GetColor(255, 255, 255));

		// 詳細メッセージの描画位置調整
		SetFontSize(36); // DETAILの文字サイズを調整
		msg_.Draw(170, 410);
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
	results_.clear();
	resultBgImages_.clear();
	choiceRects_.clear();
}


void ResultScene::Transition(void)
{
	// ResultSceneのInitは呼ばれていないため、手動で初期状態をセット
	resultState_ = ResultState::TAIL;
	resultTailIndex_ = 0;
	if (!resultTailMessages_.empty()) {
		msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
	}
	// GameScene を RESULT 状態にする
	GameScene::GetInstance().SetSceneState(SceneState::RESULT);
}


void ResultScene::AddResult(const ChoiceResult& result)
{
	results_.push_back(result);
}

// ... (ResultScene::GetInstance() および ResultScene::DetermineResultType(void) は変更なし)

ResultScene& ResultScene::GetInstance(void)
{
	static ResultScene instance;
	return instance;
}

int ResultScene::DetermineResultType(void)
{
	if (results_.empty()) return 0;

	// 仮のロジック: 質問インデックスの合計を計算し、結果タイプとする
	int sum = 0;
	for (const auto& r : results_) {
		sum += r.questionIndex;
	}
	return sum;
}
