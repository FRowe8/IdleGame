#include "bridge/CommandRouter.h"
#include <stdexcept>

namespace paradox::bridge {

void CommandRouter::RegisterCommand(const std::string& command_name, CommandCallback callback) {
    commands_[command_name] = std::move(callback);
}

void CommandRouter::Execute(const std::string& command_name, const std::string& arg) {
    auto it = commands_.find(command_name);
    if (it == commands_.end()) {
        throw std::runtime_error("Unknown command: " + command_name);
    }
    it->second(arg);
}

} // namespace paradox::bridge
