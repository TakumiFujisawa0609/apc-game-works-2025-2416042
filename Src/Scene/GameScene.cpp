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
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// 背景画像の読み込み
	gImage_ = LoadGraph("Data/Image/haikei.png");

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
	// 問いの初期化
	questions_ = {
		{
		"もし、今後人生でパンかご飯の片方だけしか\n"
		"食べられないとしたら、どちらを選ぶ？",
		{{"パン" , 1, 470, 760} ,
		{ "ご飯" , 2, 1330, 760 }}
		},
	 { "じゃーパンは何が好き？",
		{ {"焼きたて", 3, 410, 760}, 
		{"バター", 3, 1310, 760}}
		},
		{ "じゃー和食派？洋食派？",
		{ {"和食派", 3, 450, 760},
		{"洋食派", 3, 1310, 760} }
		},
		{
		"ピカチュウ?",
		{ {"ピカチュウ", -1,  380, 760},
		{"ピカチュウ", -1, 1280, 760} }
		},
	};

	// 解答後の会話の初期化
	afterTalks_ = {
		{"なるほど、君はパン派なんだね。", 0, 0},
		{"なるほど、君はご飯派なんだね。", 0, 1},
		{"そうなんだ。僕は焼きたてのパンが好きだな。", 1, 0},
		{"そうなんだ。僕はバターたっぷりのパンが好きだな。", 1, 1},
		{"そうなんだ。僕は和食派かな。", 2, 0},
		{"そうなんだ。僕は洋食派かな。", 2, 1},
		{"ピカチュウ", 3, 0},
		{"ピカチュウ。", 3, 1},
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

	questionManager_.LoadData();
}

void GameScene::Update(void)
{
	msg_.Update();

	switch (state_)
	{
	case SceneState::STORY:
		// ストーリーを表示して終わったら次の文章へ
		if (msg_.IsFinished() && CheckHitKey(KEY_INPUT_SPACE))
		{
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
		break;

	case SceneState::QUESTION:
		// 問いを表示して選択肢が選ばれたら結果へ
		if (msg_.IsFinished()) {
			//DrawChoices(questions_[questionIndex_].choices, selectedChoice_, );

			// 左右キーで選択
			if (CheckHitKey(KEY_INPUT_A)) { // 左
				if (!leftPressed_) {
					selectedChoice_ = (selectedChoice_ - 1 + (int)questions_[questionIndex_].choices.size())
						% (int)questions_[questionIndex_].choices.size();
					leftPressed_ = true;
				}
			}
			else
			{
				leftPressed_ = false;
			}

			if (CheckHitKey(KEY_INPUT_D)) { // 右
				if (!rightPressed_) {
					selectedChoice_ = (selectedChoice_ + 1)
						% (int)questions_[questionIndex_].choices.size();
					rightPressed_ = true;
				}
			}
			else
			{
				rightPressed_ = false;
			}

			// 決定
			if (inputManager_.IsTrgDown(KEY_INPUT_SPACE)) {
				// カウントアップ
				auto& choice = questions_[questionIndex_].choices[selectedChoice_];
				choice.count++; // 選択肢の選ばれた回数をカウント

				// 全体集計
				questionManager_.SelectChoice(questionIndex_, selectedChoice_);

				prevQuestionIndex_ = questionIndex_;
				prevSelectedChoice_ = selectedChoice_;

				afterTalkIndex_ = -1;
				for (int i = 0; i <(int) afterTalks_.size(); i++) {
					if (afterTalks_[i].questionIndex == questionIndex_ &&
						afterTalks_[i].choiceIndex == selectedChoice_) {
						afterTalkIndex_ = i;
						break;
					}
				}

			//	resultLog_.push_back({ questions_[questionIndex_].text, choice.text });

				if (afterTalkIndex_ >= 0) {
					// アフタートークの文をセットして遷移
					msg_.SetMessage(afterTalks_[afterTalkIndex_].text);
					state_ = SceneState::ANSWER_TALK;
				}
				else {
					// アフタートークが見つからなければそのまま次へ
					int follow = choice.nextIndex;
					if (follow >= 0) {
						questionIndex_ = follow;
						msg_.SetMessage(questions_[questionIndex_].text);
						state_ = SceneState::QUESTION;
					}
					else {
						state_ = SceneState::END;
					}
				}
			}
			break;

	case SceneState::ANSWER_TALK:
		// 選択した後の会話を表示して終わったら結果へ
		if (msg_.IsFinished() && inputManager_.IsTrgDown(KEY_INPUT_SPACE)) {
			// 次の質問へ
			int next = questions_[prevQuestionIndex_].choices[prevSelectedChoice_].nextIndex;
			if (next == -1) {
				state_ = SceneState::END;
				msg_.SetMessage("これで終わりだよ。");
			}
			else {
				questionIndex_ = next;
				state_ = SceneState::QUESTION;
				msg_.SetMessage(questions_[questionIndex_].text);
			}
		}
		break;

	case SceneState::RESULT:
		// 集計結果の表示
		resultTimer_--;
		if (resultTimer_ <= 0) {
			auto& choice = questions_[questionIndex_].choices[selectedChoice_];
			NextQuestion(choice.nextIndex); // 次の問題 or END へ
		}
		break;

	case SceneState::END:
		// 最後の文章を表示して終わったらクリアシーンへ
		if (msg_.IsFinished() && CheckHitKey(KEY_INPUT_SPACE))
		{
			// シーン終了
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
		}
		break;
		}
	}
}

void GameScene::Draw(void)
{
	// 背景画像の描画
	DrawGraph(0, 0, gImage_, true);
	// 吹き出しの描画
	DrawBox(145, 45, 1750, 300, GetColor(255, 255, 255), true);   // 白い吹き出し背景
	DrawBox(150, 50, 1745, 295, GetColor(0, 0, 0), true);        // 黒い枠線

	msg_.Draw(165, 65);
	
	if (state_ == SceneState::QUESTION) {
		// 問いの背景枠(左側)	DrawBox(左側面、上、右側面、下) 
		DrawBox(325, 490, 740, 880,  GetColor(255, 255, 255), true);  // 白背景
		DrawBox(330, 495, 735, 875, GetColor(0, 0, 0), true);       // 黒枠線

		// 問いの背景枠(右側)
		DrawBox(1180, 490, 1595, 880, GetColor(255, 255, 255), true);  // 白背景
		DrawBox(1185, 495, 1590, 875, GetColor(0, 0, 0), true);       // 黒枠線

		// 選択肢の描画
		DrawChoices(questions_[questionIndex_].choices, selectedChoice_, false);
	}
	else if (state_ == SceneState::ANSWER_TALK) {
		if (afterTalkIndex_ >= 0 && afterTalkIndex_ < (int)afterTalks_.size()) {

			const auto& talk = afterTalks_[afterTalkIndex_];
			const auto& question = questions_[talk.questionIndex];

			// 吹き出し or 背景枠
			DrawBox(300, 450, 1600, 950, GetColor(255, 255, 255), true);
			DrawBox(305, 455, 1595, 945, GetColor(0, 0, 0), true);

			// アフタートーク本文
		//	DrawString(350, 470, talk.text.c_str(), GetColor(255, 255, 255));

			// 元の問いを表示
			DrawString(350, 470, question.text.c_str(), GetColor(255, 255, 255));

			// 各選択肢と割合を表示
			const auto& questions = questionManager_.GetQuestions();
			int total = 0;
			int y = 700; // 縦の開始位置
			for (auto count : questions[questionIndex_].choiceCounts)
				total += count;


			for (size_t i = 0; i < question.choices.size(); i++) {
				auto& c = question.choices[i];
				float percent = (total > 0) ? (100.0f * c.count / total) : 0.0f;

				int color = (i == talk.choiceIndex) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

				// 選択肢文字
				DrawString(400, y, c.text.c_str(), color);

				// 割合
				DrawFormatString(900, y, GetColor(255, 255, 0), "%.1f%%", percent);

				y += 60; // 次の行へ
			}
		}
	}
	else if (state_ == SceneState::RESULT){
		// 最終結果表示
		if (prevQuestionIndex_ >= 0 && prevQuestionIndex_ < (int)questions_.size()) {
			DrawChoices(questions_[prevQuestionIndex_].choices, -1, true);
		}
	}
}

void GameScene::DrawChoices(const std::vector<Choice>& choices, int cursorIndex, bool showPercent)
{
	int total = 0;
	if (showPercent) {
		for (auto& c : choices) total += c.count;
	}

	for (size_t i = 0; i < choices.size(); i++) {
		int color = (i == cursorIndex) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

		DrawString(choices[i].x, choices[i].y, choices[i].text.c_str(), color);

		// 割合の表示
		if (showPercent &&  total > 0) {
			float percentage_ = (choices[i].count / (float)total) * 100.0f;
			char buf[64];
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
		msg_.SetMessage("これで実験は終了だよ。\nお疲れ様！");
	}
}

