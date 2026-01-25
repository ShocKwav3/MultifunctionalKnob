#include "BleKeyboardService.h"
#include "BleKeyboard.h"
#include "Config/log_config.h"

// Internal mapping structure (implementation detail, not exposed in header)
struct MediaKeyAction {
    uint8_t id;
    const char* identifier;
    const char* displayName;
    const MediaKeyReport* mediaKey;
    const char* confirmMsg;
};

// Single source of truth for button actions
// To add new action: append one line here, rebuild - that's it!
static const MediaKeyAction MEDIA_KEY_ACTIONS[] = {
    { 0, "NONE",         "None",           nullptr,                      "Button Cleared" },
    { 1, "PLAY_PAUSE",   "Play/Pause",     &KEY_MEDIA_PLAY_PAUSE,        "Play/Pause Assigned" },
    { 2, "NEXT",         "Next Track",     &KEY_MEDIA_NEXT_TRACK,        "Next Track Assigned" },
    { 3, "PREVIOUS",     "Previous Track", &KEY_MEDIA_PREVIOUS_TRACK,    "Previous Track Assigned" },
    { 4, "MUTE",         "Mute",           &KEY_MEDIA_MUTE,              "Mute Assigned" },
    { 5, "VOLUME_UP",    "Volume Up",      &KEY_MEDIA_VOLUME_UP,         "Volume Up Assigned" },
    { 6, "VOLUME_DOWN",  "Volume Down",    &KEY_MEDIA_VOLUME_DOWN,       "Volume Down Assigned" },
    { 7, "STOP",         "Stop",           &KEY_MEDIA_STOP,              "Stop Assigned" }
};

static constexpr uint8_t ACTION_COUNT = sizeof(MEDIA_KEY_ACTIONS) / sizeof(MediaKeyAction);

// Compile-time validation: Ensure header constant matches actual array size
// Prevents silent menu truncation if BUTTON_ACTION_COUNT in header is not updated
static_assert(ACTION_COUNT == BUTTON_ACTION_COUNT,
              "ACTION_COUNT mismatch - update BUTTON_ACTION_COUNT in BleKeyboardService.h");

// Helper: Find action by ID
static const MediaKeyAction* findActionById(ButtonActionId id) {
    for (uint8_t i = 0; i < ACTION_COUNT; i++) {
        if (MEDIA_KEY_ACTIONS[i].id == id) {
            return &MEDIA_KEY_ACTIONS[i];
        }
    }
    return nullptr;
}

BleKeyboardService::BleKeyboardService(BleKeyboard* keyboard)
    : bleKeyboard(keyboard) {
    if (!keyboard) {
        LOG_ERROR("BleKeyboardService", "Constructor called with null BleKeyboard");
    }
}

bool BleKeyboardService::executeMediaKey(ButtonActionId actionId) {
    if (!bleKeyboard) {
        LOG_ERROR("BleKeyboardService", "BleKeyboard is null");
        return false;
    }

    if (!bleKeyboard->isConnected()) {
        LOG_DEBUG("BleKeyboardService", "BLE not connected, skipping action %d", actionId);
        return false;
    }

    const MediaKeyAction* action = findActionById(actionId);
    if (!action) {
        LOG_ERROR("BleKeyboardService", "Invalid action ID: %d", actionId);
        return false;
    }

    // NONE action - no key to execute
    if (action->mediaKey == nullptr) {
        LOG_DEBUG("BleKeyboardService", "Action %s has no media key (NONE)", action->identifier);
        return true;  // Not an error, just no-op
    }

    // Execute the media key
    bleKeyboard->write(*action->mediaKey);
    LOG_INFO("BleKeyboardService", "Executed: %s", action->displayName);
    return true;
}

bool BleKeyboardService::isConnected() const {
    return bleKeyboard && bleKeyboard->isConnected();
}

const char* BleKeyboardService::getActionDisplayName(ButtonActionId actionId) const {
    const MediaKeyAction* action = findActionById(actionId);
    return action ? action->displayName : "Unknown";
}

const char* BleKeyboardService::getActionConfirmation(ButtonActionId actionId) const {
    const MediaKeyAction* action = findActionById(actionId);
    return action ? action->confirmMsg : "Unknown Action";
}

const char* BleKeyboardService::getActionIdentifier(ButtonActionId actionId) const {
    const MediaKeyAction* action = findActionById(actionId);
    return action ? action->identifier : "UNKNOWN";
}

uint8_t BleKeyboardService::getActionCount() const {
    return ACTION_COUNT;
}

bool BleKeyboardService::isValidActionId(ButtonActionId actionId) const {
    return findActionById(actionId) != nullptr;
}

ButtonActionId BleKeyboardService::getActionIdByIndex(uint8_t index) const {
    if (index >= ACTION_COUNT) {
        LOG_ERROR("BleKeyboardService", "Index %d out of range (max %d)", index, ACTION_COUNT - 1);
        return 0;  // Out of range - return NONE
    }
    return MEDIA_KEY_ACTIONS[index].id;
}
