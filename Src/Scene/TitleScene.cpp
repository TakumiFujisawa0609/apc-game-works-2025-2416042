#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
{
	imgTitle_ = -1;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	imgTitle_ = LoadGraph((Application::PATH_IMAGE + "Title1.png").c_str());
}

void TitleScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();

	// 全てのキーのうち、どれかを押したら画面遷移
	for (int key = 0; key < 256; key++) {
		if (ins.IsTrgDown(key)) {
			SceneManager::GetInstance().ChangeScene(
				SceneManager::SCENE_ID::GAME);
			break; // 一つでも押されたら遷移
		}
	}
}

void TitleScene::Draw(void)
{

	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 2,
		1.0f, 0.0, imgTitle_, true);

}

void TitleScene::Release(void)
{
	DeleteGraph(imgTitle_);
}
