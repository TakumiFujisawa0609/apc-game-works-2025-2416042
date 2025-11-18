#include <DxLib.h>
#include <vector>
#include "SceneBase.h"
#include "../Manager/InputManager.h"
#include "GameScene.h"

class PauseScene : public SceneBase
{
public:
	// コンストラクタ
	PauseScene(void);
	// デストラクタ
	~PauseScene(void);
	// 初期化処理
	void Init(void) override;
	// 更新ステップ
	void Update(void) override;
	void GameUpdate(void);
	void TitleUpdate(void);
	void ClearUpdate(void);
	// 描画処理
	 void Draw(void) override;
	 void GameDraw(void);
	 void TitleDraw(void);
	 void ClearDraw(void);
	// 解放処理
	 void Release(void) override;

	 // インスタンスの取得
	 static PauseScene& GetInstance(void);

private:


	// 入力制御オブジェクト
	InputManager& inputManager_;
	// ポーズ前の状態を記録
	SceneState stateBeforePause_;
	// 選択肢の描画矩形リスト
	std::vector<ChoiceRect> choiceRects_;

	// ポーズメニューの選択肢インデックス
	int pauseSelectIndex_;

	// マウス左ボタンの押下状態
	bool isLButtonDown_;
};