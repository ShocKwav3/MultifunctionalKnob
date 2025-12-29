#pragma once

#include "MenuAction.h"
#include "Enum/ButtonActionEnum.h"

// Forward declaration
class ConfigManager;

/**
 * @brief Menu action to assign a behavior to a button
 *
 * Encapsulates button action assignment:
 * 1. Persist the button action to NVS via ConfigManager
 * 2. Runtime mapping updated by ButtonManager (optional)
 *
 * Follows the Command Pattern for menu actions.
 */
class SetButtonBehaviorAction : public MenuAction {
public:
    /**
     * @brief Construct a SetButtonBehaviorAction
     *
     * @param buttonIndex Index of the button (0 to BUTTON_COUNT-1)
     * @param action The ButtonAction to assign (MUTE, PLAY, PAUSE, etc.)
     * @param config ConfigManager instance for NVS persistence
     */
    explicit SetButtonBehaviorAction(uint8_t buttonIndex, ButtonAction action, ConfigManager* config);

    /**
     * @brief Execute the button action assignment
     *
     * Saves the action to NVS and updates runtime mapping.
     * Action assignment takes effect without restart.
     */
    void execute() override;

    /**
     * @brief Get confirmation message for the action assignment
     *
     * @return Action-specific confirmation message:
     *         - "Mute Assigned" for MUTE
     *         - "Play Assigned" for PLAY
     *         - "Pause Assigned" for PAUSE
     *         - "Next Assigned" for NEXT
     *         - "Previous Assigned" for PREVIOUS
     *         - "Button Cleared" for NONE
     */
    const char* getConfirmationMessage() override;

private:
    uint8_t buttonIndex;
    ButtonAction action;
    ConfigManager* configManager;
};
