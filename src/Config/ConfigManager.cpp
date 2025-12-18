#include "ConfigManager.h"
#include "Config/log_config.h"
#include "Config/button_config.h"

static const char* TAG = "ConfigManager";

// Static buffer for button key generation
static char buttonKeyBuffer[16];

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

const char* ConfigManager::getButtonKey(uint8_t index) {
    snprintf(buttonKeyBuffer, sizeof(buttonKeyBuffer), "btn%d.action", index);
    return buttonKeyBuffer;
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

    const char* key = getButtonKey(index);
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

    const char* key = getButtonKey(index);
    uint8_t stored = prefs->getUChar(key, static_cast<uint8_t>(ButtonAction::NONE));

    if (stored > ButtonAction_MAX) {
        LOG_ERROR(TAG, "Invalid stored button action: %d for key: %s, returning default NONE", stored, key);
        return ButtonAction::NONE;
    }

    ButtonAction action = static_cast<ButtonAction>(stored);
    LOG_DEBUG(TAG, "Loaded button %d action: %s", index, buttonActionToString(action));
    return action;
}
