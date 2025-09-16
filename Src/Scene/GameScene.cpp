#include <DxLib.h>
#include <string>
#include <vector>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Object/Message.h"
#include "GameScene.h"

GameScene::GameScene(void)
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
		{{"パン" , 1} ,{ "ご飯" , 2 }}
		},
	 { "パンを選んだね。パンは何が好き？",
			{ {"焼きたて", 3}, {"バターたっぷり", 4}}
		},
		{ "ご飯を選んだね。和食派？洋食派？",
			{ {"和食派", 3}, {"洋食派", 4} }
		},
		{
		"ピカチュウ?",
		{ {"ピカチュウ", -1}, {"イーブイ", -1} }
		},
	};
	// 選択肢の初期化
	/*choices_ = {
	};*/

	// メッセージの初期化
	storyIndex_ = 0;
	questionIndex_ = 0;
	selectedChoice_ = 0;
	state_ = SceneState::STORY;

	// メッセージオブジェクトの初期化
	msg_.Init();
	msg_.SetMessage(story_[storyIndex_]);
	//msg_.SetMessage(questions_[storyIndex_]);
	//msg_.SetMessage(choices_[storyIndex_]);

	leftPressed_ = false;
	rightPressed_ = false;
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
		if (msg_.IsFinished()) {
	//		DrawChoices(questions_[questionIndex_].choices, selectedChoice_);

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
			if (CheckHitKey(KEY_INPUT_SPACE)) {
				auto& choice = questions_[questionIndex_].choices[selectedChoice_];
				resultLog_.push_back({ questions_[questionIndex_].text, choice.text });

				 NextQuestion(choice.nextIndex);
				}
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

void GameScene::Draw(void)
{
	// 背景画像の描画
	DrawGraph(0, 0, gImage_, true);
	// 吹き出しの描画
	DrawBox(145, 45, 1750, 300, GetColor(255, 255, 255), true);   // 白い吹き出し背景
	DrawBox(150, 50, 1745, 295, GetColor(0, 0, 0), true);        // 黒い枠線

	msg_.Draw(165, 65);

	if (state_ == SceneState::QUESTION) {
		DrawChoices(questions_[questionIndex_].choices, selectedChoice_);
	}
}

void GameScene::DrawChoices(const std::vector<Choice>& choices, int cursorIndex)
{
	int startY = 850;
	int startX = 750;

	for (size_t i = 0; i < choices.size(); i++) {
		int color = (i == cursorIndex) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
		DrawString(startX + (int)i * 200, startY, choices[i].text.c_str(), color);
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
	}
	else
	{
		// 全問終了 → 終了メッセージへ
		state_ = SceneState::END;
		msg_.SetMessage("これで実験は終了だよ。\nお疲れ様！");
	}
	
}

