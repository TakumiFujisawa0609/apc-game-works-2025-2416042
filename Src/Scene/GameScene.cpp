#include <DxLib.h>
#include <string>
#include <vector>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include"../Manager/QuestionManager.h"
#include "../Object/Message.h"
#include "../Scene/PauseScene.h"
#include "GameScene.h"

#pragma region メモ
// 家のディレクトリ
// "C:\DxLib\プロジェクトに追加すべきファイル_VC用"
// 学校のディレクトリ
// $(DXLIB_DIR)

// 今後やること
// ポーズシーンの中を充実させる
// ポーズシーンのボタンをゲームシーンにも導入させる
// タイトルシーンのボタンや選択肢のボックスにアニメーションをつける
#pragma endregion


GameScene::GameScene(void)
	:inputManager_(InputManager::GetInstance()),
	questionManager_(QuestionManager::GetInstance()),
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
	resultDisplayed_(false),
	resultType_(0),
	resultState_(ResultState::LIST),
	resultSelectIndex_(0),
	pauseDownPressed_(false),
	justEnteredList_(true),
	resultTailIndex_(0),
	talkMessage_(""),
	prevMouseLeft_(false),
	isLButtonDown_(false),
	currentBgIndex_(0),
	answeredCount_(0),
	detailIndex_(-1),
	skipListInput_(false),
	characterAlpha_(0),
	characterVisible_(false)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// 背景画像の読み込み
	gImage_ = LoadGraph("Data/Image/haikei.png");

	// キャラクター画像の読み込み
	characterImage_ = LoadGraph("Data/Image/Character.png");

	// --- リザルト背景群の読み込み ---
	resultBgImages_.clear();
	const int maxBgCount = 10; // 将来的に10枚くらい用意してもOK
	for (int i = 0; i < maxBgCount; ++i)
	{
		std::string path = "Data/Image/Result/Title2" + std::to_string(i) + ".png";
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
		"やぁ、目が覚めた？",
		/*"ここは精神と物質の狭間にある場所。\n"
		"君の心象世界だと言えばわかりやすいかな。",
		"実は君にお願いがあって、僕の実験を手伝ってもらいたいんだ。",
		"あ、難しく考えなくて大丈夫だよ。\n"
		"ただ、僕が出題する問いに答えてもらうだけだから。",
		"ちょっと複雑な問いはあるかもだけど、君なら大丈夫。",
		"それじゃあ、始める前に少し注意点を説明するね。",
		"まず、君が答えた問いは全て記録される。\n"
		"だから、君の価値観に沿って正直に答えてね。",
		"それと、君が答えた問いは後で確認できるから楽しみにしててね。",
		"それじゃあ、今から実験を始めるね。",*/
	};
	// 問いの内容
	questions_ =
{
    // region 1
    {
        "この先の人生で「新しい経験」と「安心感」、より重きを置くべきはどっち？",
        {
            {"新しい経験", 1, 282, 600, 0, LoadGraph("Data/Image/Choice/新しい経験.png")},
            {"安心感",     2, 1270, 600, 0, LoadGraph("Data/Image/Choice/安心感.png")}
        }
    },

    // region 2 - 新しい経験を選んだ場合
    {
        "もし全ての新しい経験がいばらの道で必ず後悔すると知っていた場合でも、\n"
        "君は「停滞」を避けるために新しい経験を選ぶ？",
        {
            {"選ぶ",     3, 422, 600, 0, LoadGraph("Data/Image/Choice/いばらの道.png")},
            {"選ばない", 3, 1230, 600, 0, LoadGraph("Data/Image/Choice/選ばない.png")}
        }
    },

    // region 2 - 安心感を選んだ場合
    {
        "安心感が永続的に保証された環境が、同時に全ての可能性を否定・排除する\n"
        "檻であった場合、君はそれを「幸福」と呼べる？",
        {
            {"呼べる",   3, 377, 600, 0, LoadGraph("Data/Image/Choice/呼べる.png")},
            {"呼べない", 3, 1230, 600, 0, LoadGraph("Data/Image/Choice/呼べない.png")}
        }
    },

    // region 3
    {
        "もし、過去の全ての失敗や後悔を「無かったこと」にできるボタンがあった\n"
        "として、君は「今の幸福な自分」を失うことを恐れず、そのボタンを押せる？",
        {
            {"押せる",   4, 377, 600, 0, LoadGraph("Data/Image/Choice/押せる.png")},
            {"押せない", 5, 1230, 600, 0, LoadGraph("Data/Image/Choice/押せない.png")}
        }
    },

    // region 4 - 押せる
    {
        "ボタンを押した結果、新しい人生を生きる君は「今の幸福な自分」を\n"
        "知らない大切な誰かに、「無かったことにした自分」の思い出を語れる？",
        {
            {"語れる",   6, 377, 600, 0, LoadGraph("Data/Image/Choice/語れる.png")},
            {"語れない", 6, 1230, 600, 0, LoadGraph("Data/Image/Choice/語れない.png")}
        }
    },

    // region 4 - 押せない
    {
        "もし君の大切な誰かが、君の過去の失敗によって今も苦しみ続けていたら、\n"
        "君はその「誰かの痛み」を見てなお、自分の幸福を優先する？",
        {
            {"自分を優先",   6, 282, 600, 0, LoadGraph("Data/Image/Choice/できる.png")},
            {"相手を優先", 6, 1180, 600, 0,LoadGraph("Data/Image/Choice/大切な人を優先.png")}
        }
    },

    // region 5
    {
        "完璧な計画と即座の行動、より成果を生み出すのはどちらだと思う？",
        {
            {"完璧な計画", 7, 282, 600, 0, LoadGraph("Data/Image/Choice/完璧な計画.png")},
            {"即座の行動", 8, 1180, 600, 0, LoadGraph("Data/Image/Choice/即座の行動.png")}
        }
    },

    // region 6 - 完璧な計画
    {
        "「完璧な計画」の完成を待つことで、それを実行に移すタイミングが\n"
        "永久に失われた場合、その計画は「失敗」と見なす？",
        {
            {"見なす",     9, 377, 600, 0, LoadGraph("Data/Image/Choice/見なす.png")},
            {"見なさない", 9, 1180, 600, 0, LoadGraph("Data/Image/Choice/見なさない.png")}
        }
    },

    // region 6 - 即座の行動
    {
        "即座に行動することで成果を得られる可能性が高い一方で、修正不可能な\n"
        "致命的なミスが、行動開始から1分以内に発生することが確実である場合、\n"
        "それでも、あなたは「即座の行動」を優先する？",
        {
            {"優先する",   9, 430, 600, 0, LoadGraph("Data/Image/Choice/優先する.png")},
            {"優先しない", 9, 1180, 600, 0, LoadGraph("Data/Image/Choice/優先しない.png")}
        }
    },

    // region 7
    {
        "もし君が無気力で夢がない場合、時間をかけて見つかるまで探す？\n"
        "それとも、外部からの強制に素直に従う？",
        {
            {"探す",         10, 422, 600, 0, LoadGraph("Data/Image/Choice/探す.png")},
            {"素直に従う",   11, 1180, 600, 0, LoadGraph("Data/Image/Choice/素直に従う.png")}
        }
    },

    // region 8 - 探す
    {
        "「探す時間」が無限に与えられた結果、何も見つからないまま人生の終わりに\n"
        "到達した場合、その探求は有意義であったと言えるかな？",
        {
            {"言える",   12, 377, 600, 0, LoadGraph("Data/Image/Choice/言える.png")},
            {"言えない", 12, 1230, 600, 0, LoadGraph("Data/Image/Choice/言えない.png")}
        }
    },

    // region 8 - 素直に従う
    { // ここから画像生成
        "言われるがままに従い続け、いつか君が『何者でもない空っぽな存在』に\n"
        "なったとき、君の人生に後悔はないのか？",
        {
            {"後悔はない", 12, 282, 600, 0, LoadGraph("Data/Image/Choice/後悔はない.png")},
            {"少し怖い", 12, 1230, 600, 0, LoadGraph("Data/Image/Choice/少し怖い.png")}
        }
    },

    // region 9
    {
        "もし自分の命日がわかってしまい、それが避けられない運命だった場合、\n"
        "君はその運命に抗って長生きをしようとする？\n"
        "それとも、おとなしく受け入れる？",
        {
            {"抗う",       13, 422, 600, 0, LoadGraph("Data/Image/Choice/抗う.png")},
            {"受け入れる", 14, 1180, 600, 0, LoadGraph("Data/Image/Choice/受け入れる.png")}
        }
    },

    // region 10 - 抗う
    {
        "運命に抗う行為が、結果的に運命の定めた日付よりも「早く」訪れた場合、\n"
        "その行動は正しかったと言える？",
        {
            {"言える",   -1, 377, 600, 0, LoadGraph("Data/Image/Choice/言える1.png")},
            {"言えない", -1, 1230, 600, 0, LoadGraph("Data/Image/Choice/言えない1.png")}
        }
    },

    // region 10 - 受け入れる
    {
        "おとなしく運命を受け入れるということは、君は自分の人生に一定の満足感を\n"
		"感じているから？\n"
		"それとも、抗うことが無意味だと悟ったから？",
        {
            {"満足感",   -1, 377, 600, 0, LoadGraph("Data/Image/Choice/満足感.png")},
            {"無意味", -1, 1270, 600, 0, LoadGraph("Data/Image/Choice/無意味.png")}
        }
    }
};


	// 解答後の会話
	afterTalks_ = {
#pragma region 1
		// 新しい経験を選んだ場合
	{{"なるほど、君は変化を恐れないチャレンジャーなんだね。",
		"現状維持は衰退だと考える、その強い意志は尊敬できるよ！",
		"なら、その新しい経験が、「後悔」を伴うと確実な場合でも、\n"
		"君は立ち止まらないのかな？"},0, 0},
		// 安心感を選んだ場合
	{{"なるほど、君は安定こそが最上の幸福だと考えているんだね。",
		"地に足をつけた堅実な生き方だ。多くの人が望む道だろうね。",
		"じゃあ、その安心感が、「全ての可能性を奪う檻」のようなものだとしたら、\n"
		"どうだろう？"},0, 1},

#pragma region 2
		// 選ぶを選んだ場合
	{{"君はこの選択を選ぶんだね。\n"
		"君は後悔を恐れるよりも、停滞を何よりも恐れるということか...",}, 1, 0},
		// 選ばないを選んだ場合
	{{"そうなんだね。\n"
		"君は確実な後悔を予期してまで飛び込むのは賢明ではないと判断したんだね。",
		"慎重さもまた、人生を豊かにする要素だね。"}, 1, 1},
		// 呼べるを選んだ場合
	{{"なるほど。\n"
		"「自由」よりも「平穏」を選んだんだね。",
		"君は全ての可能性を閉ざしても、不安のない日々こそが\n"
		"真の幸福だと思ってるんだね！"}, 2, 0},
		// 呼べないを選んだ場合
	{{"そうなんだ。\n"
		"君は安心感のためであっても、可能性が閉ざされるのは耐えられないんだね。",
		"人間の探求心は、決して満たされないということか..."}, 2, 1},
#pragma region 3
		// 押せるを選んだ場合
		{{"なるほど。君はボタンを押すんだね。",
			"つまり、君は「現在の幸福」も「過去の失敗の代償」だと\n"
			"捉えてるってことなんだね。",
			"なら、もし新しくなった自分に大切な誰かができたとき、その人たちに向けて\n"
			"「無かったことにした自分」を語ることができるのかな？"},3, 0},
		// 押せないを選んだ場合
	{{"君はそのボタンを押さないんだね。",
		"つまり君は「今の自分」という存在を、過去の全ての「失敗や痛み」を含めて\n"
		"「今の幸福な自分」だと見なしてるということか。",
		"なら、もし君の大切な誰かが、君の過去の失敗によって苦しみ続けていたら\n"
		"それでも自分を優先できるのかな？"},3, 1},

	#pragma region 4
		// 語れるを選んだ場合 
	{{"なるほど。",
		"君は、失ったものがあっても言葉にできると判断したんだね。",
		"過去を語れると思えることも、一つの強さだね。"}, 4, 0},
		//語れないを選んだ場合
	{{"なるほど。",
		"君は、語れない記憶を抱えたまま進む道を選んだんだね。",
		"無理に言葉にしない選択も、君なりの誠実さだね。"}, 4, 1},
		// できるを選んだ場合
	{{"そうなんだね。",
		"君は、他人の痛みがあっても自分の幸福うぃ選ぶと決めたんだね。",
		"それもまた、生き方の一つだね。"}, 5, 0},
		// できないを選んだ場合
	{{"そうなんだね。",
		"君は、誰かの痛みを見過ごせないと感じたんだね。",
		"その感覚を大切にする生き方もあるね。"}, 5, 1},

#pragma region 5
		// 完璧な計画を選んだ場合
	{{"準備こそが成功の鍵。君は石橋を叩いて渡るタイプなんだね。",
		"でも、その完璧な計画の「完成を待つ」ことで、実行のタイミングを永遠に\n"
		"失った場合、君はどう思う？"},6, 0},
		// 即座の行動を選んだ場合
	{{"スピードを重視する、決断力のあるタイプなんだね！",
		"でも、「修正不可能な致命的なミス」が1分以内に起こる確実な場合、\n"
		"それでも君は即座に行動を優先する？"},6, 1},

	#pragma region 6
		// 失敗と見なすを選んだ場合
	{{"君は失敗と見なすんだね。",
		"実行されない計画は、存在しないに等しい。君の厳格な判断だ。"}, 7, 0},
		// 失敗と見なさないを選んだ場合
	{{"君は失敗と見なさないんだね。",
		"つまり、計画自体は完璧だけど、失敗した原因は実行のタイミングだけ、\n"
		"ということだね。"}, 7, 1},
		// 優先するを選んだ場合
	{{"君にとっての「成果」は、自分自身の破滅よりも価値があるんだね。",
		"1分後に終わると知っていて、それでも君の歩みは引き返さないと\n"
		"決めたんだね。"}, 8, 0},
		// 優先しないを選んだ場合
	{{"なるほどね。",
		"君は、不確実な成果を得ることよりも、致命的な失敗を避ける道を\n"
		"選んだんだね。",}, 8, 1},

#pragma region 7
		// 内部からの探求を選んだ場合
	{{"夢がない時こそ、立ち止まって自分と向き合う、それが君の流儀なんだね。",
		"なら、探す時間が無限に与えられ、何も見つからないまま「人生の終わり」が\n"
		"来たら、その探求は有意義だったと言えるかな？"},9, 0},
		// 外部からの強制に素直に従うを選んだ場合
	{{"君は決断を他者に委ねる、もしくは、決断をする意思がないと言うことかな？",
		"確かに、決断をするには悩みや葛藤が発生するから煩わしい気持ちは\n"
		"理解できる。",
		"なら、君は今後、他者からの意見に素直に従い続け、いつしか君が\n"
		"「何者でもない空っぽな存在」になったとき、君は後悔しないのかな？"},9, 1},

#pragma region 8
		// 内部からの探求を選んだ場合
	{{"君は有意義であると言えるんだね。",
		"結果ではなく、探求のプロセスそのものに君は価値を見出したってことかな。"}, 10, 0},
		// 外部からの強制に素直に従うを選んだ場合
	{{"そっか。君は有意義ではないという意見なんだね。",
		"やはり、目標を持たない探求は、時間という資源の浪費ということだよね。"}, 10, 1},
		// 後悔はないを選んだ場合
	{{"なるほど...",
		"君は全てを強制させられ、空っぽの存在になっても、後悔はないということか。",
		"それぐらい強制された世の中で生きてきたのか、もしくは、周りの環境が\n",
		"君という人間を捻じ曲げられていたと考えるべきなのかな..."}, 11, 0},
		// 少し怖いを選んだ場合
	{{"そっか。素直に従う生き方をしても、君にはまだ恐怖心は\n"
		"存在しているんだね。",
		"その気持ちは残しておくべきだよ。他者からの言いなりなんてのは、\n"
		"自分という人間を見てるのではなく、使い捨ての道具や奴隷など、\n"
		"蔑んだ目で君を見ている証拠だからね。",
		"まだ、その恐怖心が残っているのであれば、自分なりの夢や道を探す\n"
		"その一歩を歩みだしてみようね！！"}, 11, 1},
#pragma region 9
		// 抗うを選んだ場合
	{{"運命に屈しない、強い意志だね。\n"
	    "生きることに、徹底的に執着するか...",
		"しかし、その「抵抗」が、皮肉にも運命の日より「早く」死を招いた行動の\n"
		 "場合、その行動は無謀であったと言えるだろうか？"},12, 0},
		// 受け入れるを選んだ場合
	{{"受け入れることの潔さ、もしくは諦めたってことかな。",
		"なら、受け入れて残された時間を「最大限に充実させる」努力は、\n"
		"運命への「抵抗」と呼べると思う？"},12, 1},

#pragma region 10
		// 言えるを選んだ場合
	{{"なるほど。",
		"運命に抗う行為が結果的に運命を予定より早めてしまったけど、\n"
		"君はそれでも正しい行動だったという結論なんだね。"}, 13, 0},
		// 言えないを選んだ場合
	{{"言えないを選択したんだね。",
		"君は、結果的に予定より早まった以上、その行動を正しいとは\n"
		"言い切れないと感じだんだね。",
		"つまり、意図と結果を切り分けて考えた判断をしたってことか。"}, 13, 1},
		// 満足感を選んだ場合
	{{"なるほど。",
		"君の人生はとても充実していて、楽しい日々を過ごしていたんだね。",
		"後悔のない素晴らしい人生を歩んでいた証拠だよ"}, 14, 0},
		// 無意味を選んだ場合
	{{"そっか...",
		"亡くなる運命には抗っても結果は変わらないと思ってるんだね。",
		"確かに抗ってもそれが絶対に報われる保障もないだろうから、\n"
		"君の選択も間違っていないよ。"}, 14, 1},
	};
	//"おとなしく運命を受け入れるということは、君は自分の人生に一定の満足感を\n"
	//	"感じているから？\n"
	//	"それとも、抗うことが無意味だと悟ったから？",
	//{
	//	{"満足感",   -1, 450, 760, 0, LoadGraph("Data/Image/Choice/満足感.png")},
	//	{"無意味", -1, 1310, 760, 0, LoadGraph("Data/Image/Choice/無意味.png")}
	//}
	resultTailMessages_ = {
		"お疲れ様！\n"
		"これで全ての質問が終わったよ！",
		"どうだった？正直に答えることはできたかな？",
		"最初に伝えておくけど、君の選択や全ての問い自体に答えは存在しないよ。",
		"この実験は正解・不正解を出すのではなく、君自身の価値観に向き合うこと。\n"
		"思考という行為を楽しんでもらうことを重きにおいた実験だったんだ。",
		"だから、君がどんな選択をしたとしても、それは君にとって正しい\n"
		"選択なんだよね。",
		"それに、君の選択によって君の価値観が変わることもあるし、\n"
		"君の価値観が君の選択を変えることもある。",
		"大切なのは、君が自分自身の価値観に向き合い、\n"
		"その価値観を理解し、受け入れることなんだ。",
		"それを踏まえた上で、君の解答結果を見てみよう。",
	};

	// メッセージの初期化
	storyIndex_ = 0;
	questionIndex_ = 0;
	selectedChoice_ = 0;
	state_ = SceneState::STORY;

	// メッセージオブジェクトの初期化
	msg_.Init();
	if (!story_.empty()) {
		msg_.SetMessage(story_[storyIndex_]);
	}
	else if (!questions_.empty()) {
		// 直接質問から始める場合（タイトルテキストが無いときの保険）
		msg_.SetMessage(questions_[questionIndex_].text);
	}
	else {
		msg_.SetMessage(""); // 最悪何も表示しない
	}

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
	if (inputManager_.IsTrgDown(KEY_INPUT_TAB) || inputManager_.IsTrgDown(KEY_INPUT_ESCAPE))
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

			pauseQuestionIndex_ = questionIndex_;
			pauseSelectedChoice_ = selectedChoice_;
			pauseMessage_ = questions_[questionIndex_].text;

			state_ = SceneState::PAUSE;

			if (stateBeforePause_ == SceneState::RESULT)
			{
				msg_.SetMessage("");
			}
		}
		
	}

// =========================
// デバッグ用：強制リスト画面へ
// =========================
#ifdef _DEBUG
	if (CheckHitKey(KEY_INPUT_F1))
	{
		state_ = SceneState::RESULT;
		resultState_ = ResultState::LIST; // ← もし分かれてるなら
		return;
	}
#endif

	switch (state_)
	{
#pragma region ストーリー
	case SceneState::STORY:
		StoryUpdate();
		break;
#pragma endregion

#pragma region 問題表示
	case SceneState::QUESTION:
		if (!msg_.IsFinished()) break;
		QuestionUpdate();
		break;
#pragma endregion

#pragma region 解答後の会話
	case SceneState::AFTER_TALK:
		if (!msg_.IsFinished()) break;
		AfterTalkUpdate();
		break;
#pragma endregion

#pragma region 最終結果表示
	case SceneState::RESULT:
		ResultUpdate();
		break;
#pragma endregion

#pragma region シーン終了
	case SceneState::END:
		EndUpdate();
		break;
#pragma endregion

#pragma region ポーズメニュー
	case SceneState::PAUSE:
		PauseUpdate();
		break;
	}
	// --- キャラクターフェード ---
	CharacterFade();

#pragma endregion
	// --- デバッグ用 ---
	// Rキーで問いデータリセット
	if (inputManager_.IsTrgDown(KEY_INPUT_BACK))
	{
		questionManager_.ResetData();
	}
}

void GameScene::StoryUpdate(void)
{
	// マウス座標とボタン状態を取得
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

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
}

void GameScene::QuestionUpdate(void)
{
	// マウス座標とボタン状態を取得
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

	//if (!msg_.IsFinished()); return;

	// --- キーボード操作（既存） ---
	if (inputManager_.IsTrgDown(KEY_INPUT_A))
	{
		selectedChoice_ = (selectedChoice_ == 0) ? (int)questions_[questionIndex_].choices.size() - 1 : selectedChoice_ - 1;
	}
	else if (inputManager_.IsTrgDown(KEY_INPUT_D))
	{
		selectedChoice_ = (selectedChoice_ == (int)questions_[questionIndex_].choices.size() - 1) ? 0 : selectedChoice_ + 1;
	}

	// マウス操作 
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
		currentResultIndex_ = static_cast<int>(results_.size()) - 1;

		// --- アフタートーク判定 ---
		afterTalkIndex_ = -1;
		for (int i = 0; i < static_cast<int>(afterTalks_.size()); i++)
		{
			// 該当するアフタートークがあるか確認
			if (afterTalks_[i].questionIndex == questionIndex_ &&
				afterTalks_[i].choiceIndex == selectedChoice_)
			{
				afterTalkIndex_ = i;
				break;
			}
		}
		// アフタートークがある場合
		if (afterTalkIndex_ >= 0)
		{
			state_ = SceneState::AFTER_TALK;
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
}

void GameScene::AfterTalkUpdate(void)
{
	// マウス座標とボタン状態を取得
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

	// --- 安全ガード（必須） ---
	if (afterTalkIndex_ < 0 || afterTalkIndex_ >= (int)afterTalks_.size())
	{
		return;
	}

	auto& lines = afterTalks_[afterTalkIndex_].lines;

	if (lines.empty())
	{
		return;
	}

	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
	{
		// 会話の次の行へ
		currentLineIndex_++;
		auto& lines = afterTalks_[afterTalkIndex_].lines;

		// 会話終了判定
		if (currentLineIndex_ >= static_cast<int>(lines.size()))
		{
			answeredCount_++;

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
}

void GameScene::ResultUpdate(void)
{
	// 最初の一度だけ実行
	if (!resultDisplayed_)
	{
		resultDisplayed_ = true;

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
		TailUpdate();
		break;
	case ResultState::LIST:
		ListUpdate();
		break;

	case ResultState::DETAIL:
		DetailUpdate();
		break;
	}
}

void GameScene::EndUpdate(void)
{
	// マウス座標とボタン状態を取得
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

	// スペースキーでクリアシーンへ
	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
	{
		if (!msg_.IsFinished()) msg_.Skip();
		else
		{
			//msg_.SetMessage("これで実験は終了だよ。遊んでくれてありがとう！");
			StopSoundMem(bgmHandle_);
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
		}
	}
}

void GameScene::PauseUpdate(void)
{
	PauseScene& pauseScene_ = PauseScene::GetInstance();
	pauseScene_.GameUpdate();

	// ポーズ解除
	if (pauseScene_.IsResume())
	{
		// 元の状態に戻す
		state_ = stateBeforePause_;

		// 問いの状態を復元
		if (state_ == SceneState::QUESTION)
		{
			questionIndex_ = pauseQuestionIndex_;
			selectedChoice_ = pauseSelectedChoice_;
			msg_.SetMessage(questions_[questionIndex_].text);
		}
	}
	pauseScene_.SetResume(false);
}

void GameScene::TailUpdate(void)
{
	// マウス座標とボタン状態を取得
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

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
			//msg_.SetMessage("結果はこちら");
		}
	}
}

void GameScene::ListUpdate(void)
{
	// マウス座標とボタン状態を取得
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

	// TAILで会話を終えたため、ここではメッセージを表示しない
	if (justEnteredList_) {
		justEnteredList_ = false;  // 一度だけ実行
	}



	int listSize = static_cast<int>(results_.size());
	int NEXT_INDEX = listSize;               // 「次へ進む」は最後のインデックス
	int ROW_ITEMS = 5;                       // 1段あたりの数
	int totalOptions = listSize + 1;         // +1 は「次へ進む」
	int oldIndex = resultSelectIndex_;

	// =========================
	// W：上の段へ移動
	// =========================
	if (inputManager_.IsTrgDown(KEY_INPUT_W))
	{
		// 一番上段にいる → 変化なし
		if (resultSelectIndex_ < ROW_ITEMS) {
			// ただし「次へ進む」(最下段扱い) が選ばれていた場合のみ特別処理
			if (resultSelectIndex_ == NEXT_INDEX) {
				// 下段の一番左へ
				resultSelectIndex_ = ROW_ITEMS;
			}
		}
		else {
			// 下段 → 上段へ
			resultSelectIndex_ -= ROW_ITEMS;
		}
	}

	// =========================
	// S：下の段へ移動
	// =========================
	if (inputManager_.IsTrgDown(KEY_INPUT_S))
	{
		if (resultSelectIndex_ < ROW_ITEMS) {
			// 上段 → 下段へ
			int next = resultSelectIndex_ + ROW_ITEMS;

			if (next >= listSize) {
				// 下段に行けない位置 → 「次へ進む」へ
				resultSelectIndex_ = NEXT_INDEX;
			}
			else {
				resultSelectIndex_ = next;
			}
		}
		else if (resultSelectIndex_ >= ROW_ITEMS && resultSelectIndex_ < listSize) {
			// 下段 → 「次へ進む」
			resultSelectIndex_ = NEXT_INDEX;
		}
		else if (resultSelectIndex_ == NEXT_INDEX) {
			// 「次へ進む」で S → 変化なし
		}
	}

	// =========================
	// A：左へ移動
	// =========================
	if (inputManager_.IsTrgDown(KEY_INPUT_A))
	{
		if (resultSelectIndex_ == NEXT_INDEX) {
			// 次へ進む → 下段の右端へ戻す
			resultSelectIndex_ = listSize - 1;
		}
		else if (resultSelectIndex_ > 0) {
			resultSelectIndex_--;
		}
	}

	// =========================
	// D：右へ移動
	// =========================
	if (inputManager_.IsTrgDown(KEY_INPUT_D))
	{
		if (resultSelectIndex_ == NEXT_INDEX) {
			// 「次へ進む」なら変化なし
		}
		else if (resultSelectIndex_ < listSize - 1) {
			resultSelectIndex_++;
		}
		else {
			// 下段の最後 → 次へ進む
			resultSelectIndex_ = NEXT_INDEX;
		}
	}


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
			// 選択インデックスが変更されたときのみ代入する
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

	// 決定ロジックの開始点としてラベルを定義
RESULT_DECISION:

	// Spaceキーで決定
	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
	{
		// カーソルが「次へ進む」の項目を指しているかチェック
		if (resultSelectIndex_ == (int)results_.size())
		{
			// 「次へ進む」が選択された場合、すぐに END シーンへ遷移
			//SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::END);
			state_ = SceneState::END; // GameSceneの状態も終了へ
			msg_.SetMessage("これで終了だよ。遊んでくれてありがとう！");
			return; // Update処理を終了
		}
		else
		{
			if (resultSelectIndex_ >= 0 && resultSelectIndex_ < (int)results_.size())
			{
				// 詳細表示へ
				resultState_ = ResultState::DETAIL;

				detailIndex_ = resultSelectIndex_;

				std::string detailMsg =
					"【問 " + std::to_string(resultSelectIndex_ + 1) + " 】\n\n" +
					results_[resultSelectIndex_].questionText + "\n\n" +
					"あなたの選択: " + results_[resultSelectIndex_].selectedChoiceText;

				msg_.SetMessage(detailMsg);
			}

			// アフタートーク未再生なら再生
			if (!results_[resultSelectIndex_].afterTalkDone)
			{
				afterTalkIndex_ = resultSelectIndex_; // ← 再生対象を記録
				results_[resultSelectIndex_].afterTalkDone = true;
			}
		}
		if (resultSelectIndex_ == listSize)
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

			}
		}
	}
}

void GameScene::DetailUpdate(void)
{
	// マウス座標とボタン状態を取得
	int mouseButton = GetMouseInput();
	bool isLButtonTrg = (mouseButton & MOUSE_INPUT_LEFT) && !isLButtonDown_;
	isLButtonDown_ = (mouseButton & MOUSE_INPUT_LEFT);

	// Spaceで一覧へ戻る
	if (inputManager_.IsTrgDown(KEY_INPUT_SPACE) || isLButtonTrg)
	{
		resultState_ = ResultState::LIST;
		msg_.SetMessage("");
		isLButtonDown_ = true;
		detailIndex_ = -1;
	}
}

bool GameScene::IsCharacterTargetState(void)
{
	return state_ == SceneState::STORY ||
			  resultState_ == ResultState::TAIL ||
			  state_ == SceneState::END;
}

void GameScene::CharacterFade(void)
{
	const int FADE_SPEED = 8; // 調整用

	// 今の state 的に「出したいか」
	bool targetVisible = IsCharacterTargetState();

	if (targetVisible)
	{
		// フェードイン
		characterAlpha_ += FADE_SPEED;
		if (characterAlpha_ > 255)
			characterAlpha_ = 255;
	}
	else
	{
		// フェードアウト
		characterAlpha_ -= FADE_SPEED;
		if (characterAlpha_ < 0)
			characterAlpha_ = 0;
	}
}

void GameScene::Draw(void)
{
	// --- 背景 ---
	int bgToDraw = gImage_;
	DrawGraph(0, 0, bgToDraw, FALSE);

	//// キャラクターの描画
	//if (
	//	state_ == SceneState::STORY ||
	//	resultState_ == ResultState::TAIL ||
	//	state_ == SceneState::END
	//	)
	//{
	//	DrawGraph(CHARACTER_IMAGE_X, CHARACTER_IMAGE_Y, characterImage_, TRUE);
	//}
	if (characterAlpha_ > 0)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, characterAlpha_);
		DrawGraph(CHARACTER_IMAGE_X, CHARACTER_IMAGE_Y, characterImage_, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 吹き出しの描画
	DrawBox(WHITE_LEFT, WHITE_TOP, 
		WHITE_RIGHT, WHITE_BOTTOM, 
		GetColor(255, 255, 255), true);   // 白い吹き出し背景
	DrawBox(BLACK_LEFT, BLACK_TOP, 
		BLACK_RIGHT, BLACK_BOTTOM,
		GetColor(0, 0, 0), true);        // 黒枠

	// 吹き出しのメッセージ描画 (RESULT/PAUSE以外、またはRESULTのTAIL状態でのみ描画)
	if (state_ != SceneState::RESULT || resultState_ == ResultState::TAIL)
	{
		msg_.Draw(35, 826);
	}

	switch (state_)
	{
	case SceneState::STORY:
		break;
	case SceneState::QUESTION:
		QuestionDraw();
		break;
	case SceneState::AFTER_TALK:
		AfterTalkDraw();
		break;
	case SceneState::RESULT:
		ResultDraw();
		break;
	case SceneState::END:
		EndDraw();
		break;
	case SceneState::PAUSE:
		PauseDraw();
		break;
	default:
		break;
	}

}

void GameScene::QuestionDraw(void)
{
	if (state_ == SceneState::QUESTION)
	{
		// 問いの選択肢の背景枠(左側)	DrawBox(左側面、上、右側面、下) 
		DrawBox(CHOICE_WHITE_LEFT, CHOICE_WHITE_TOP, 
			CHOICE_WHITE_RIGHT, CHOICE_WHITE_BOTTOM,
			GetColor(255, 255, 255), true);  // 白背景
		DrawBox(CHOICE_BLACK_LEFT, CHOICE_BLACK_TOP, 
			CHOICE_BLACK_RIGHT, CHOICE_BLACK_BOTTOM,
			GetColor(0, 0, 0), true);       // 黒枠線

		// 問いの選択肢の背景枠(右側)
		DrawBox(CHOICE2_WHITE_LEFT, CHOICE2_WHITE_TOP, 
			CHOICE2_WHITE_RIGHT, CHOICE2_WHITE_BOTTOM,
			GetColor(255, 255, 255), true);  // 白背景
		DrawBox(CHOICE2_BLACK_LEFT, CHOICE2_BLACK_TOP, 
			CHOICE2_BLACK_RIGHT, CHOICE2_BLACK_BOTTOM,
			GetColor(0, 0, 0), true);       // 黒枠線

		// 選択肢の描画
		DrawChoices(questions_[questionIndex_].choices, selectedChoice_, false);

	}
}

void GameScene::AfterTalkDraw(void)
{
	// 解答後の会話と結果表示
	if (state_ == SceneState::AFTER_TALK) {
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
			DrawBox(RESULT_WHITE_LEFT, RESULT_WHITE_TOP,
				RESULT_WHITE_RIGHT, RESULT_WHITE_BOTTOM,
				GetColor(255, 255, 255), true);
			DrawBox(RESULT_BLACK_LEFT, RESULT_BLACK_TOP,
				RESULT_BLACK_RIGHT, RESULT_BLACK_BOTTOM,
				GetColor(0, 0, 0), true);

			// 合計票数（manager 側）
			int total = 0;
			for (int v : questionData.choiceCounts) total += v;

			// 選択肢と割合の表示（manager 側の counts を使う）
			int y = 440; // 縦の開始位置
			for (size_t i = 0; i < question.choices.size(); i++) {
				const auto& cVisual = question.choices[i];

				// manager 側の count を安全に取得
				int count = 0;
				if (i < questionData.choiceCounts.size()) {
					count = questionData.choiceCounts[i];
				}
				// 割合計算
				float percent = (total > 0) ? (100.0f * count / (float)total) : 0.0f;

				bool isSelected = (i == talk.choiceIndex);

				int textColor = (i == talk.choiceIndex) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
				int barColor = (i == talk.choiceIndex) ? GetColor(255, 100, 100) : GetColor(100, 100, 255);
				int questionNo = answeredCount_ + 1;

				// 問題番号
				SetFontSize(50);
				DrawFormatString(
					25,		// X座標
					80,		// Y座標
					GetColor(255, 255, 255),
					"[問%d]",
					questionNo
				);

				// 問題文
				DrawString(
					25,			// X座標
					140,			//	Y座標
					question.text.c_str(),
					GetColor(255, 255, 255)
				);

			

				// 選択肢文字
				SetFontSize(80);
				DrawString(
					250,		// X座標
					y,			// Y座標
					cVisual.text.c_str(),
					textColor
				);

				// 棒グラフ表示
				DrawPercentageBar(
					690,		// X座標
					y + 6, // Y座標
					700,		// 幅
					75,		// 高さ
					percent,
					barColor
				);

				// 割合表示
				DrawFormatString(
					1450,
					y,
					textColor,
					"%.1f%%",
					percent,
					count
				);

				if (!results_.empty())
				{
					SetFontSize(50);
					const auto& r = results_.back();

					DrawString(
						25,
						290,
						"あなたの選択：",
						GetColor(255, 255, 255)
					);

					// ② 文字幅を取得して、その右に赤文字を描画
					int offsetX = GetDrawStringWidth("あなたの選択：", strlen("あなたの選択："));

					DrawString(
						25 + offsetX,
						290,
						r.selectedChoiceText.c_str(),
						GetColor(255, 0, 0)
					);
				}

				// 行間
				y += 100;
			}
		}
	}
}

void GameScene::ResultDraw(void)
{
	if (state_ == SceneState::RESULT)
	{
		switch (resultState_)
		{
		case ResultState::TAIL:
			TailDraw();
			break;

		case ResultState::LIST:
			ListDraw();
			break;
		case ResultState::DETAIL:
			DetailDraw();
			break;
		}
	}
}

void GameScene::EndDraw(void)
{
}

void GameScene::PauseDraw(void)
{
	if (state_ == SceneState::PAUSE)
	{
		PauseScene::GetInstance().GameDraw();
	}
}

void GameScene::TailDraw(void)
{
}

void GameScene::ListDraw(void)
{
	// LIST/DETAIL状態でのみ、結果一覧/詳細の大きな枠を描画
	DrawBox(LIST_WHITE_LEFT, LIST_WHITE_TOP,
		LIST_WHITE_RIGHT, LIST_WHITE_BOTTOM,
		GetColor(255, 255, 255), TRUE);
	DrawBox(LIST_BLACK_LEFT, LIST_BLACK_TOP,
		LIST_BLACK_RIGHT, LIST_BLACK_BOTTOM,
		GetColor(0, 0, 0), TRUE);

	SetFontSize(50);
	DrawFormatString(35, 820, GetColor(255, 255, 255),
		"結果はこちら。");
	DrawFormatString(35, 870, GetColor(255, 255, 255),
		"WASDで操作 or マウス操作、Spaceキー or クリックで詳細を見れます。");

	// マウス用矩形リストをクリア
	choiceRects_.clear();	

	// 回答リストの表示（横1列 × 2段）
	const int itemPerRow = 5;     // 1段に5個
	const int itemWidth = 300;    // 横幅の間隔（各アイテム間の距離）
	const int topRowY = 100;      // 上段のY座標
	const int bottomRowY = 300;   // 下段のY座標
	const int base2X = 270;        // 左端の開始位置

	SetFontSize(120);

	for (size_t i = 0; i < results_.size(); i++)
	{
		// どの段か判定
		int row = (i < itemPerRow) ? 0 : 1;

		// インデックスに応じた座標計算
		int colIndex = (int)(i % itemPerRow);  // 0〜4 の位置
		int x = base2X + colIndex * itemWidth;
		int y = (row == 0) ? topRowY : bottomRowY;

		int color = (i == resultSelectIndex_) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
		std::string line = "問"+std::to_string(i + 1);

		// ===== 背景 =====
		int rect_left = x - 30;
		int rect_top = y - 10;
		int rect_right = x + 210;
		int rect_bottom = y + 120;

		// 選択中の項目は背景色を変える
		if (i == resultSelectIndex_)
		{
			DrawBox(rect_left, rect_top, rect_right, rect_bottom, GetColor(50, 50, 50), TRUE);
		}

		// ===== 文字描画 =====
		DrawString(x, y, line.c_str(), color);
		if (i == resultSelectIndex_) DrawString(x - 50, y, ">", color);

		// ===== マウス判定矩形登録 =====
		choiceRects_.push_back({ rect_left, rect_top, rect_right, rect_bottom });
	}

	// 次へ進む（この部分は一切変更しない）
	SetFontSize(140);
	std::string nextText = "次へ進む";
	int nextY = 560;
	int nextX = 690;
	int nextWidth = GetDrawStringWidth(nextText.c_str(), (int)nextText.size());
	int nextColor = (resultSelectIndex_ == (int)results_.size()) ?
		GetColor(255, 255, 0) : GetColor(255, 255, 255);

	if (resultSelectIndex_ == (int)results_.size()) DrawString(690, nextY, ">", nextColor);

	int rect_left = nextX - 100;
	int rect_top = nextY;
	int rect_right = nextX + nextWidth + 40;
	int rect_bottom = nextY + 145;

	if (resultSelectIndex_ == (int)results_.size())
	{
		DrawBox(rect_left, rect_top, rect_right, rect_bottom, GetColor(50, 50, 50), TRUE);
	}

	DrawString(nextX, nextY, nextText.c_str(), nextColor);
	if (resultSelectIndex_ == (int)results_.size()) DrawString(620, nextY, ">", nextColor);

	choiceRects_.push_back({ rect_left, rect_top, rect_right, rect_bottom });

}

void GameScene::DetailDraw(void)
{
	// 背景 
	DrawBox(LIST_WHITE_LEFT, LIST_WHITE_TOP,
		LIST_WHITE_RIGHT, LIST_WHITE_BOTTOM,
		GetColor(255, 255, 255), TRUE);
	DrawBox(LIST_BLACK_LEFT, LIST_BLACK_TOP,
		LIST_BLACK_RIGHT, LIST_BLACK_BOTTOM,
		GetColor(0, 0, 0), TRUE);

	//// 対象データ取得 
	//if (resultSelectIndex_ < 0 || resultSelectIndex_ >= (int)results_.size())
	//	return;

		// ===== 表示対象チェック（最重要）=====
	if (detailIndex_ < 0 || detailIndex_ >= (int)results_.size())
		return;

	const auto& r = results_[resultSelectIndex_];

	if (r.questionIndex < 0 || r.questionIndex >= (int)questions_.size())
		return;

	const auto& question = questions_[r.questionIndex];
	const auto& managerQuestions = questionManager_.GetQuestions();
	if (r.questionIndex >= (int)managerQuestions.size())
		return;

	const auto& questionData = managerQuestions[r.questionIndex];

	//  合計票数
	int total = 0;
	for (int v : questionData.choiceCounts) total += v;

	// 問番号・問題文 
	SetFontSize(50);
	DrawFormatString(
		35, 75,
		GetColor(255, 255, 255),
		"[問%d]",
		resultSelectIndex_ + 1
	);
	// 問題文 
	DrawString(
		35, 130,
		question.text.c_str(),
		GetColor(255, 255, 255)
	);

	// あなたの選択
	SetFontSize(45);
	DrawFormatString(
		35,
		300,
		GetColor(255, 255, 255),
		"あなたの選択：",
		r.selectedChoiceText.c_str()
	);
	// あなたの選択の％
	SetFontSize(45);
	DrawFormatString(
		400,
		300,
		GetColor(255, 0, 0),
		"%s",
		r.selectedChoiceText.c_str()
	);

	// ====== 選択肢＋棒グラフ ======
	int y = 440;

	for (size_t i = 0; i < question.choices.size(); i++)
	{
		int count = (i < questionData.choiceCounts.size())
			? questionData.choiceCounts[i]
			: 0;

		float percent = (total > 0)
			? (100.0f * count / total)
			: 0.0f;

		bool isSelected = r.selectedChoiceText == question.choices[i].text;

		int textColor = isSelected
			? GetColor(255, 0, 0)
			: GetColor(255, 255, 255);

		int barColor = isSelected
			? GetColor(255, 80, 80)
			: GetColor(100, 100, 255);
		// 選択肢文字描画
		SetFontSize(80);
		DrawString(
			200, y,
			question.choices[i].text.c_str(),
			textColor
		);
		// 棒グラフ描画
		DrawPercentageBar(
			650, y + 6,
			700, 70,
			percent,
			barColor
		);
		// 割合表示
		DrawFormatString(
			1400, y,
			textColor,
			"%.1f%%",
			percent
		);

		y += 100;
	}

	// ====== 戻る案内 ======
	SetFontSize(50);
	DrawString(
		35, 820,
		"Space or クリックで一覧に戻る。",
		GetColor(255, 255, 255)
	);
}

void GameScene::CharacterDraw(void)
{
	if (characterImage_ == -1) return;

	DrawGraph(CHARACTER_IMAGE_X, CHARACTER_IMAGE_Y, characterImage_, TRUE);
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
	SetFontSize(90); // フォントサイズを再設定

	// マウスの当たり判定に必要なサイズを定義（既存の選択肢背景サイズに合わせる）
	int choiceHeight = 60;  // ※文字サイズ50を基準とした高さ。背景サイズに合わせて調整してください。

	// 選択肢の表示
	for (size_t i = 0; i < choices.size(); i++) {
		int choiceWidth = GetDrawStringWidth(choices[i].text.c_str(), (int)choices[i].text.size());
		int color = (i == cursorIndex) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

		// 選択肢のテキストの上に画像を表示する
		if (choices[i].imageHandle != -1) {
			int w, h;
			// 画像のサイズを取得
			GetGraphSize(choices[i].imageHandle, &w, &h);

			// 画像の描画座標を計算：
			// X: テキストの開始位置(choices[i].x) ＋ テキスト幅の半分 － 画像幅の半分 ＝ 中央揃え
			// Y: テキストの開始位置(choices[i].y) － 画像の高さ － 少しの余白(20)
			DrawGraph(choices[i].x + (choiceWidth / 2) - (w / 2),
				choices[i].y - h - 50,
				choices[i].imageHandle, TRUE);
		}

		DrawString(choices[i].x, choices[i].y, choices[i].text.c_str(), color);

		// 割合の表示
		if (showPercent && total > 0) {
			float percentage_ = (choices[i].count / (float)total) * 100.0f;
			char buf[64]{};
			DrawFormatString(choices[i].x, choices[i].x + 50, GetColor(255, 255, 0), "%d%%", percentage_);
		}
		// マウス当たり判定用の矩形を保存 (choices[i].x/yを利用)
		// ----------------------------------------------------
		ChoiceRect rect;
		if (i == 0) { // 左側の選択肢
			rect.left = CHOICE_WHITE_LEFT;   // 左の枠の左端
			rect.top = CHOICE_WHITE_TOP;    // 左の枠の上端
			rect.right = CHOICE_WHITE_RIGHT;  // 左の枠の右端
			rect.bottom = CHOICE_WHITE_BOTTOM; // 左の枠の下端
		}
		else {      // 右側の選択肢
			rect.left = CHOICE2_WHITE_LEFT;   // 右の枠の左端
			rect.top = CHOICE2_WHITE_TOP;    // 右の枠の上端
			rect.right = CHOICE2_WHITE_RIGHT;  // 右の枠の右端
			rect.bottom = CHOICE2_WHITE_BOTTOM; // 右の枠の下端
		}

		choiceRects_.push_back(rect);
		// ----------------------------------------------------
	}
}

void GameScene::DrawPercentageBar(int x, int y, int width, int height, float percent, int barColor)
{
	// 安全対策
	if (percent < 0.0f)  percent = 0.0f;
	if (percent > 100.0f) percent = 100.0f;

	// 割合 → 横幅
	int w = static_cast<int>(width * (percent / 100.0f));

	// 背景（空バー）
	DrawBox(x, y, x + width, y + height, GetColor(50, 50, 50), TRUE);

	// 中身（割合分）
	DrawBox(x, y, x + w, y + height, barColor,  TRUE);

	// 枠
	DrawBox(x, y, x + width, y + height, GetColor(255, 255, 255), FALSE);
}

void GameScene::Release(void)
{
	// 背景画像の解放
	if (gImage_ != -1) {
		DeleteGraph(gImage_);
		gImage_ = -1;
	}

	// キャラクター画像の解放
	if (characterImage_ != -1) {
		DeleteGraph(characterImage_);
		characterImage_ = -1;
	}


	// BGMを停止して削除
	if (bgmHandle_ != -1) {
		StopSoundMem(bgmHandle_);
		DeleteSoundMem(bgmHandle_);
		bgmHandle_ = -1;
	}

	// 質問の選択肢画像を削除
	for (auto& q : questions_) {
		for (auto& c : q.choices) {
			if (c.imageHandle != -1) {
				DeleteGraph(c.imageHandle);
				c.imageHandle = -1;
			}
		}
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

GameScene& GameScene::GetInstance(void)
{
	// C++におけるシングルトンの典型的な実装
	static GameScene instance;
	return instance;
}
