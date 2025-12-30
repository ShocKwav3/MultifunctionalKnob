#pragma once

#include "MenuAction.h"
#include "BleKeyboard.h"

// Forward declarations
class ConfigManager;
class DisplayInterface;

/**
 * @brief Menu action to display current device status
 *
 * Shows wheel mode, BLE connection status, and button action assignments.
 * Provides a diagnostic screen for users to verify their configuration.
 */
class ShowStatusAction : public MenuAction {
public:
    /**
     * @brief Construct a ShowStatusAction
     *
     * @param config ConfigManager to read wheel mode and button actions
     * @param keyboard BleKeyboard to check connection status
     * @param display DisplayInterface to show status information
     */
    explicit ShowStatusAction(ConfigManager* config, BleKeyboard* keyboard, DisplayInterface* display);

    /**
     * @brief Execute the status display
     *
     * Reads current configuration and displays:
     * - Current wheel mode (SCROLL/VOLUME/ZOOM)
     * - BLE connection status (Connected/Disconnected)
     * - Button action assignments for all 4 buttons
     *
     * @param context The MenuItem that was selected (unused)
     */
    void execute(const MenuItem* context) override;

    /**
     * @brief No confirmation message - keeps status on screen
     *
     * @return nullptr to keep status display visible until user navigates back
     */
    const char* getConfirmationMessage() override;

private:
    ConfigManager* configManager;
    BleKeyboard* bleKeyboard;
    DisplayInterface* displayInterface;
};
