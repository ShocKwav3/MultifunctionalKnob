#include "ConfigManager.h"
#include "Config/log_config.h"
#include "Config/button_config.h"
#include "BLE/BleKeyboardService.h"
#include "Enum/MacroInputEnum.h"

static const char* TAG = "ConfigManager";

ConfigManager::ConfigManager(Preferences* preferences, BleKeyboardService* bleService)
    : prefs(preferences)
    , bleKeyboardService(bleService)
    , initialized(false) {
}

bool ConfigManager::ensureInitialized() {
    if (initialized) {
        return true;
    }

    if (prefs == nullptr) {
        LOG_ERROR(TAG, "Preferences pointer is null");
        return false;
    }

    initialized = prefs->begin(NVS_NAMESPACE, false);
    if (!initialized) {
        LOG_ERROR(TAG, "Failed to initialize NVS namespace: %s", NVS_NAMESPACE);
    }
    return initialized;
}

void ConfigManager::getButtonKey(uint8_t index, char* buffer, size_t bufferSize) const {
    snprintf(buffer, bufferSize, "btn%d.action", index);
}

Error ConfigManager::saveWheelMode(WheelMode mode) {
    if (static_cast<uint8_t>(mode) > WheelMode_MAX) {
        LOG_ERROR(TAG, "Invalid wheel mode: %d", static_cast<uint8_t>(mode));
        return Error::INVALID_PARAM;
    }

    if (!ensureInitialized()) {
        return Error::NVS_WRITE_FAIL;
    }

    size_t written = prefs->putUChar(KEY_WHEEL_MODE, static_cast<uint8_t>(mode));
    if (written == 0) {
        LOG_ERROR(TAG, "Failed to write wheel mode to NVS");
        return Error::NVS_WRITE_FAIL;
    }

    LOG_INFO(TAG, "Saved wheel mode: %s", wheelModeToString(mode));
    return Error::OK;
}

WheelMode ConfigManager::loadWheelMode() {
    if (!ensureInitialized()) {
        LOG_ERROR(TAG, "NVS not initialized, returning default SCROLL");
        return WheelMode::SCROLL;
    }

    uint8_t stored = prefs->getUChar(KEY_WHEEL_MODE, static_cast<uint8_t>(WheelMode::SCROLL));

    if (stored > WheelMode_MAX) {
        LOG_ERROR(TAG, "Invalid stored wheel mode: %d, returning default SCROLL", stored);
        return WheelMode::SCROLL;
    }

    WheelMode mode = static_cast<WheelMode>(stored);
    LOG_DEBUG(TAG, "Loaded wheel mode: %s", wheelModeToString(mode));
    return mode;
}

Error ConfigManager::setWheelDirection(WheelDirection direction) {
    if (static_cast<uint8_t>(direction) > WheelDirection_MAX) {
        LOG_ERROR(TAG, "Invalid wheel direction: %d", static_cast<uint8_t>(direction));
        return Error::INVALID_PARAM;
    }

    if (!ensureInitialized()) {
        return Error::NVS_WRITE_FAIL;
    }

    size_t written = prefs->putUChar(KEY_WHEEL_DIR, static_cast<uint8_t>(direction));
    if (written == 0) {
        LOG_ERROR(TAG, "Failed to write wheel direction to NVS");
        return Error::NVS_WRITE_FAIL;
    }

    LOG_INFO(TAG, "Saved wheel direction: %s", wheelDirectionToString(direction));
    return Error::OK;
}

WheelDirection ConfigManager::getWheelDirection() const {
    if (!const_cast<ConfigManager*>(this)->ensureInitialized()) {
        LOG_ERROR(TAG, "NVS not initialized, returning default NORMAL");
        return DEFAULT_WHEEL_DIR;
    }

    uint8_t stored = prefs->getUChar(KEY_WHEEL_DIR, static_cast<uint8_t>(DEFAULT_WHEEL_DIR));

    if (stored > WheelDirection_MAX) {
        LOG_ERROR(TAG, "Invalid stored wheel direction: %d, returning default NORMAL", stored);
        return DEFAULT_WHEEL_DIR;
    }

    WheelDirection direction = static_cast<WheelDirection>(stored);
    LOG_DEBUG(TAG, "Loaded wheel direction: %s", wheelDirectionToString(direction));
    return direction;
}

Error ConfigManager::saveButtonAction(uint8_t index, ButtonActionId action) {
    if (index >= BUTTON_COUNT) {
        LOG_ERROR(TAG, "Invalid button index: %d (max: %d)", index, BUTTON_COUNT - 1);
        return Error::INVALID_PARAM;
    }

    // Validate action ID via service
    if (bleKeyboardService && !bleKeyboardService->isValidActionId(action)) {
        LOG_ERROR(TAG, "Invalid button action ID: %d", action);
        return Error::INVALID_PARAM;
    }

    if (!ensureInitialized()) {
        return Error::NVS_WRITE_FAIL;
    }

    char key[BUTTON_KEY_BUFFER_SIZE];
    getButtonKey(index, key, sizeof(key));
    size_t written = prefs->putUChar(key, action);
    if (written == 0) {
        LOG_ERROR(TAG, "Failed to write button action to NVS for key: %s", key);
        return Error::NVS_WRITE_FAIL;
    }

    const char* actionName = bleKeyboardService ? bleKeyboardService->getActionIdentifier(action) : "UNKNOWN";
    LOG_INFO(TAG, "Saved button %d action: %s", index, actionName);
    return Error::OK;
}

ButtonActionId ConfigManager::loadButtonAction(uint8_t index) {
    if (index >= BUTTON_COUNT) {
        LOG_ERROR(TAG, "Invalid button index: %d, returning default NONE", index);
        return 0;  // ID 0 = NONE
    }

    if (!ensureInitialized()) {
        LOG_ERROR(TAG, "NVS not initialized, returning default NONE");
        return 0;  // ID 0 = NONE
    }

    char key[BUTTON_KEY_BUFFER_SIZE];
    getButtonKey(index, key, sizeof(key));
    uint8_t stored = prefs->getUChar(key, 0);  // Default to ID 0 (NONE)

    // Validate loaded ID via service
    if (bleKeyboardService && !bleKeyboardService->isValidActionId(stored)) {
        LOG_ERROR(TAG, "Invalid stored button action ID: %d for key: %s, returning default NONE", stored, key);
        return 0;  // ID 0 = NONE
    }

    const char* actionName = bleKeyboardService ? bleKeyboardService->getActionIdentifier(stored) : "UNKNOWN";
    LOG_DEBUG(TAG, "Loaded button %d action: %s", index, actionName);
    return stored;
}

Error ConfigManager::loadMacro(uint8_t index, MacroDefinition& out) {
    if (index >= static_cast<uint8_t>(MacroInput::COUNT)) {
        LOG_ERROR(TAG, "Invalid macro index: %d (max: %d)", index, static_cast<uint8_t>(MacroInput::COUNT) - 1);
        return Error::INVALID_PARAM;
    }

    if (!ensureInitialized()) {
        LOG_ERROR(TAG, "NVS not initialized, returning empty macro");
        out = MacroDefinition{0, 0};  // Return empty macro
        return Error::NVS_READ_FAIL;
    }

    char key[16];
    snprintf(key, sizeof(key), "macro.%d", index);

    // Load from NVS (default to 0x0000 if key doesn't exist)
    uint16_t packed = prefs->getUShort(key, 0x0000);
    out = MacroDefinition::fromPacked(packed);

    if (packed == 0x0000) {
        LOG_DEBUG(TAG, "Macro %d is empty (key: %s)", index, key);
    } else {
        LOG_DEBUG(TAG, "Loaded macro %d: 0x%04X (key: %s)", index, packed, key);
    }

    return Error::OK;
}

Error ConfigManager::saveMacro(uint8_t index, uint16_t packed) {
    if (index >= static_cast<uint8_t>(MacroInput::COUNT)) {
        LOG_ERROR(TAG, "Invalid macro index: %d (max: %d)", index, static_cast<uint8_t>(MacroInput::COUNT) - 1);
        return Error::INVALID_PARAM;
    }

    if (!ensureInitialized()) {
        return Error::NVS_WRITE_FAIL;
    }

    char key[16];
    snprintf(key, sizeof(key), "macro.%d", index);

    size_t written = prefs->putUShort(key, packed);
    if (written == 0) {
        LOG_ERROR(TAG, "Failed to write macro to NVS for key: %s", key);
        return Error::NVS_WRITE_FAIL;
    }

    LOG_INFO(TAG, "Saved macro %d: 0x%04X", index, packed);
    return Error::OK;
}

Error ConfigManager::clearAll() {
    if (!ensureInitialized()) {
        return Error::NVS_WRITE_FAIL;
    }

    bool cleared = prefs->clear();
    if (!cleared) {
        LOG_ERROR(TAG, "Failed to clear NVS namespace: %s", NVS_NAMESPACE);
        return Error::NVS_WRITE_FAIL;
    }

    LOG_INFO(TAG, "Cleared all configuration from NVS namespace: %s", NVS_NAMESPACE);
    return Error::OK;
}
