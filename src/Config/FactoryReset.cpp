#include "FactoryReset.h"
#include "ConfigManager.h"
#include "Display/Interface/DisplayInterface.h"
#include "Config/log_config.h"
#include <Arduino.h>

static const char* TAG = "FactoryReset";

bool FactoryReset::isResetRequested(uint8_t buttonPin) {
    pinMode(buttonPin, INPUT_PULLUP);

    // Check if button is currently pressed (active low)
    if (digitalRead(buttonPin) == HIGH) {
        return false;
    }

    LOG_INFO(TAG, "Button held at boot, checking for factory reset...");

    uint32_t startTime = millis();
    uint32_t holdTime = 0;

    // Poll button state for RESET_HOLD_TIME_MS
    while (holdTime < RESET_HOLD_TIME_MS) {
        if (digitalRead(buttonPin) == HIGH) {
            // Button released before timeout
            LOG_INFO(TAG, "Button released after %lu ms, no factory reset", holdTime);
            return false;
        }

        delay(POLL_INTERVAL_MS);
        holdTime = millis() - startTime;
    }

    LOG_INFO(TAG, "Button held for %lu ms, factory reset requested", holdTime);
    return true;
}

void FactoryReset::execute(ConfigManager& configManager, DisplayInterface& display) {
    LOG_INFO(TAG, "Executing factory reset...");

    display.showMessage("Factory Reset...");

    Error result = configManager.clearAll();

    if (result == Error::OK) {
        LOG_INFO(TAG, "Factory reset completed successfully");
        display.showMessage("Complete");
    } else {
        LOG_ERROR(TAG, "Factory reset failed with error: %d", static_cast<int>(result));
        display.showMessage("Reset Failed!");
    }

    // Brief delay to allow user to see the message
    delay(2000);
}
