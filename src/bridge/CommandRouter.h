#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace paradox::bridge {

/**
 * @brief Routes UI interactions to Core commands
 *
 * Example:
 *   CommandRouter router;
 *   router.RegisterCommand("purchase_upgrade", [&](const std::string& arg) {
 *       game_state.PurchaseUpgrade(arg);
 *   });
 *
 *   // From RmlUi button click:
 *   router.Execute("purchase_upgrade", "time_multiplier_1");
 */
class CommandRouter {
public:
    using CommandCallback = std::function<void(const std::string& arg)>;

    void RegisterCommand(const std::string& command_name, CommandCallback callback);
    void Execute(const std::string& command_name, const std::string& arg = "");

private:
    std::unordered_map<std::string, CommandCallback> commands_;
};

} // namespace paradox::bridge
