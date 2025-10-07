#include "QuestionManager.h"

QuestionManager::QuestionManager()
{
}

QuestionManager::~QuestionManager()
{
}

void QuestionManager::Init()
{
}

void QuestionManager::Update()
{
}

void QuestionManager::Draw()
{
	if (questions_.empty()) return;

    const auto& q = questions_[0];
	// 質問文表示
    DrawFormatString(100, 100, GetColor(255, 255, 255), "%s", q.questionText.c_str());

    // 合計を算出
    int total = 0;
    for (int c : q.choiceCounts) total += c;

    // 選択肢と集計表示
    for (int i = 0; i < (int)q.choices.size(); i++) {
        int y = 150 + i * 40;
        DrawFormatString(120, y, GetColor(255, 255, 255), "%s", q.choices[i].c_str());

        // 集計数表示（％）
        float percent = (total > 0) ? (q.choiceCounts[i] * 100.0f / total) : 0.0f;
        DrawFormatString(400, y, GetColor(170, 170, 170), "%.1f%%", percent);
    }
}

void QuestionManager::SelectChoice(int questionIndex, int choiceIndex)
{
	// 範囲チェック
    if (questionIndex < 0 || questionIndex >= questions_.size()) return;
	// 選択肢の集計を増やす
    auto& q = questions_[questionIndex];
    if (choiceIndex >= 0 && choiceIndex < q.choiceCounts.size()) {
        q.choiceCounts[choiceIndex]++;
    }

    SaveData(); // 選択後に即保存
}

void QuestionManager::LoadData()
{
	// ファイルから集計データを読み込む
    FILE* fp = nullptr;
    if (fopen_s(&fp, "SurveyData.txt", "r") != 0 || !fp) return;

	// 1件ずつ読み込む
    for (auto& q : questions_) {
        for (int i = 0; i < (int)q.choiceCounts.size(); i++) {
            fscanf_s(fp, "%d", &q.choiceCounts[i]);
        }
    }

    fclose(fp);
}

void QuestionManager::SaveData()
{
	// ファイルに集計データを書き込む
    FILE* fp = nullptr;
    if (fopen_s(&fp, "SurveyData.txt", "w") != 0 || !fp) return;

	// 1件ずつ書き込む
    for (auto& q : questions_) {
        for (int count : q.choiceCounts) {
            fprintf(fp, "%d ", count);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

