#include "core/TickSystem.h"
#include <algorithm>

namespace paradox::core {

TickSystem::TickSystem(uint32_t target_ticks_per_second)
    : target_ticks_per_second_(target_ticks_per_second)
    , tick_duration_(1.0f / static_cast<float>(target_ticks_per_second))
    , last_update_time_(std::chrono::system_clock::now())
{
}

void TickSystem::SetTickCallback(TickCallback callback) {
    tick_callback_ = std::move(callback);
}

void TickSystem::SetProgressCallback(ProgressCallback callback) {
    progress_callback_ = std::move(callback);
}

void TickSystem::SetMaxTicksPerFrame(uint32_t max_ticks) {
    max_ticks_per_frame_ = max_ticks;
}

void TickSystem::Update(float frame_delta_time) {
    // Update timestamp
    last_update_time_ = std::chrono::system_clock::now();

    if (!tick_callback_) {
        return;  // No callback registered yet
    }

    // ========================================================================
    // MODE 1: CATCH-UP (Processing offline time)
    // ========================================================================
    if (is_catching_up_) {
        uint32_t ticks_to_process = std::min(
            static_cast<uint32_t>(target_tick_ - current_tick_),
            max_ticks_per_frame_
        );

        for (uint32_t i = 0; i < ticks_to_process; ++i) {
            tick_callback_(tick_duration_);
            ++current_tick_;
        }

        // Invoke progress callback
        if (progress_callback_) {
            float progress = GetCatchUpProgress();
            progress_callback_(progress, current_tick_, target_tick_);
        }

        // Check if catch-up complete
        if (current_tick_ >= target_tick_) {
            is_catching_up_ = false;
            accumulated_time_ = 0.0f;
        }

        return;
    }

    // ========================================================================
    // MODE 2: NORMAL PLAY (Fixed timestep)
    // ========================================================================
    accumulated_time_ += frame_delta_time;

    // Clamp to prevent spiral of death (if frame time > 1 second)
    constexpr float MAX_ACCUMULATED_TIME = 0.25f;  // Max 250ms of ticks per frame
    accumulated_time_ = std::min(accumulated_time_, MAX_ACCUMULATED_TIME);

    // Process fixed-timestep ticks
    while (accumulated_time_ >= tick_duration_) {
        tick_callback_(tick_duration_);
        accumulated_time_ -= tick_duration_;
        ++current_tick_;
    }
}

float TickSystem::GetCatchUpProgress() const {
    if (target_tick_ == 0) {
        return 1.0f;
    }
    return static_cast<float>(current_tick_) / static_cast<float>(target_tick_);
}

void TickSystem::SimulateOfflineTime(float elapsed_seconds) {
    if (elapsed_seconds <= 0.0f) {
        return;
    }

    // Calculate how many ticks occurred while offline
    uint64_t ticks_to_simulate = static_cast<uint64_t>(
        elapsed_seconds * static_cast<float>(target_ticks_per_second_)
    );

    if (ticks_to_simulate == 0) {
        return;
    }

    // Enter catch-up mode
    target_tick_ = current_tick_ + ticks_to_simulate;
    is_catching_up_ = true;

    // Notify via progress callback
    if (progress_callback_) {
        progress_callback_(0.0f, current_tick_, target_tick_);
    }
}

} // namespace paradox::core
