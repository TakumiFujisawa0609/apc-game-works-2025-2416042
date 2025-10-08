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

    // ---------------------------
   // 選択肢＋割合を縦に描画
   // ---------------------------
    int startY = 200; // 開始位置
    int spacing = 50; // 行間

    for (size_t i = 0; i < q.choices.size(); i++)
    {
        const std::string& choiceText = q.choices[i];
        int count = q.choiceCounts[i];

        float percent = (total > 0) ? (static_cast<float>(count) / total * 100.0f) : 0.0f;

        int color = GetColor(255, 255, 255);
        int percentColor = GetColor(255, 255, 0);

        // 選択肢テキスト
        DrawFormatString(120, startY + i * spacing, color, "%s", choiceText.c_str());

        // 割合（右側）
        DrawFormatString(800, startY + i * spacing, percentColor, "%.1f%%", percent);
    }

    // ---------------------------
    // 合計件数を右下に表示
    // ---------------------------
    DrawFormatString(100, startY + q.choices.size() * spacing + 30,
        GetColor(180, 180, 180), "Total: %d votes", total);

#if 0
    // 選択肢と集計表示
    for (int i = 0; i < (int)q.choices.size(); i++) {
        int y = 150 + i * 40;
        DrawFormatString(120, y, GetColor(255, 255, 255), "%s", q.choices[i].c_str());

        // 集計数表示（％）
        float percent = (total > 0) ? (q.choiceCounts[i] * 100.0f / total) : 0.0f;
        DrawFormatString(400, y, GetColor(170, 170, 170), "%.1f%%", percent);
    }
#endif
}

void QuestionManager::SelectChoice(int questionIndex, int choiceIndex)
{
    // 範囲チェック
    if (questionIndex < 0 || questionIndex >= questions_.size()) return;

    auto& q = questions_[questionIndex];
    if (choiceIndex < 0 || choiceIndex >= q.choiceCounts.size()) return;

    // 選択肢の集計を増やす
    q.choiceCounts[choiceIndex]++;

    // 選択後に即保存
    SaveData();
}

void QuestionManager::ResetData()
{
    for (auto& q : questions_) {
        std::fill(q.choiceCounts.begin(), q.choiceCounts.end(), 0);
    }

    SaveData(); // リセットした内容を保存
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


void QuestionManager::SetQuestions(const std::vector<QuestionData>& questions)
{
	questions_ = questions;
}

