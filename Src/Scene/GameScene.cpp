#include <DxLib.h>
#include <string>
#include <vector>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include"../Manager/QuestionManager.h"
#include "../Object/Message.h"
#include "../Scene/PauseScene.h"
#include "../Scene/ResultScene.h"
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
	currentLineIndex_(0),
	resultTimer_(0),
	leftPressed_(false),
	rightPressed_(false),
	prevQuestionIndex_(-1),
	prevSelectedChoice_(-1),
	pauseUpPressed_(false),
	pauseSelected_(0),
	isAfterTalkActive_(false),
	pauseDownPressed_(false),
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
	//afterTalks_ = {
	//{{"なるほど、君はパン派なんだね。",
	//	"確かに、パンだけでも様々な種類があって美味しいよね。",
	//	"なら、もし日本での製造ができない場合って考えたことある？"},0, 0},
	//{{"なるほど、君はご飯派なんだね。",
	//	"確かに、ご飯をしばらく食べていないと恋しくなるくらい\n"
	//	"日本人にとって欠かせない食べ物だよね。",
	//	"なら、お米の生産が物凄く減った場合って考えたことはある？"},0, 1},
	//{{"そうなんだね。\n"
	//	"君はお金を使ってでも食べたいくらいパンが大好きなんだね。"	}, 1, 0},
	//{{"そうなんだね。\n"
	//	"やっぱり物価が高騰した状態では購入するのは厳しいよね。"}, 1, 1},
	//{{"なるほど！\n"
	//	"素晴らしい！君は日本人の鑑だ！！",
	//	"君はお金を使ってでもご飯を食べたいんだね！",}, 2, 0},
	//{{"そうなんだ。\n"
	//	"ご飯を選んでも物価の高騰で値段が上がると選びにくいよね。"}, 2, 1},
	//};


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
		if (state_ == SceneState::RESULT)
		{
			ResultScene::GetInstance().SetResultState(ResultState::TAIL);
			ResultScene::GetInstance().SetResultState(ResultState::DETAIL);
			// 無視して何もしない
		}
		else if (state_ != SceneState::PAUSE)
		{
			stateBeforePause_ = state_;
			state_ = SceneState::PAUSE;

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
			ResultScene::GetInstance().AddResult(result);

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
					ResultScene::GetInstance().Transition();
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
					ResultScene::GetInstance().Transition();
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
		ResultScene::GetInstance().Update();
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
		PauseScene::GetInstance().GameUpdate();
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

	//if (state_ == SceneState::RESULT)
	//{
	//	if (!resultBgImages_.empty())
	//	{
	//		bgToDraw = resultBgImages_[currentBgIndex_];
	//	}
	//}

	DrawGraph(0, 0, bgToDraw, FALSE);


	// 吹き出しの描画
	DrawBox(145, 45, 1750, 300, GetColor(255, 255, 255), true);   // 白い吹き出し背景
	DrawBox(150, 50, 1745, 295, GetColor(0, 0, 0), true);        // 黒い枠線

	// 吹き出しのメッセージ描画 (RESULT/PAUSE以外、またはRESULTのTAIL状態でのみ描画)
	if (state_ != SceneState::RESULT)
	{
	//	ResultScene::GetInstance().SetResultState(ResultState::TAIL);
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
		PauseScene::GetInstance().GameDraw();
	}
	else if (state_ == SceneState::RESULT)
	{
		ResultScene::GetInstance().Draw();
	}
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
		ResultScene::GetInstance().Transition();
	}
}

GameScene& GameScene::GetInstance(void)
{
	// C++におけるシングルトンの典型的な実装
	static GameScene instance;
	return instance;
}
