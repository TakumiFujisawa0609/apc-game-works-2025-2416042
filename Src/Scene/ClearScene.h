#pragma once
#include "SceneBase.h"

class SceneManager;

class ClearScene : public SceneBase
{
public:
#pragma region 前方宣言
	// コンストラクタ
	ClearScene(void);

	// デストラクタ
	~ClearScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
#pragma region 変数宣言
	// ゲームクリア画像
	int imgGameClear_;

};

