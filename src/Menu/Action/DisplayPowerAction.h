#pragma once

#include "MenuAction.h"

// Forward declarations
class DisplayInterface;
class MenuController;

/**
 * @brief Menu action to toggle display power state
 *
 * Toggles the display power on/off (session-only, no persistence):
 * 1. Read current state from HardwareState
 * 2. Toggle power via DisplayInterface
 * 3. When turning OFF, exit menu automatically (user can't see menu when display is dark)
 * 4. Display always starts ON after boot/reboot
 *
 * Follows the Command Pattern for menu actions.
 * Dependencies are injected via constructor (Dependency Inversion Principle).
 */
class DisplayPowerAction : public MenuAction {
public:
    /**
     * @brief Construct a DisplayPowerAction
     *
     * @param disp DisplayInterface instance for hardware power control
     * @param menuCtrl MenuController for exiting menu when display turns off
     */
    explicit DisplayPowerAction(DisplayInterface* disp, MenuController* menuCtrl);

    /**
     * @brief Execute the display power toggle
     *
     * Toggles display power state (no NVS persistence).
     * When turning OFF, automatically exits menu so controls work immediately.
     *
     * @param context The MenuItem that was selected (unused)
     */
    void execute(const MenuItem* context) override;

    /**
     * @brief Get confirmation message for the power change
     *
     * @return "Display Off" if display is now off, "Display On" if now on
     */
    const char* getConfirmationMessage() override;

private:
    DisplayInterface* display;
    MenuController* menuController;
};
