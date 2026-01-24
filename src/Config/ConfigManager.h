#pragma once

#include <Preferences.h>
#include "Enum/ErrorEnum.h"
#include "Enum/WheelModeEnum.h"
#include "Enum/WheelDirection.h"
#include "Enum/ButtonActionEnum.h"
#include "Config/device_config.h"
#include "Config/macro_config.h"
#include "Type/MacroDefinition.h"

class ConfigManager {
public:
    explicit ConfigManager(Preferences* preferences);

    Error saveWheelMode(WheelMode mode);
    WheelMode loadWheelMode();

    Error setWheelDirection(WheelDirection direction);
    WheelDirection getWheelDirection() const;

    Error saveButtonAction(uint8_t index, ButtonAction action);
    ButtonAction loadButtonAction(uint8_t index);

    /**
     * @brief Load a macro definition from NVS
     * @param index Macro slot index (0 to MACRO_INPUT_COUNT-1)
     * @param out Output parameter to store loaded macro (empty macro if key doesn't exist)
     * @return Error::OK on success, Error::INVALID_PARAM if index out of range, Error::NVS_READ_FAIL if NVS not initialized
     */
    Error loadMacro(uint8_t index, MacroDefinition& out);

    /**
     * @brief Save a macro definition to NVS
     * @param index Macro slot index (0 to MACRO_INPUT_COUNT-1)
     * @param packed Packed macro value (modifiers << 8 | keycode)
     * @return Error::OK on success, Error::INVALID_PARAM if index out of range, Error::NVS_WRITE_FAIL on failure
     */
    Error saveMacro(uint8_t index, uint16_t packed);

    /**
     * @brief Clear all configuration data from NVS
     * @return Error::OK on success, Error::NVS_WRITE_FAIL on failure
     */
    Error clearAll();

private:
    static constexpr uint8_t BUTTON_KEY_BUFFER_SIZE = 16;

    Preferences* prefs;
    bool initialized;

    bool ensureInitialized();
    void getButtonKey(uint8_t index, char* buffer, size_t bufferSize) const;
};
