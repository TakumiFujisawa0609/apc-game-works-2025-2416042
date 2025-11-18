#include <DxLib.h>
#include <string>
#include <vector>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include"../Manager/QuestionManager.h"
#include "../Object/Message.h"
#include "GameScene.h"

#pragma region メモ
// 家のディレクトリ
// "C:\DxLib\プロジェクトに追加すべきファイル_VC用"
// 学校のディレクトリ
// $(DXLIB_DIR)

// 今後やること
//・問いを考え、実装
//・TabからESCに変更
//・最終結果を全部みなくても先に遷移できるようにする。
//・タイトルとエンドシーンにもマウス操作の導入とポーズメニューの改善
//・エンドシーンの画像生成
//・enumクラスの核ソースを各シーンに振り分ける。
//・問いによって画像を差し込んで視覚的に理解をできるようにする。
//・最終結果後、世界の在りようを導入(背景やセリフが切り替わる)
//・クリックの音のSEや選択肢を決めてる際のSE
#pragma endregion

GameScene::GameScene(void)
	:inputManager_(InputManager::GetInstance()),
	state_(SceneState::STORY),
	stateBeforePause_(SceneState::STORY),
	gImage_(-1),
	bgmHandle_(-1),
	storyIndex_(0),
	questionIndex_(0),
	selectedChoice_(0),
	afterTalkIndex_(-1),
	pauseSelectIndex_(0),
	currentLineIndex_(0),
	resultTimer_(0),
	leftPressed_(false),
	rightPressed_(false),
	prevQuestionIndex_(-1),
	prevSelectedChoice_(-1),
	pauseUpPressed_(false),
	pauseSelected_(0),
	isAfterTalkActive_(false),
	resultDisplayed_(false),
	resultType_(0),
	resultState_(ResultState::LIST),
	resultSelectIndex_(0),
	pauseDownPressed_(false),
	justEnteredList_(true),
	resultTailIndex_(0),
	talkMessage_(""),
	prevMouseLeft_(false),
	isLButtonDown_(false)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// 背景画像の読み込み
	gImage_ = LoadGraph("Data/Image/haikei.png");

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

	// BGMの読み込みと再生
	bgmHandle_ = LoadSoundMem("Data/BGM/GameScene.mp3");
	PlaySoundMem(bgmHandle_, DX_PLAYTYPE_LOOP);

	// 文章の初期化
	story_ = {
		/*"やぁ、目が覚めた？。",
		"ここは精神と物質の狭間にある場所。\n"
		"君には少し実験を手伝ってもらいたいんだ。",
		"なに、簡単なことだよ。\n"
		"ただ、僕が出題する問いに答えてもらうだけだから。",
		"あぁ、難しい問いは出ないから安心していいよ。\n"
		"ちょっと複雑な問いはあるかもだけど、君なら大丈夫。",
		"それじゃあ、始める前に少し注意点を説明するね。",
		"まず、君が答えた問いは全て記録される。\n"
		"だから、正直に君の価値観に沿って答えてね。",
		"それと、君が答えた問いは後で可視化されるから\n"
		"楽しみにしててね。",
		"それじゃあ、準備ができたら\n"
		"スペースキーを押して始めよう。",*/
		"まずは例題を出してみるね。",
	};
	// 問いの内容
	questions_ = {
		{
		/*	"もし、今後人生でパンかご飯の片方だけしか\n"
		"食べられないとしたら、どちらを選ぶ？",
		{{"パン" , -1, 470, 760} ,
		{ "ご飯" , -1, 1330, 760 }}
		},*/
		"もし、今後人生でパンかご飯の片方だけしか\n"
		"食べられないとしたら、どちらを選ぶ？",
		{{"パン" , 1, 470, 760} ,
		{ "ご飯" , 2, 1330, 760 }}
		},
	 { "日本でパンの製造ができなくなり、輸入を頼るしかなくなった。\n"
		"値段は今の10倍だけど、それでも君はパンを購入する？",
		{ {"購入する", -1, 440, 760}, 
		{"購入しない", -1, 1270, 760}}
		},
	{ "世界的にお米の生産が大幅に減少し、お米の価値が高騰した。\n"
		"値段は今の10倍となった場合、それでも君はご飯を選ぶ？",
		{ {"選ぶ", -1, 480, 760},
		{"選ばない", -1, 1290, 760} }
		},
	};

	// 解答後の会話
	afterTalks_ = {
	{{"なるほど、君はパン派なんだね。",
		"確かに、パンだけでも様々な種類があって美味しいよね。",
		"なら、もし日本での製造ができない場合って考えたことある？"},0, 0},
	{{"なるほど、君はご飯派なんだね。",
		"確かに、ご飯をしばらく食べていないと恋しくなるくらい\n"
		"日本人にとって欠かせない食べ物だよね。",
		"なら、お米の生産が物凄く減った場合って考えたことはある？"},0, 1},
	{{"そうなんだね。\n"
		"君はお金を使ってでも食べたいくらいパンが大好きなんだね。"	}, 1, 0},
	{{"そうなんだね。\n"
		"やっぱり物価が高騰した状態では購入するのは厳しいよね。"}, 1, 1},
	{{"なるほど！\n"
		"素晴らしい！君は日本人の鑑だ！！",
		"君はお金を使ってでもご飯を食べたいんだね！",}, 2, 0},
	{{"そうなんだ。\n"
		"ご飯を選んでも物価の高騰で値段が上がると選びにくいよね。"}, 2, 1},
	};

	resultTailMessages_ = {
		"これで全ての質問が終わったよ。",
		"どうだった？正直に答えることはできたかい？",
		"君の選択は全て記録され、ここから確認できるよ。",
		"それじゃあ、君の解答結果を見てみよう。",
	};

	// メッセージの初期化
	storyIndex_ = 0;
	questionIndex_ = 0;
	selectedChoice_ = 0;
	state_ = SceneState::STORY;

	// メッセージオブジェクトの初期化
	msg_.Init();
	msg_.SetMessage(story_[storyIndex_]);

	leftPressed_ = false;
	rightPressed_ = false;

	resultDisplayed_ = false;

	// リザルト関連の初期化
	resultState_ = ResultState::LIST;
	resultSelectIndex_ = 0;
	pauseDownPressed_ = false;
	results_.clear();
	resultTailIndex_ = 0;

	ManagerInit();
}

void GameScene::ManagerInit(void)
{
	// QuestionManagerの初期化
	std::vector<QuestionData> qdata;
	qdata.reserve(questions_.size());
	for (auto& q : questions_) {
		QuestionData dq;
		dq.questionText = q.text;
		dq.choices = {};
		dq.choiceCounts.clear();
		for (auto& ch : q.choices) {
			dq.choices.push_back(ch.text);
			dq.choiceCounts.push_back(0); // 初期は0、LoadDataで上書きされる
		}
		qdata.push_back(std::move(dq));
	}
	questionManager_.SetQuestions(qdata);

	questionManager_.LoadData();
}

void GameScene::Update(void)
{
	// マウスカーソルを表示
	SetMouseDispFlag(TRUE);

	// メッセージ更新
	msg_.Update();

	// --- ポーズ ---
	if (inputManager_.IsTrgDown(KEY_INPUT_TAB))
	{
		if (state_ == SceneState::RESULT && 
			resultState_ == ResultState::TAIL ||
			 resultState_ == ResultState::DETAIL)
		{
			// 無視して何もしない
		}
		else if (state_ != SceneState::PAUSE)
		{
			stateBeforePause_ = state_;
			state_ = SceneState::PAUSE;
			pauseSelectIndex_ = 0;

			if (stateBeforePause_ == SceneState::RESULT)
			{
				msg_.SetMessage("");
			}
		}
	}

	// マウス座標とボタン状態を取得
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

	switch (state_)
	{
#pragma region ストーリー
	case SceneState::STORY:
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
		{
			if (!msg_.IsFinished())
			{
				msg_.Skip();
			}
			else
			{
				storyIndex_++;
				if (storyIndex_ < static_cast<int>(story_.size()))
				{
					msg_.SetMessage(story_[storyIndex_]);
				}
				else
				{
					state_ = SceneState::QUESTION;
					msg_.SetMessage(questions_[questionIndex_].text);
				}
			}
		}
		break;
#pragma endregion

#pragma region 問題表示
	case SceneState::QUESTION:
	{
		if (!msg_.IsFinished()) break;

	

		// --- キーボード操作（既存） ---
		if (inputManager_.IsTrgDown(KEY_INPUT_W))
		{
			selectedChoice_ = (selectedChoice_ == 0) ? (int)questions_[questionIndex_].choices.size() - 1 : selectedChoice_ - 1;
		}
		else if (inputManager_.IsTrgDown(KEY_INPUT_S))
		{
			selectedChoice_ = (selectedChoice_ == (int)questions_[questionIndex_].choices.size() - 1) ? 0 : selectedChoice_ + 1;
		}

		// --- ★追加: マウス操作 ---
		{
			int newSelected = selectedChoice_;
			bool isMouseOverChoice = false;

			// マウスオーバー判定
			for (size_t i = 0; i < choiceRects_.size(); ++i) {
				const auto& rect = choiceRects_[i];
				if (mouseX >= rect.left && mouseX <= rect.right &&
					mouseY >= rect.top && mouseY <= rect.bottom)
				{
					newSelected = (int)i; // マウスオーバーした選択肢にカーソルを移動
					isMouseOverChoice = true;
					break;
				}
			}

			if (isMouseOverChoice) {
				selectedChoice_ = newSelected;
			}

			// マウスクリック決定処理
			if (isLButtonTrg && isMouseOverChoice) {
				// マウスで選択した項目が選択されている状態にし、決定処理へ
				selectedChoice_ = newSelected;
				// 決定処理のフラグを立てて、キー入力と同じ決定ロジックへ移行
				goto CHOICE_DECISION;
			}
		}

		// 選択肢決定 (キーボード操作)
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
		{
			goto CHOICE_DECISION;
		}

	CHOICE_DECISION:

		// --- スペース or マウスクリックで決定 ---
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
		{
			prevQuestionIndex_ = questionIndex_;
			prevSelectedChoice_ = selectedChoice_;

			questionManager_.SelectChoice(questionIndex_, selectedChoice_);
			questionManager_.SaveData();

			// 結果を保存
			ChoiceResult result;
			result.questionIndex = questionIndex_;
			result.questionText = questions_[questionIndex_].text;
			result.selectedChoiceText = questions_[questionIndex_].choices[selectedChoice_].text;
			results_.push_back(result);

			// --- アフタートーク判定 ---
			afterTalkIndex_ = -1;
			for (int i = 0; i < static_cast<int>(afterTalks_.size()); i++)
			{
				if (afterTalks_[i].questionIndex == questionIndex_ &&
					afterTalks_[i].choiceIndex == selectedChoice_)
				{
					afterTalkIndex_ = i;
					break;
				}
			}

			if (afterTalkIndex_ >= 0)
			{
				state_ = SceneState::ANSWER_TALK;
				currentLineIndex_ = 0;
				isAfterTalkActive_ = true;
				msg_.SetMessage(afterTalks_[afterTalkIndex_].lines[currentLineIndex_]);
			}
			else
			{
				int follow = questions_[questionIndex_].choices[selectedChoice_].nextIndex;
				if (follow >= 0)
				{
					questionIndex_ = follow;
					msg_.SetMessage(questions_[questionIndex_].text);
				}
				else
				{
					state_ = SceneState::RESULT;
					resultDisplayed_ = false;
					resultState_ = ResultState::TAIL;
					resultTailIndex_ = 0;
					msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
				}
			}
		}

		break;
	}
#pragma endregion


#pragma region 解答後の会話
	case SceneState::ANSWER_TALK:
		if (!msg_.IsFinished()) break;

		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
		{
			// 会話の次の行へ
			currentLineIndex_++;
			auto& lines = afterTalks_[afterTalkIndex_].lines;

			// 会話終了判定
			if (currentLineIndex_ >= static_cast<int>(lines.size()))
			{
				// 会話終了
				isAfterTalkActive_ = false;
				int next = questions_[prevQuestionIndex_].choices[prevSelectedChoice_].nextIndex;
				if (next >= 0)
				{
					questionIndex_ = next;
					state_ = SceneState::QUESTION;
					msg_.SetMessage(questions_[questionIndex_].text);
				}
				else
				{
					state_ = SceneState::RESULT;
					resultDisplayed_ = false;
					resultState_ = ResultState::TAIL;
					resultTailIndex_ = 0;
					msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
				}
			}
			else
			{
				msg_.SetMessage(lines[currentLineIndex_]);
			}
		}
		break;

#pragma endregion

#pragma region 最終結果表示
	case SceneState::RESULT:
	{
		if (!resultDisplayed_)
		{
			resultDisplayed_ = true;
			resultType_ = DetermineResultType();

			// 背景を切り替え（存在する数以内で）
			if (!resultBgImages_.empty()) {
				currentBgIndex_ = resultType_ % resultBgImages_.size();
			}
		}


		// --- 結果画面の状態別処理 ---
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
						state_ = SceneState::END;
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
		break;
	}
#pragma endregion

#pragma region シーン終了
	case SceneState::END:
		// スペースキーでクリアシーンへ
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
		{
			if (!msg_.IsFinished()) msg_.Skip();
			else
			{
				StopSoundMem(bgmHandle_);
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
			}
		}
		break;
#pragma endregion

#pragma region ポーズメニュー
	case SceneState::PAUSE:
		if (inputManager_.IsTrgDown(KEY_INPUT_W))
			pauseSelectIndex_ = (pauseSelectIndex_ - 1 + 3) % 3;
		if (inputManager_.IsTrgDown(KEY_INPUT_S))
			pauseSelectIndex_ = (pauseSelectIndex_ + 1) % 3;

		{
			int mouseX, mouseY;
			GetMousePoint(&mouseX, &mouseY);
			bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_; // Update冒頭で取得した変数を使用

			int newSelected = pauseSelectIndex_;
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
				pauseSelectIndex_ = newSelected;
			}

			// マウスクリック決定処理
			if (isLButtonTrg && isMouseOverChoice) {
				goto PAUSE_DECISION;
			}
		}

		// 選択肢決定 (キーボード操作)
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
		{
			goto PAUSE_DECISION;
		}

		// ★追加: 決定ロジックの開始点としてラベルを定義
	PAUSE_DECISION:

		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
		{
			switch (pauseSelectIndex_)
			{
			case 0: state_ = stateBeforePause_; break; // 続ける
			case 1: SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE); break;
			case 2: DxLib_End(); break;
			}
		}
		break;
#pragma endregion
	}

	// --- デバッグ用 ---
	// Rキーで問いデータリセット
	if (inputManager_.IsTrgDown(KEY_INPUT_BACK))
	{
		questionManager_.ResetData();
	}
}

void GameScene::Draw(void)
{
	// --- 背景 ---
	int bgToDraw = gImage_;

	if (state_ == SceneState::RESULT)
	{
		if (!resultBgImages_.empty())
		{
			bgToDraw = resultBgImages_[currentBgIndex_];
		}
	}

	DrawGraph(0, 0, bgToDraw, FALSE);


	// 吹き出しの描画
	DrawBox(145, 45, 1750, 300, GetColor(255, 255, 255), true);   // 白い吹き出し背景
	DrawBox(150, 50, 1745, 295, GetColor(0, 0, 0), true);        // 黒い枠線

	// 吹き出しのメッセージ描画 (RESULT/PAUSE以外、またはRESULTのTAIL状態でのみ描画)
	if (state_ != SceneState::RESULT || resultState_ == ResultState::TAIL)
	{
		msg_.Draw(165, 65);
	}

	if (state_ == SceneState::QUESTION)
	{
		// 問いの選択肢の背景枠(左側)	DrawBox(左側面、上、右側面、下) 
		DrawBox(325, 490, 740, 880, GetColor(255, 255, 255), true);  // 白背景
		DrawBox(330, 495, 735, 875, GetColor(0, 0, 0), true);       // 黒枠線

		// 問いの選択肢の背景枠(右側)
		DrawBox(1180, 490, 1595, 880, GetColor(255, 255, 255), true);  // 白背景
		DrawBox(1185, 495, 1590, 875, GetColor(0, 0, 0), true);       // 黒枠線

		// 選択肢の描画
		DrawChoices(questions_[questionIndex_].choices, selectedChoice_, false);

		// 左右キーのヒント
		SetFontSize(60);
		DrawFormatString(0, 1000, GetColor(255, 255, 255), "A/Dキーで操作、Spaceで選択");
	}
	// 解答後の会話と結果表示
	else if (state_ == SceneState::ANSWER_TALK) {
		if (afterTalkIndex_ >= 0 && afterTalkIndex_ < (int)afterTalks_.size()) {

			const auto& talk = afterTalks_[afterTalkIndex_];

			// 見た目用の question（GameScene が持つもの）
			if (talk.questionIndex < 0 || talk.questionIndex >= (int)questions_.size()) return;
			const auto& question = questions_[talk.questionIndex];

			// manager 側のデータを取得（安全チェック）
			const auto& managerQuestions = questionManager_.GetQuestions();
			if (talk.questionIndex < 0 || talk.questionIndex >= (int)managerQuestions.size()) {
				// manager 側にデータが無ければ表示しない（またはログ）
				DrawFormatString(350, 470, GetColor(255, 0, 0), "No manager data for question %d", talk.questionIndex);
				return;
			}
			const auto& questionData = managerQuestions[talk.questionIndex];

			// 描画枠
			DrawBox(160, 450, 1735, 950, GetColor(255, 255, 255), true);
			DrawBox(165, 455, 1730, 945, GetColor(0, 0, 0), true);

			// 元の問い（見た目）
			DrawString(175, 470, question.text.c_str(), GetColor(255, 255, 255));

			// 合計票数（manager 側）
			int total = 0;
			for (int v : questionData.choiceCounts) total += v;

			// 選択肢と割合の表示（manager 側の counts を使う）
			int y = 700; // 縦の開始位置
			for (size_t i = 0; i < question.choices.size(); i++) {
				const auto& cVisual = question.choices[i];

				// manager 側の count を安全に取得
				int count = 0;
				if (i < questionData.choiceCounts.size()) {
					count = questionData.choiceCounts[i];
				}
				// 割合計算
				float percent = (total > 0) ? (100.0f * count / (float)total) : 0.0f;

				int color = (i == talk.choiceIndex) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

				// 選択肢文字
				DrawString(400, y, cVisual.text.c_str(), color);

				// 割合（と件数）
				DrawFormatString(900, y, GetColor(255, 255, 0), "%.1f%%", percent, count);

				// 次の行へ
				y += 60;
			}
		}
	}
	else if (state_ == SceneState::PAUSE)
	{
		// 画面を少し暗くする
		DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150); // 半透明黒
		DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		SetFontSize(100);
		// メニュータイトル
		DrawString(700, 300, "ポーズ中", GetColor(255, 255, 0));
		const std::vector<std::string> PAUSE_MENU = { "ゲームに戻る", "タイトルに戻る", "ゲーム終了"};
		int startX = 710;
		int startY = 500;
		int spacing = 100;
		int choiceHeight = 60; // 文字の高さ+余裕

		// ★追加: 矩形リストをクリアし、設定を初期化
		choiceRects_.clear();
		SetFontSize(50);

		for (size_t i = 0; i < PAUSE_MENU.size(); i++)
		{
			int currentY = startY + (int)i * spacing;
			int color = (i == pauseSelectIndex_) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
			const std::string& text = PAUSE_MENU[i];

			// 文字列の幅を取得 (DxLib)
			int choiceWidth = GetDrawStringWidth(text.c_str(), (int)text.size());

			// 選択時の背景描画
			if (i == pauseSelectIndex_)
			{
				DrawBox(startX - 10, currentY - 5, startX + choiceWidth + 10, currentY + choiceHeight + 5, GetColor(50, 50, 50), TRUE);
			}

			// 描画
			DrawString(startX, currentY, text.c_str(), color);

			// ★追加: 矩形を保存
			choiceRects_.push_back({
				startX - 10,
				currentY - 5,
				startX + choiceWidth + 10,
				currentY + choiceHeight + 5
				});
		}

		// ポーズメニューの操作ヒント
		SetFontSize(60);
		DrawFormatString(0, 1000, GetColor(255, 255, 0), "W/Sキーで操作、Spaceまたはクリックで選択");

		
		
	}
	else if (state_ == SceneState::RESULT)
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
			int nextY = 900 ;
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
	// --- マウスカーソル描画 ---
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	DrawCircle(mouseX, mouseY, 10, GetColor(255, 255, 0), TRUE);  // 黄色い丸のカーソル
}

void GameScene::DrawChoices(const std::vector<Choice>& choices, int cursorIndex, bool showPercent)
{
	// 選択肢の描画
	int total = 0;
	if (showPercent) {
		for (auto& c : choices) total += c.count;
	}

	// マウス用矩形リストのクリアと設定
	choiceRects_.clear();
	SetFontSize(50); // フォントサイズを再設定

	// マウスの当たり判定に必要なサイズを定義（既存の選択肢背景サイズに合わせる）
	int choiceHeight = 60;  // ※文字サイズ50を基準とした高さ。背景サイズに合わせて調整してください。

	// 選択肢の表示
	for (size_t i = 0; i < choices.size(); i++) {
		int choiceWidth = GetDrawStringWidth(choices[i].text.c_str(), (int)choices[i].text.size());
		int color = (i == cursorIndex) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

		DrawString(choices[i].x, choices[i].y, choices[i].text.c_str(), color);

		// 割合の表示
		if (showPercent && total > 0) {
			float percentage_ = (choices[i].count / (float)total) * 100.0f;
			char buf[64]{};
			DrawFormatString(choices[i].x, choices[i].x + 50, GetColor(255, 255, 0), "%d%%", percentage_);
		}
		// マウス当たり判定用の矩形を保存 (choices[i].x/yを利用)
		// ----------------------------------------------------
			choiceRects_.push_back({
			choices[i].x - 155,        // 左端 (背景の描画に合わせて)
			choices[i].y - 260,         // 上端 (背景の描画に合わせて)
			choices[i].x + choiceWidth + 155, // 右端 (文字の幅に合わせる)
			choices[i].y + choiceHeight + 55  // 下端 (文字の高さに合わせる)
			});
		// ----------------------------------------------------
	}
}
void GameScene::Release(void)
{
	// 背景画像の解放
	if (gImage_ != -1)
	{
		DeleteGraph(gImage_);
		gImage_ = -1;
	}
	// BGMの解放
	if (bgmHandle_ != -1)
	{
		StopSoundMem(bgmHandle_);
		DeleteSoundMem(bgmHandle_);
		bgmHandle_ = -1;
	}
}

void GameScene::NextQuestion(int nextIndex_)
{
	if (nextIndex_ >= 0) {
		questionIndex_ = nextIndex_;
		selectedChoice_ = 0;
		msg_.SetMessage(questions_[questionIndex_].text);
		state_ = SceneState::QUESTION;
	}
	else
	{
		// 変更: 全問終了 → RESULTのTAIL状態へ
		state_ = SceneState::RESULT;
		resultState_ = ResultState::TAIL;
		resultTailIndex_ = 0;
		msg_.SetMessage(resultTailMessages_[resultTailIndex_]);
	}
}

int GameScene::DetermineResultType(void)
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