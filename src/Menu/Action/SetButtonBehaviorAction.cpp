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
    uint8_t buttonIndex = extractButtonIndex(context);

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
    
    LOG_INFO("SetButtonBehavior", "Button %d set to: %s", buttonIndex, buttonActionToString(action));
}

uint8_t SetButtonBehaviorAction::extractButtonIndex(const MenuItem* context) const {
    // Context is the parent branch (currentItem from MenuController)
    // It should be one of the button submenu items: "Button 1", "Button 2", "Button 3", "Button 4"
    
    // Null pointer validation
    if (!context) {
        LOG_ERROR("SetButtonBehavior", "Null menu context - defaulting to button 0");
        return 0;
    }
    
    if (!context->label) {
        LOG_ERROR("SetButtonBehavior", "Menu context has null label - defaulting to button 0");
        return 0;
    }
    
    // Validate label format: must start with "Button "
    const char* expectedPrefix = "Button ";
    const size_t prefixLen = 7; // strlen("Button ")
    
    if (strncmp(context->label, expectedPrefix, prefixLen) != 0) {
        LOG_ERROR("SetButtonBehavior", "Invalid label format '%s' (expected 'Button X') - defaulting to button 0", context->label);
        return 0;
    }
    
    // Extract and validate the button number character
    char buttonChar = context->label[prefixLen];
    
    // Validate digit range '1' to '4'
    if (buttonChar < '1' || buttonChar > '0' + BUTTON_COUNT) {
        LOG_ERROR("SetButtonBehavior", "Button number '%c' out of valid range (1-%d) in label '%s' - defaulting to button 0", 
                  buttonChar, BUTTON_COUNT, context->label);
        return 0;
    }
    
    // Ensure the string ends after the digit (no trailing characters)
    if (context->label[prefixLen + 1] != '\0') {
        LOG_ERROR("SetButtonBehavior", "Label '%s' has unexpected trailing characters - defaulting to button 0", context->label);
        return 0;
    }
    
    // Convert '1'-'4' to 0-3 index
    uint8_t index = buttonChar - '1';
    
    LOG_DEBUG("SetButtonBehavior", "Extracted button index %d from label: %s", index, context->label);
    return index;
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
