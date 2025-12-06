#include "view/ScreenManager.h"

namespace paradox::view {

void ScreenManager::SetActiveScreen(Screen screen) {
    current_screen_ = screen;
    // TODO: Load corresponding RmlUi document
}

} // namespace paradox::view
