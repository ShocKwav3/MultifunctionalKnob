#pragma once

#include "MenuAction.h"
#include "Enum/WheelDirection.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

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
     * @param displayQueue Queue for display refresh
     */
    explicit SelectWheelDirectionAction(WheelDirection direction, ConfigManager* config, QueueHandle_t displayQueue);

    /**
     * @brief Execute the wheel direction change
     *
     * Saves the direction to NVS, updates appState.hardwareState.encoderWheelState.direction,
     * and triggers display refresh immediately (AC 4).
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
    QueueHandle_t displayRequestQueue;
};
