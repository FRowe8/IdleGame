#pragma once

#include <cstdint>
#include <functional>
#include <chrono>

namespace paradox::core {

/**
 * @brief Fixed-timestep game loop with chunked offline catch-up
 *
 * PRODUCTION CONSTRAINT: Offline Catch-up
 * ========================================
 * When a player returns after 24 hours, we CANNOT run 86,400 ticks
 * in a single frame (would freeze the UI).
 *
 * Solution: Chunked Simulation
 * - Break catch-up into smaller batches (e.g., 1000 ticks per frame)
 * - Spread computation across multiple frames
 * - Provide progress callbacks for UI loading bar
 *
 * Example:
 *   Player offline for 1 hour = 216,000 ticks (60 Hz * 3600 sec)
 *   Process in chunks of 5000 ticks
 *   = 43.2 frames of catch-up (< 1 second at 60 FPS)
 *
 * Usage:
 *   TickSystem tick_system(60);  // 60 Hz tick rate
 *   tick_system.SetTickCallback([](float dt) {
 *       game_state.Update(dt);
 *   });
 *
 *   // In main loop:
 *   tick_system.Update(frame_delta_time);
 *   if (tick_system.IsCatchingUp()) {
 *       ui.ShowLoadingBar(tick_system.GetCatchUpProgress());
 *   }
 */
class TickSystem {
public:
    using TickCallback = std::function<void(float delta_time)>;
    using ProgressCallback = std::function<void(float progress, uint64_t current_tick, uint64_t target_tick)>;

    // ========================================================================
    // CONFIGURATION
    // ========================================================================
    explicit TickSystem(uint32_t target_ticks_per_second = 60);

    /**
     * @brief Set the callback invoked every logical tick
     * @param callback Function receiving delta_time (in seconds, usually 1/60)
     */
    void SetTickCallback(TickCallback callback);

    /**
     * @brief Set the callback invoked during catch-up progression
     * @param callback Function receiving (progress [0-1], current_tick, target_tick)
     */
    void SetProgressCallback(ProgressCallback callback);

    /**
     * @brief Configure chunked catch-up behavior
     * @param max_ticks_per_frame Maximum ticks to process in one Update() call
     */
    void SetMaxTicksPerFrame(uint32_t max_ticks);

    // ========================================================================
    // MAIN UPDATE LOOP
    // ========================================================================
    /**
     * @brief Process elapsed time and invoke tick callbacks
     *
     * This is the ONLY method called from main.cpp's game loop.
     * Internally handles:
     * - Fixed timestep accumulation
     * - Chunked catch-up if player was offline
     * - Progress callbacks for UI updates
     *
     * @param frame_delta_time Time since last Update() call (seconds)
     */
    void Update(float frame_delta_time);

    // ========================================================================
    // STATE QUERIES
    // ========================================================================
    [[nodiscard]] bool IsCatchingUp() const { return is_catching_up_; }
    [[nodiscard]] float GetCatchUpProgress() const;  // 0.0 - 1.0
    [[nodiscard]] uint64_t GetCurrentTick() const { return current_tick_; }

    // ========================================================================
    // OFFLINE PROGRESSION SUPPORT
    // ========================================================================
    /**
     * @brief Simulate time passing while app was closed
     *
     * Called when loading a save file that has an older timestamp.
     *
     * @param elapsed_seconds Time since last save (e.g., 86400 for 24 hours)
     */
    void SimulateOfflineTime(float elapsed_seconds);

    /**
     * @brief Get the last update timestamp (for save/load)
     */
    [[nodiscard]] std::chrono::system_clock::time_point GetLastUpdateTime() const {
        return last_update_time_;
    }

    /**
     * @brief Restore the last update timestamp (when loading save)
     */
    void SetLastUpdateTime(std::chrono::system_clock::time_point time) {
        last_update_time_ = time;
    }

private:
    // Configuration
    uint32_t target_ticks_per_second_;
    float tick_duration_;  // 1.0f / target_ticks_per_second_
    uint32_t max_ticks_per_frame_ = 5000;  // Default: process 5000 ticks per frame during catch-up

    // Callbacks
    TickCallback tick_callback_;
    ProgressCallback progress_callback_;

    // State
    float accumulated_time_ = 0.0f;
    uint64_t current_tick_ = 0;
    uint64_t target_tick_ = 0;  // Used during catch-up
    bool is_catching_up_ = false;

    // Timestamp tracking (for offline progression)
    std::chrono::system_clock::time_point last_update_time_;
};

} // namespace paradox::core
