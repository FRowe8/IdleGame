#include "bridge/UpdateThrottle.h"

namespace paradox::bridge {

UpdateThrottle::UpdateThrottle(float interval_seconds)
    : interval_seconds_(interval_seconds)
{
}

void UpdateThrottle::SetCallback(Callback callback) {
    callback_ = std::move(callback);
}

void UpdateThrottle::Update(float delta_time) {
    accumulated_time_ += delta_time;

    if (accumulated_time_ >= interval_seconds_) {
        if (callback_) {
            callback_();
        }
        accumulated_time_ = 0.0f;
    }
}

void UpdateThrottle::ForceUpdate() {
    if (callback_) {
        callback_();
    }
    accumulated_time_ = 0.0f;
}

void UpdateThrottle::SetInterval(float interval_seconds) {
    interval_seconds_ = interval_seconds;
    accumulated_time_ = 0.0f;
}

} // namespace paradox::bridge
