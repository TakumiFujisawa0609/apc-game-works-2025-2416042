#pragma once
#include <string>
#include <vector>
#include "SceneBase.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h" // SceneManagerへのアクセスが必要

// 選択肢の描画矩形を保存するための構造体 (GameScene.hから移動)
struct ChoiceRect {
    int left, top, right, bottom;
};

// GameSceneの状態を保存するためのenum（GameScene.hからコピー）
enum class SceneState
{
    STORY,
    QUESTION,
    ANSWER_TALK,
    RESULT,
    END,
    // PAUSEは不要
};

class PauseScene : public SceneBase
{
public:
    // コンストラクタ
    PauseScene(SceneState stateBeforePause);
    // デストラクタ
    ~PauseScene(void);

    // 初期化処理
    void Init(void) override;
    // 更新ステップ
    void Update(void) override;
    // 描画処理
    void Draw(void) override;
    // 解放処理
    void Release(void) override;

private:
    // 入力制御オブジェクト
    InputManager& inputManager_;

    // ポーズに入る前の状態を記録 (GameSceneから移動)
    SceneState stateBeforePause_;

    // ポーズメニューの選択肢インデックス (GameSceneから移動)
    int pauseSelectIndex_;

    // 選択肢の描画矩形リスト (GameSceneから移動)
    std::vector<ChoiceRect> choiceRects_;

    // マウス左ボタンの押下状態 (GameSceneから移動、またはInputManagerに頼る)
    bool isLButtonDown_;
};