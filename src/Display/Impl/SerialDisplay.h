#pragma once

#include <Arduino.h>
#include "../Interface/DisplayInterface.h"

/**
 * @brief Serial monitor implementation of DisplayInterface
 *
 * Outputs display content to Serial for debugging and monitoring via serial terminal.
 */
class SerialDisplay : public DisplayInterface {
public:
    SerialDisplay();

    void showMenu(const char* title, const char** items, uint8_t count, uint8_t selected) override;
    void showMessage(const char* message) override;
    void showConfirmation(const char* message) override;
    void showStatus(const char* key, const char* value) override;
    void clear() override;

private:
    static constexpr const char* TAG = "SerialDisplay";
};
