#include "ShowStatusAction.h"

#include <cstdint>

#include "BleKeyboard.h"

#include "Config/ConfigManager.h"
#include "Config/button_config.h"
#include "Config/log_config.h"
#include "Display/Interface/DisplayInterface.h"
#include "Enum/ButtonActionEnum.h"
#include "Enum/WheelModeEnum.h"
#include "Menu/Model/MenuItem.h"

ShowStatusAction::ShowStatusAction(ConfigManager* config, BleKeyboard* keyboard, DisplayInterface* display)
    : configManager(config)
    , bleKeyboard(keyboard)
    , displayInterface(display) {
}

void ShowStatusAction::execute(const MenuItem* context) {
    // Context unused - status display has no parameters
    
    if (!displayInterface) {
        LOG_ERROR("ShowStatus", "DisplayInterface is null");
        return;
    }
    
    // Clear display before showing status
    displayInterface->clear();
    
    // Show wheel mode
    WheelMode currentMode = configManager->loadWheelMode();
    displayInterface->showStatus("Wheel Mode", wheelModeToString(currentMode));
    
    // Show BLE connection status
    const char* bleStatus = (bleKeyboard && bleKeyboard->isConnected()) ? "Connected" : "Disconnected";
    displayInterface->showStatus("BLE", bleStatus);
    
    // Show button assignments
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        ButtonAction action = configManager->loadButtonAction(i);
        displayInterface->showStatus(BUTTONS[i].label, buttonActionToString(action));
    }
    
    LOG_INFO("ShowStatus", "Status displayed");
}

const char* ShowStatusAction::getConfirmationMessage() {
    return nullptr;  // Keep status on screen until user navigates back
}
