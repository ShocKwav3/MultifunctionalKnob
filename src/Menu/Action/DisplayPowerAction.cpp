#include "DisplayPowerAction.h"
#include "Display/Interface/DisplayInterface.h"
#include "Menu/Controller/MenuController.h"
#include "Config/log_config.h"
#include "state/HardwareState.h"

static const char* TAG = "DisplayPowerAction";

DisplayPowerAction::DisplayPowerAction(DisplayInterface* disp, MenuController* menuCtrl)
    : display(disp), menuController(menuCtrl) {
}

void DisplayPowerAction::execute(const MenuItem* context) {
    // Context unused - power toggle is stateless

    // Get current power state from HardwareState (single source of truth)
    bool currentPower = hardwareState.displayPower;
    bool newPower = !currentPower;

    LOG_INFO(TAG, "Toggling display: %s -> %s",
             currentPower ? "ON" : "OFF",
             newPower ? "ON" : "OFF");

    // Toggle display power (no NVS persistence - display always starts ON after boot)
    display->setPower(newPower);  // Updates hardwareState.displayPower internally

    // When turning OFF, exit menu so user can use controls immediately (can't see menu anyway)
    if (!newPower && menuController != nullptr) {
        LOG_INFO(TAG, "Display OFF - deactivating menu for immediate control access");
        menuController->deactivate();
    }
}

const char* DisplayPowerAction::getConfirmationMessage() {
    // Return message indicating the NEW state (after toggle)
    // Read from HardwareState (single source of truth)
    bool currentPower = hardwareState.displayPower;
    return currentPower ? "Display On" : "Display Off";
}
