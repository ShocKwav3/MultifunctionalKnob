#include "SetButtonBehaviorAction.h"
#include "Config/ConfigManager.h"
#include "Config/button_config.h"
#include "Config/log_config.h"
#include "Menu/Model/MenuItem.h"
#include "Event/Handler/ButtonEventHandler.h"
#include "BLE/BleKeyboardService.h"

SetButtonBehaviorAction::SetButtonBehaviorAction(ButtonActionId actionId, ConfigManager* config, ButtonEventHandler* buttonHandler, BleKeyboardService* bleService)
    : action(actionId), configManager(config), buttonEventHandler(buttonHandler), bleKeyboardService(bleService) {
}

void SetButtonBehaviorAction::execute(const MenuItem* context) {
    // Extract button index from menu navigation context
    int8_t buttonIndex = extractButtonIndex(context);
    
    // Validate button index before proceeding
    if (buttonIndex < 0 || buttonIndex >= BUTTON_COUNT) {
        LOG_ERROR("SetButtonBehavior", "Invalid button index %d - aborting action assignment", buttonIndex);
        return;
    }

    // Persist action to NVS
    Error saveResult = configManager->saveButtonAction(buttonIndex, action);
    if (saveResult != Error::OK) {
        LOG_ERROR("SetButtonBehavior", "Failed to persist button action");
        return;
    }

    // Invalidate ButtonEventHandler cache to force reload on next button press
    if (buttonEventHandler) {
        buttonEventHandler->invalidateCache();
        LOG_DEBUG("SetButtonBehavior", "Invalidated button action cache");
    }
    
    const char* actionName = bleKeyboardService ? bleKeyboardService->getActionDisplayName(action) : "Unknown";
    LOG_INFO("SetButtonBehavior", "%s set to: %s", BUTTONS[buttonIndex].label, actionName);
}

int8_t SetButtonBehaviorAction::extractButtonIndex(const MenuItem* context) const {
    // Context is the parent branch (currentItem from MenuController)
    // It should be one of the button submenu items with button index stored in userData

    // Null pointer validation
    if (!context) {
        LOG_ERROR("SetButtonBehavior", "Null menu context - returning invalid index");
        return -1;
    }

    // Extract button index from userData field (set in MenuTree.h initMenuTree())
    // userData decouples logic from label strings, enabling localization without breaking functionality
    int8_t buttonIndex = static_cast<int8_t>(reinterpret_cast<uintptr_t>(context->userData));

    // Validate index is within valid range
    if (buttonIndex < 0 || buttonIndex >= static_cast<int8_t>(BUTTON_COUNT)) {
        LOG_ERROR("SetButtonBehavior", "Invalid button index %d from userData (valid: 0-%d)", buttonIndex, BUTTON_COUNT - 1);
        return -1;
    }

    LOG_DEBUG("SetButtonBehavior", "Extracted button index %d from userData (label: %s)", buttonIndex, context->label ? context->label : "null");
    return buttonIndex;
}

const char* SetButtonBehaviorAction::getConfirmationMessage() {
    // Lookup confirmation message from service (data-driven)
    return bleKeyboardService ? bleKeyboardService->getActionConfirmation(action) : "Unknown Action";
}
