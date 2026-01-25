#include "ShowStatusAction.h"

#include <stdint.h>

#include "BLE/BleKeyboardService.h"
#include "Config/ConfigManager.h"
#include "Config/button_config.h"
#include "Config/log_config.h"
#include "Display/Interface/DisplayInterface.h"
#include "Enum/WheelModeEnum.h"
#include "Menu/Model/MenuItem.h"

ShowStatusAction::ShowStatusAction(ConfigManager* config, BleKeyboardService* bleService, DisplayInterface* display)
    : configManager(config)
    , bleKeyboardService(bleService)
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
    const char* bleStatus = (bleKeyboardService && bleKeyboardService->isConnected()) ? "Connected" : "Disconnected";
    displayInterface->showStatus("BLE", bleStatus);

    // Show button assignments
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        ButtonActionId actionId = configManager->loadButtonAction(i);
        const char* actionName = bleKeyboardService ? bleKeyboardService->getActionDisplayName(actionId) : "Unknown";
        displayInterface->showStatus(BUTTONS[i].label, actionName);
    }
    
    LOG_INFO("ShowStatus", "Status displayed");
}

const char* ShowStatusAction::getConfirmationMessage() {
    return nullptr;  // Keep status on screen until user navigates back
}
