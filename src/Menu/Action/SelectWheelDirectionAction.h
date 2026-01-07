#pragma once

#include "MenuAction.h"
#include "Enum/WheelDirection.h"

// Forward declaration
class ConfigManager;

/**
 * @brief Menu action to select wheel direction (Normal/Reversed)
 *
 * Encapsulates the user's wheel direction selection:
 * - Persist the direction setting to NVS via ConfigManager
 *
 * Follows the Command Pattern for menu actions.
 */
class SelectWheelDirectionAction : public MenuAction {
public:
    /**
     * @brief Construct a SelectWheelDirectionAction
     *
     * @param direction The target WheelDirection (NORMAL or REVERSED)
     * @param config ConfigManager instance for NVS persistence
     */
    explicit SelectWheelDirectionAction(WheelDirection direction, ConfigManager* config);

    /**
     * @brief Execute the wheel direction change
     *
     * Saves the direction to NVS. Direction will be applied to encoder
     * input processing after device restart.
     *
     * @param context The MenuItem that was selected (unused)
     */
    void execute(const MenuItem* context) override;

    /**
     * @brief Get confirmation message for the direction change
     *
     * @return Confirmation message: "Direction saved"
     */
    const char* getConfirmationMessage() override;

private:
    WheelDirection targetDirection;
    ConfigManager* configManager;
};
