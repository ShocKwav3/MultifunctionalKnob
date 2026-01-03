#include "SetButtonBehaviorAction.h"
#include "Config/ConfigManager.h"
#include "Config/button_config.h"
#include "Config/log_config.h"
#include "Menu/Model/MenuItem.h"
#include "Event/Handler/ButtonEventHandler.h"
#include <string.h>

SetButtonBehaviorAction::SetButtonBehaviorAction(ButtonAction action, ConfigManager* config, ButtonEventHandler* buttonHandler)
    : action(action), configManager(config), buttonEventHandler(buttonHandler) {
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
    
    LOG_INFO("SetButtonBehavior", "%s set to: %s", BUTTONS[buttonIndex].label, buttonActionToString(action));
}

int8_t SetButtonBehaviorAction::extractButtonIndex(const MenuItem* context) const {
    // Context is the parent branch (currentItem from MenuController)
    // It should be one of the button submenu items with labels from BUTTONS[].label
    
    // Null pointer validation
    if (!context) {
        LOG_ERROR("SetButtonBehavior", "Null menu context - returning invalid index");
        return -1;
    }
    
    if (!context->label) {
        LOG_ERROR("SetButtonBehavior", "Menu context has null label - returning invalid index");
        return -1;
    }
    
    // Match label against BUTTONS[].label to find button index
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        if (strcmp(context->label, BUTTONS[i].label) == 0) {
            LOG_DEBUG("SetButtonBehavior", "Matched button index %d from label: %s", i, context->label);
            return i;
        }
    }
    
    // No match found - log error and return invalid index
    LOG_ERROR("SetButtonBehavior", "Label '%s' does not match any button label - returning invalid index", context->label);
    return -1;
}

const char* SetButtonBehaviorAction::getConfirmationMessage() {
    switch (action) {
        case ButtonAction::MUTE:
            return "Mute Assigned";
        case ButtonAction::PLAY:
            return "Play Assigned";
        case ButtonAction::PAUSE:
            return "Pause Assigned";
        case ButtonAction::NEXT:
            return "Next Assigned";
        case ButtonAction::PREVIOUS:
            return "Previous Assigned";
        case ButtonAction::NONE:
            return "Button Cleared";
        default:
            return "Action Assigned";
    }
}
