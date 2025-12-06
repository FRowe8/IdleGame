#pragma once

#include <RmlUi/Core/DataModelHandle.h>
#include <string>

// Forward declarations
namespace Rml { class Context; }
namespace paradox::core { class GameState; }
namespace paradox::bridge { class DataBindingRegistry; }

namespace paradox::view {

/**
 * @brief Binds Core data to RmlUi data models
 */
class DataModelBinder {
public:
    void Initialize(Rml::Context* context, core::GameState* game_state,
                    bridge::DataBindingRegistry* registry);
    void UpdateBindings();

private:
    Rml::Context* context_ = nullptr;
    core::GameState* game_state_ = nullptr;
    bridge::DataBindingRegistry* registry_ = nullptr;
    Rml::DataModelHandle data_model_;

    // Data model variables (bound to RmlUi)
    Rml::String credits_text_;
    Rml::String credits_per_sec_text_;
    Rml::String time_bank_text_;
    Rml::String paradox_text_;
    float paradox_percentage_ = 0.0f;
};

} // namespace paradox::view
