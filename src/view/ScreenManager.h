#pragma once

#include <string>

namespace paradox::view {

/**
 * @brief Manages screen transitions (main game, prestige, settings, etc.)
 */
class ScreenManager {
public:
    enum class Screen {
        MainGame,
        TimelineView,
        PrestigeScreen,
        Settings
    };

    void SetActiveScreen(Screen screen);
    [[nodiscard]] Screen GetActiveScreen() const { return current_screen_; }

private:
    Screen current_screen_ = Screen::MainGame;
};

} // namespace paradox::view
