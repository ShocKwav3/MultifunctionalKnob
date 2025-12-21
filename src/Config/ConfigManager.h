#pragma once

#include <Preferences.h>
#include "Enum/ErrorEnum.h"
#include "Enum/WheelModeEnum.h"
#include "Enum/ButtonActionEnum.h"

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
    static constexpr const char* NVS_NAMESPACE = "knobkoky";
    static constexpr const char* KEY_WHEEL_MODE = "wheel.mode";

    Preferences* prefs;
    bool initialized;

    bool ensureInitialized();
    const char* getButtonKey(uint8_t index);
};
