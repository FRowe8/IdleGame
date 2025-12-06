#pragma once

#include <string>
#include <functional>
#include <unordered_map>

namespace paradox::bridge {

/**
 * @brief Maps Core data to RmlUi data model keys
 *
 * Example:
 *   DataBindingRegistry registry;
 *
 *   registry.Bind("credits", [&]() -> std::string {
 *       return game_state.GetResource("credits").ToHumanReadable();
 *   });
 *
 *   // Later, in UI update loop:
 *   std::string credits_text = registry.Get("credits");
 *   rmlui_model.SetValue("credits", credits_text);
 */
class DataBindingRegistry {
public:
    using ValueGetter = std::function<std::string()>;

    void Bind(const std::string& key, ValueGetter getter);
    [[nodiscard]] std::string Get(const std::string& key) const;

private:
    std::unordered_map<std::string, ValueGetter> bindings_;
};

} // namespace paradox::bridge
