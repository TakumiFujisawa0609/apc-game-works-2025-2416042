#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "ClearScene.h"

ClearScene::ClearScene(void)
{
	imgGameClear_ = -1;
}

ClearScene::~ClearScene(void)
{
}

void ClearScene::Init(void)
{
	imgGameClear_ = LoadGraph((Application::PATH_IMAGE + "a.png").c_str());
}

void ClearScene::Update(void)
{
	// ƒV[ƒ“‘JˆÚ
	InputManager& ins = InputManager::GetInstance();

	// ‘S‚Ä‚ÌƒL[‚Ì‚¤‚¿A‚Ç‚ê‚©‚ğ‰Ÿ‚µ‚½‚ç‰æ–Ê‘JˆÚ
	for (int key = 0; key < 256; key++) {
		if (ins.IsTrgDown(key)) {
			SceneManager::GetInstance().ChangeScene(
				SceneManager::SCENE_ID::TITLE);
			break; // ˆê‚Â‚Å‚à‰Ÿ‚³‚ê‚½‚ç‘JˆÚ
		}
	}
}

void ClearScene::Draw(void)
{
	// ”wŒi‰æ‘œ‚Ì•`‰æ
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 2,
		1.0f, 0.0, imgGameClear_, true);
}

void ClearScene::Release(void)
{
	// ‰æ‘œ‚Ì‰ğ•ú
	DeleteGraph(imgGameClear_);
}
