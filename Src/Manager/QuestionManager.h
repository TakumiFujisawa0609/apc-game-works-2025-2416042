#pragma once
#include <DxLib.h>
#include <vector>
#include <string>

// 質問1件分のデータ
struct QuestionData {
	std::string questionText;              // 質問文
	std::vector<std::string> choices;      // 選択肢一覧
	std::vector<int> choiceCounts;         // 各選択肢の選ばれた回数（集計用）
};

class QuestionManager
{
public:
	QuestionManager();
	~QuestionManager();

	void Init();
	void Update();
	void Draw();
	void SelectChoice(int questionIndex, int choiceIndex);

	void ResetData(); // 集計データをリセット
	void LoadData();
	void SaveData();

	void SetQuestions(const std::vector<QuestionData>& questions);
	const std::vector<QuestionData>& GetQuestions() const { return questions_; }
private:
	std::vector<QuestionData> questions_;
};

