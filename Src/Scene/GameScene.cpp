#include <DxLib.h>
#include <string>
#include <vector>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include"../Manager/QuestionManager.h"
#include "../Object/Message.h"
#include "GameScene.h"

// 家のディレクトリ
// "C:\DxLib\プロジェクトに追加すべきファイル_VC用"
// 学校のディレクトリ
// $(DXLIB_DIR)

GameScene::GameScene(void)
	:inputManager_(InputManager::GetInstance())
	, state_(SceneState::STORY)
	, stateBeforePause_(SceneState::STORY)
	, gImage_(-1)
	, bgmHandle_(-1)
	, storyIndex_(0)
	, questionIndex_(0)
	, selectedChoice_(0)
	, afterTalkIndex_(-1)
	, pauseSelectIndex_(0)
	, currentLineIndex_(0)
	, resultTimer_(0)
	, leftPressed_(false)
	, rightPressed_(false)
	, prevQuestionIndex_(-1)
	, prevSelectedChoice_(-1)
	, pauseUpPressed_(false)
	, pauseSelected_(0)
	, isAfterTalkActive_(false)
	, resultDisplayed_(false)
	, resultType_(0)
	, resultState_(ResultState::LIST)
	, resultSelectIndex_(0)
	, pauseDownPressed_(false)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// 背景画像の読み込み
	gImage_ = LoadGraph("Data/Image/haikei.png");
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
			"もし、今後人生でパンかご飯の片方だけしか\n"
		"食べられないとしたら、どちらを選ぶ？",
		{{"パン" , -1, 470, 760} ,
		{ "ご飯" , -1, 1330, 760 }}
		},
		/*"もし、今後人生でパンかご飯の片方だけしか\n"
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
		},*/
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
	/*{{"そうなんだね。\n"
		"君はお金を使ってでも食べたいくらいパンが大好きなんだね。"	}, 1, 0},
	{{"そうなんだね。\n"
		"やっぱり物価が高騰した状態では購入するのは厳しいよね。"}, 1, 1},
	{{"なるほど！\n"
		"素晴らしい！君は日本人の鑑だ！！",
		"君はお金を使ってでもご飯を食べたいんだね！",}, 2, 0},
	{{"そうなんだ。\n"
		"ご飯を選んでも物価の高騰で値段が上がると選びにくいよね。"}, 2, 1},*/
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

	resultState_ = ResultState::LIST;
	resultSelectIndex_ = 0;
	pauseDownPressed_ = false;
	results_.clear();

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
	msg_.Update();

	// ポーズ
	if (inputManager_.IsTrgDown(KEY_INPUT_TAB))
	{
		if (state_ != SceneState::PAUSE)
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


	switch (state_)
	{
#pragma region ストーリー
	case SceneState::STORY:
		// ストーリーを表示して終わったら次の文章へ
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
		{
			// まだ表示中なら一気に表示
			if (!msg_.IsFinished())
			{
				msg_.Skip();
			}
			else
			{
				// 次の文章へ
				storyIndex_++;
				if (storyIndex_ < (int)story_.size())
				{
					msg_.SetMessage(story_[storyIndex_]);
				}
				else
				{
					// ストーリー終わったら問いへ
				//	storyIndex_ = 0;
					state_ = SceneState::QUESTION;
					msg_.SetMessage(questions_[questionIndex_].text);
				}
			}
		}
		break;
#pragma endregion

#pragma region 問題表示
	case SceneState::QUESTION:
		if (!msg_.IsFinished())
			break; // 表示中なら選択肢操作は無効

		// 左右で選択
		if (inputManager_.IsTrgDown(KEY_INPUT_A) && !leftPressed_)
		{
			selectedChoice_ = (selectedChoice_ - 1 + (int)questions_[questionIndex_].choices.size())
				% (int)questions_[questionIndex_].choices.size();
			leftPressed_ = true;
		}
		else if (!inputManager_.IsTrgDown(KEY_INPUT_A))
		{
			leftPressed_ = false;
		}

		if (inputManager_.IsTrgDown(KEY_INPUT_D) && !rightPressed_)
		{
			selectedChoice_ = (selectedChoice_ + 1) % (int)questions_[questionIndex_].choices.size();
			rightPressed_ = true;
		}
		else if (!inputManager_.IsTrgDown(KEY_INPUT_D))
		{
			rightPressed_ = false;
		}

		// 決定
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
		{
			prevQuestionIndex_ = questionIndex_;
			prevSelectedChoice_ = selectedChoice_;

			questionManager_.SelectChoice(questionIndex_, selectedChoice_);
			questionManager_.SaveData();

			// 結果の記録
			ChoiceResult result;
			result.questionIndex = questionIndex_;
			result.questionText = questions_[questionIndex_].text;
			result.selectedChoiceText =
				questions_[questionIndex_].choices[selectedChoice_].text;
			results_.push_back(result);

			// アフタートーク検索
			afterTalkIndex_ = -1;
			for (int i = 0; i < (int)afterTalks_.size(); i++)
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
				}
			}
		}
		break;
#pragma endregion

#pragma region 解答後の会話
	case SceneState::ANSWER_TALK:
		// スペースキーで次の行へ
		if (!msg_.IsFinished())
			break;

		// アフタートークを表示して終わったら次の問いへ		

			// スペースで次の行へ
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
		{
			currentLineIndex_++;
			auto& lines = afterTalks_[afterTalkIndex_].lines;

			// 最後まで表示したら通常ルートへ戻る
			if (currentLineIndex_ >= static_cast<int>(lines.size()))
			{
				isAfterTalkActive_ = false;

				// 次の質問へ（元の処理をここに移動）
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
				}
			}
			else
			{
				// 次の行をセット
				msg_.SetMessage(lines[currentLineIndex_]);
			}
		}

		break;
#pragma endregion

#pragma region 結果表示
	case SceneState::RESULT:
		// 一度だけ結果生成
		if (!resultDisplayed_)
		{
			resultDisplayed_ = true;

			int positive = 0, calm = 0;
			for (auto& r : results_)
			{
				if (r.selectedChoiceText.find("挑戦") != std::string::npos) positive++;
				else if (r.selectedChoiceText.find("安定") != std::string::npos) calm++;
			}
			resultType_ = (positive > calm) ? 0 : 1;
			msg_.SetMessage("");
		}
		if (inputManager_.IsTrgDown(KEY_INPUT_ESCAPE) || inputManager_.IsTrgDown(KEY_INPUT_TAB))
		{
			StopSoundMem(bgmHandle_);
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			break;
		}
#pragma endregion

#pragma region リザルト操作
		switch (resultState_)
		{
		case ResultState::LIST:
			int listSize = static_cast<int>(results_.size());
			int totalOptions = listSize + 1; // 結果リスト＋「次に進む」

			// W/Sキーで選択項目を上下に移動
			if (inputManager_.IsTrgDown(KEY_INPUT_W) && !pauseDownPressed_)
			{
				resultSelectIndex_ = (resultSelectIndex_ - 1 + (int)results_.size()) % (int)results_.size();
				pauseDownPressed_ = true;
			}
			else if (inputManager_.IsTrgDown(KEY_INPUT_S) && !pauseDownPressed_)
			{
				resultSelectIndex_ = (resultSelectIndex_ + 1) % (int)results_.size();
				pauseDownPressed_ = true;
			}
			else if (!inputManager_.IsTrgDown(KEY_INPUT_W) && !inputManager_.IsTrgDown(KEY_INPUT_S))
			{
				pauseDownPressed_ = false;
			}

			// Spaceキー押下処理
			if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
			{
				if (resultSelectIndex_ < listSize)
				{
					// 通常の詳細表示
					resultState_ = ResultState::DETAIL;

					std::string detailMsg =
						"【質問 " + std::to_string(resultSelectIndex_ + 1) + " の詳細】\n\n" +
						results_[resultSelectIndex_].questionText + "\n\n" +
						"あなたの選択: " + results_[resultSelectIndex_].selectedChoiceText +
						"\n\nSpaceキーで一覧に戻る。";
					msg_.SetMessage(detailMsg);
				}
				else
				{
					// 「次に進む」選択時
					state_ = SceneState::END;  // ENDシーンへ遷移
					msg_.SetMessage("それじゃあ、次に進もう。");
				}
			}
		break;
#pragma endregion

#pragma region リザルト詳細表示
		case ResultState::DETAIL:
			// 修正点: メッセージ表示が完了していない場合は、いかなる操作も受け付けない
			if (!msg_.IsFinished())
				break; // メッセージ表示中はキー入力を無視

			// Spaceキーが押されたら、一覧に戻る（メッセージ表示完了後のみ反応）
			if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
			{
				resultState_ = ResultState::LIST;
				msg_.SetMessage(""); // メッセージボックスをクリア
			}
			break;
		}

		break;
#pragma endregion

#pragma region シーン終了
	case SceneState::END:
		// 最後の文章を表示して終わったらクリアシーンへ
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
		{
			if (!msg_.IsFinished())
			{
				msg_.Skip();
			}
			else
			{
				// BGM停止
				StopSoundMem(bgmHandle_);

				// シーン終了
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
			}
		}
		break;
#pragma endregion

#pragma region ポーズメニュー
	case SceneState::PAUSE:
		// ポーズメニューの操作
		if (inputManager_.IsTrgDown(KEY_INPUT_W))
		{
			pauseSelectIndex_ = (pauseSelectIndex_ - 1 + 3) % 3;
		}
		if (inputManager_.IsTrgDown(KEY_INPUT_S))
		{
			pauseSelectIndex_ = (pauseSelectIndex_ + 1) % 3;
		}

		// 決定
		if (inputManager_.IsTrgDown(KEY_INPUT_SPACE))
		{
			if (pauseSelectIndex_ == 0)
			{
				// 続ける
				state_ = stateBeforePause_;
			}
			else if (pauseSelectIndex_ == 1)
			{
				// タイトルへ戻る
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
			else
			{
				// 終了
				DxLib_End();
			}
			break;
		}
	}
#pragma endregion
	// デバッグ用：Rキーでデータリセット
	if (inputManager_.IsTrgDown(KEY_INPUT_BACK))
	{
		questionManager_.ResetData();
	}
}

void GameScene::Draw(void)
{
	// 背景画像の描画
	DrawGraph(0, 0, gImage_, true);
	// 吹き出しの描画
	DrawBox(145, 45, 1750, 300, GetColor(255, 255, 255), true);   // 白い吹き出し背景
	DrawBox(150, 50, 1745, 295, GetColor(0, 0, 0), true);        // 黒い枠線
	// 吹き出しのメッセージ描画
	msg_.Draw(165, 65);
	
	if (state_ == SceneState::QUESTION) 
	{
		// 問いの選択肢の背景枠(左側)	DrawBox(左側面、上、右側面、下) 
		DrawBox(325, 490, 740, 880,  GetColor(255, 255, 255), true);  // 白背景
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

		// ポーズメニューの操作ヒント
		SetFontSize(60);
		DrawFormatString(0, 1000, GetColor(255, 255, 0), "W/Sキーで操作、Spaceで選択");

		SetFontSize(100);
		// メニュータイトル
		DrawString(650, 300, "ポーズ中", GetColor(255, 255, 0));

		// 選択肢
		DrawString(650, 400, pauseSelectIndex_ == 0 ? "> 続ける" : "  続ける", GetColor(255, 255, 255));
		DrawString(650, 500, pauseSelectIndex_ == 1 ? "> タイトルへ" : "  タイトルへ", GetColor(255, 255, 255));
		DrawString(650, 600, pauseSelectIndex_ == 2 ? "> 終了する" : "  終了する", GetColor(255, 255, 255));
	}
	else if (state_ == SceneState::RESULT)
	{
		// 画面全体を結果表示用に暗くしない（QUESTION/ANSWER_TALKと同じ背景）

		// メインメッセージボックスの描画
		DrawBox(145, 45, 1750, 300, GetColor(255, 255, 255), true);   // 白い背景
		DrawBox(150, 50, 1745, 295, GetColor(0, 0, 0), true);        // 黒い枠線

		// 描画枠
		DrawBox(160, 320, 1735, 1050, GetColor(255, 255, 255), true);
		DrawBox(165, 325, 1730, 1045, GetColor(0, 0, 0), true);

		SetFontSize(36);

		// 「全問解答結果」を中央上部に表示
		DrawString(816, 345, "【全問解答結果】", GetColor(255, 255, 0));

		DrawLine(160, 390, 1735, 390, GetColor(255, 255, 255));


		switch (resultState_)
		{
		case ResultState::LIST:
			// 質問一覧表示
			SetFontSize(32);
			for (size_t i = 0; i < results_.size(); i++)
			{
				int y = 410 + (int)i * 60;
				// 選択中の質問は色を変える
				int color = (i == resultSelectIndex_) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

				std::string line =
					"問" + std::to_string(i + 1) + ": " + results_[i].selectedChoiceText;

				DrawString(210, y, line.c_str(), color);

				// 選択中の行にカーソルを付ける
				if (i == resultSelectIndex_)
				{
					DrawString(180, y, ">", color);
				}
			}

			// 操作ヒント
			SetFontSize(40);
			DrawFormatString(170, 1200, GetColor(255, 255, 0), "W/Sキーで選択、Spaceで詳細表示、Escape/Tabでタイトルへ");
			break;

		case ResultState::DETAIL:
			// 質問詳細表示
			// メッセージオブジェクトを使って詳細を表示
			msg_.Draw(170, 120);

			// 操作ヒント
			SetFontSize(40);
			DrawFormatString(170, 900, GetColor(255, 255, 0), "Spaceで一覧に戻る、Escape/Tabでタイトルへ");
			break;
		}
	}
}

void GameScene::DrawChoices(const std::vector<Choice>& choices, int cursorIndex, bool showPercent)
{
	// 選択肢の描画
	int total = 0;
	if (showPercent) {
		for (auto& c : choices) total += c.count;
	}

	// 
	for (size_t i = 0; i < choices.size(); i++) {
		int color = (i == cursorIndex) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

		DrawString(choices[i].x, choices[i].y, choices[i].text.c_str(), color);

		// 割合の表示
		if (showPercent &&  total > 0) {
			float percentage_ = (choices[i].count / (float)total) * 100.0f;
			char buf[64]{};
			DrawFormatString(choices[i].x, choices[i].x + 50, GetColor(255, 255, 0), "%d%%", percentage_);
		}
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
		// 全問終了 → 終了メッセージへ
		state_ = SceneState::END;
	}
}