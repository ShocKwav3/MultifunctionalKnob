#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "../Interface/DisplayInterface.h"
#include "state/HardwareState.h"
#include "Config/display_config.h"

/**
 * @brief OLED display implementation of DisplayInterface for 128x32 SSD1306
 *
 * Hardware-backed display implementation using Adafruit SSD1306 driver.
 * Displays menus, messages, and status on physical 128x32 OLED screen.
 */
class OLEDDisplay : public DisplayInterface {
public:
    OLEDDisplay();

    void showMenu(const char* title, const char* const* items, uint8_t count, uint8_t selected, const HardwareState& state) override;
    void showMessage(const char* message) override;
    void showConfirmation(const char* message) override;
    void showStatus(const char* key, const char* value) override;
    void clear() override;

    /**
     * @brief Draw normal mode status screen with icons
     * @param state Hardware state containing mode, direction, battery, BT status
     */
    void drawNormalMode(const HardwareState& state) override;

private:
    Adafruit_SSD1306 display;
    bool initialized;

    void ensureInitialized();
    void centerText(const char* text, uint8_t y);

    // Normal mode drawing helpers (decomposed from drawNormalMode)
    void drawStatusBar(const HardwareState& state);
    void drawModeIndicator(WheelMode mode);
    void drawDirectionIndicator(WheelDirection direction);

    // Menu mode drawing helpers
    void drawMenuModeStatusBar(const HardwareState& state);

    static constexpr const char* TAG = "OLEDDisplay";
};
