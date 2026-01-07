#include "ConfigManager.h"
#include "Config/log_config.h"
#include "Config/button_config.h"

static const char* TAG = "ConfigManager";

ConfigManager::ConfigManager(Preferences* preferences)
    : prefs(preferences)
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

Error ConfigManager::saveButtonAction(uint8_t index, ButtonAction action) {
    if (index >= BUTTON_COUNT) {
        LOG_ERROR(TAG, "Invalid button index: %d (max: %d)", index, BUTTON_COUNT - 1);
        return Error::INVALID_PARAM;
    }

    if (static_cast<uint8_t>(action) > ButtonAction_MAX) {
        LOG_ERROR(TAG, "Invalid button action: %d", static_cast<uint8_t>(action));
        return Error::INVALID_PARAM;
    }

    if (!ensureInitialized()) {
        return Error::NVS_WRITE_FAIL;
    }

    char key[BUTTON_KEY_BUFFER_SIZE];
    getButtonKey(index, key, sizeof(key));
    size_t written = prefs->putUChar(key, static_cast<uint8_t>(action));
    if (written == 0) {
        LOG_ERROR(TAG, "Failed to write button action to NVS for key: %s", key);
        return Error::NVS_WRITE_FAIL;
    }

    LOG_INFO(TAG, "Saved button %d action: %s", index, buttonActionToString(action));
    return Error::OK;
}

ButtonAction ConfigManager::loadButtonAction(uint8_t index) {
    if (index >= BUTTON_COUNT) {
        LOG_ERROR(TAG, "Invalid button index: %d, returning default NONE", index);
        return ButtonAction::NONE;
    }

    if (!ensureInitialized()) {
        LOG_ERROR(TAG, "NVS not initialized, returning default NONE");
        return ButtonAction::NONE;
    }

    char key[BUTTON_KEY_BUFFER_SIZE];
    getButtonKey(index, key, sizeof(key));
    uint8_t stored = prefs->getUChar(key, static_cast<uint8_t>(ButtonAction::NONE));

    if (stored > ButtonAction_MAX) {
        LOG_ERROR(TAG, "Invalid stored button action: %d for key: %s, returning default NONE", stored, key);
        return ButtonAction::NONE;
    }

    ButtonAction action = static_cast<ButtonAction>(stored);
    LOG_DEBUG(TAG, "Loaded button %d action: %s", index, buttonActionToString(action));
    return action;
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
