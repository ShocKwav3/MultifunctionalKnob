#pragma once

#include "MenuAction.h"

// Forward declarations
class DisplayInterface;

/**
 * @brief Menu action to display device information and firmware version
 *
 * Shows device name, firmware version, and manufacturer information.
 * Provides an "About" screen for users to reference for support.
 */
class ShowAboutAction : public MenuAction {
public:
    /**
     * @brief Construct a ShowAboutAction
     *
     * @param display DisplayInterface to show about information
     */
    explicit ShowAboutAction(DisplayInterface* display);

    /**
     * @brief Execute the about display
     *
     * Displays:
     * - Device name
     * - Firmware version
     * - Manufacturer
     *
     * @param context The MenuItem that was selected (unused)
     */
    void execute(const MenuItem* context) override;

    /**
     * @brief No confirmation message - keeps about screen visible
     *
     * @return nullptr to keep about display visible until user navigates back
     */
    const char* getConfirmationMessage() override;

private:
    DisplayInterface* displayInterface;
};
