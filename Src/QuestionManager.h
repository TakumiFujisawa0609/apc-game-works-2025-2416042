#pragma once
#include <DxLib.h>
#include <vector>
#include <string>
class QuestionManager
{
	// 質問1件分のデータ
	struct QuestionData {
		std::string questionText;              // 質問文
		std::vector<std::string> choices;      // 選択肢一覧
		std::vector<int> choiceCounts;         // 各選択肢の選ばれた回数（集計用）
	};

public:
	QuestionManager();
	~QuestionManager();

	void Init();
	void Update();
	void Draw();
	void SelectChoice(int questionIndex, int choiceIndex);

	void LoadData();
	void SaveData();

private:
	std::vector<QuestionData> questions_;
};

