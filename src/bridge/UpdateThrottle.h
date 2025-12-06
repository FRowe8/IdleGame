#pragma once

#include <functional>
#include <chrono>

namespace paradox::bridge {

/**
 * @brief Throttles UI updates to conserve mobile battery
 *
 * PRODUCTION CONSTRAINT: UI Throttling
 * =====================================
 * Core logic ticks at 60 Hz (every ~16ms)
 * RmlUi data models should NOT update every tick (expensive string formatting)
 *
 * Solution: Poll Timer
 * - Only update UI strings every 100-200ms
 * - Core still runs at full speed
 * - Saves battery on mobile devices
 *
 * Example:
 *   UpdateThrottle throttle(0.15f);  // 150ms between UI updates
 *
 *   throttle.SetCallback([&]() {
 *       ui_label.SetText(big_number.ToHumanReadable());  // Expensive string conversion
 *   });
 *
 *   // In main loop (called every frame):
 *   throttle.Update(frame_delta_time);
 *
 * Visual Timeline:
 *   Core Ticks:  | | | | | | | | | | | | | | | |  (60 Hz)
 *   UI Updates:  |           |           |        (6.67 Hz @ 150ms)
 */
class UpdateThrottle {
public:
    using Callback = std::function<void()>;

    /**
     * @brief Construct throttle with desired interval
     * @param interval_seconds Time between callback invocations (e.g., 0.15f = 150ms)
     */
    explicit UpdateThrottle(float interval_seconds = 0.15f);

    /**
     * @brief Set the callback to invoke when interval elapses
     */
    void SetCallback(Callback callback);

    /**
     * @brief Update internal timer and invoke callback if interval elapsed
     * @param delta_time Frame time in seconds
     */
    void Update(float delta_time);

    /**
     * @brief Force immediate callback invocation (resets timer)
     */
    void ForceUpdate();

    /**
     * @brief Change the throttle interval
     */
    void SetInterval(float interval_seconds);

    /**
     * @brief Get current interval setting
     */
    [[nodiscard]] float GetInterval() const { return interval_seconds_; }

private:
    float interval_seconds_;
    float accumulated_time_ = 0.0f;
    Callback callback_;
};

} // namespace paradox::bridge
