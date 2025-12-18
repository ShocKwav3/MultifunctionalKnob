#pragma once

#include <cstdint>

class ConfigManager;
class DisplayInterface;

/**
 * @brief Factory reset functionality for the device
 *
 * Provides static methods to check for factory reset request at boot
 * (encoder button held for 5+ seconds) and execute the reset by clearing
 * all NVS configuration data.
 *
 * Uses static methods as this runs before full system initialization.
 */
class FactoryReset {
public:
    /**
     * @brief Check if factory reset is requested at boot
     *
     * Checks if the encoder button is held for 5+ seconds at boot time.
     * Should be called early in setup() before other initialization.
     *
     * @param buttonPin GPIO pin number for the encoder button
     * @return true if button was held for 5+ seconds, false otherwise
     */
    static bool isResetRequested(uint8_t buttonPin);

    /**
     * @brief Execute factory reset
     *
     * Clears all NVS configuration data and displays feedback to the user.
     * After reset, the device will use default values (SCROLL mode, NONE actions).
     *
     * @param configManager Reference to ConfigManager for NVS access
     * @param display Reference to DisplayInterface for user feedback
     */
    static void execute(ConfigManager& configManager, DisplayInterface& display);

private:
    static constexpr uint32_t RESET_HOLD_TIME_MS = 5000;  // 5 seconds
    static constexpr uint32_t POLL_INTERVAL_MS = 100;     // Check every 100ms
};
