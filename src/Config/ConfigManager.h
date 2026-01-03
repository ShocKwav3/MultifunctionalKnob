#pragma once

#include <Preferences.h>
#include "Enum/ErrorEnum.h"
#include "Enum/WheelModeEnum.h"
#include "Enum/ButtonActionEnum.h"
#include "Config/device_config.h"

class ConfigManager {
public:
    explicit ConfigManager(Preferences* preferences);

    Error saveWheelMode(WheelMode mode);
    WheelMode loadWheelMode();

    Error saveButtonAction(uint8_t index, ButtonAction action);
    ButtonAction loadButtonAction(uint8_t index);

    /**
     * @brief Clear all configuration data from NVS
     * @return Error::OK on success, Error::NVS_WRITE_FAIL on failure
     */
    Error clearAll();

private:
    static constexpr const char* KEY_WHEEL_MODE = "wheel.mode";
    static constexpr uint8_t BUTTON_KEY_BUFFER_SIZE = 16;

    Preferences* prefs;
    bool initialized;

    bool ensureInitialized();
    void getButtonKey(uint8_t index, char* buffer, size_t bufferSize) const;
};
