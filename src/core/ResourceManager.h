#pragma once

#include "core/BigNumber.h"
#include <string>

namespace paradox::core {

/**
 * @brief Manages resource generation and consumption
 */
class ResourceManager {
public:
    ResourceManager() = default;

    void Update(float delta_time);

private:
    BigNumber credits_per_second_{1};
};

} // namespace paradox::core
