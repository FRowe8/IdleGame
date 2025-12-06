#include "bridge/DataBindingRegistry.h"
#include <stdexcept>

namespace paradox::bridge {

void DataBindingRegistry::Bind(const std::string& key, ValueGetter getter) {
    bindings_[key] = std::move(getter);
}

std::string DataBindingRegistry::Get(const std::string& key) const {
    auto it = bindings_.find(key);
    if (it == bindings_.end()) {
        throw std::runtime_error("No binding for key: " + key);
    }
    return it->second();
}

} // namespace paradox::bridge
