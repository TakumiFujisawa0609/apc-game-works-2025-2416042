#pragma once
#include <DxLib.h>
#include "../Manager/InputManager.h"

class Pause
{
public:
    enum class Menu {
        RESUME = 0,
        TITLE,
        EXIT,
        COUNT
    };

    Pause();
    ~Pause() = default;

    void Init();
    bool Update();                 // ‘I‘ğŠm’è ¨ true ‚ğ•Ô‚·
    void Draw();
    Menu GetResult() const { return result_; }

private:
    InputManager& inputManager_;
    int select_;
    Menu result_;
};


