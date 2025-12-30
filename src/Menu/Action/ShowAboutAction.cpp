#include "ShowAboutAction.h"

#include "Config/device_config.h"
#include "Config/log_config.h"
#include "Display/Interface/DisplayInterface.h"
#include "Menu/Model/MenuItem.h"
#include "version.h"

ShowAboutAction::ShowAboutAction(DisplayInterface* display)
    : displayInterface(display) {
}

void ShowAboutAction::execute(const MenuItem* context) {
    // Context unused - about display has no parameters
    
    if (!displayInterface) {
        LOG_ERROR("ShowAbout", "DisplayInterface is null");
        return;
    }
    
    // Clear display before showing about info
    displayInterface->clear();
    
    // Show device information
    displayInterface->showStatus("Device", BLUETOOTH_DEVICE_NAME);
    displayInterface->showStatus("Version", FIRMWARE_VERSION);
    displayInterface->showStatus("By", BLUETOOTH_DEVICE_MANUFACTURER);
    
    LOG_INFO("ShowAbout", "About screen displayed");
}

const char* ShowAboutAction::getConfirmationMessage() {
    return nullptr;  // Keep about screen visible until user navigates back
}
