#include "view/DataModelBinder.h"
#include "bridge/DataBindingRegistry.h"
#include "core/GameState.h"
#include <RmlUi/Core.h>

namespace paradox::view {

void DataModelBinder::Initialize(Rml::Context* context, core::GameState* game_state,
                                  bridge::DataBindingRegistry* registry)
{
    context_ = context;
    game_state_ = game_state;
    registry_ = registry;

    if (!context_) {
        return;
    }

    // Create RmlUi data model
    Rml::DataModelConstructor constructor = context->CreateDataModel("game_data");
    if (!constructor) {
        return;
    }

    // Register data model variables
    constructor.Bind("credits", &credits_text_);
    constructor.Bind("credits_per_second", &credits_per_sec_text_);
    constructor.Bind("time_bank_balance", &time_bank_text_);
    constructor.Bind("paradox_value", &paradox_text_);
    constructor.Bind("paradox_percentage", &paradox_percentage_);

    // Get the model handle for updates
    data_model_ = constructor.GetModelHandle();
}

void DataModelBinder::UpdateBindings() {
    if (!game_state_ || !data_model_) {
        return;
    }

    // Update credit display
    auto credits = game_state_->GetResource("credits");
    credits_text_ = credits.ToHumanReadable();

    // TODO: Calculate credits per second
    credits_per_sec_text_ = "0";

    // TODO: Get time bank balance
    time_bank_text_ = "0";

    // TODO: Get paradox instability
    paradox_text_ = "0%";
    paradox_percentage_ = 0.0f;

    // Notify RmlUi that data has changed
    data_model_.DirtyVariable("credits");
    data_model_.DirtyVariable("credits_per_second");
    data_model_.DirtyVariable("time_bank_balance");
    data_model_.DirtyVariable("paradox_value");
    data_model_.DirtyVariable("paradox_percentage");
}

} // namespace paradox::view
