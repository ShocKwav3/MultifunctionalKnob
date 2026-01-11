#pragma once

#include "MenuAction.h"
#include "BleKeyboard.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Forward declarations
struct DisplayRequest;

/**
 * @brief Menu action to disconnect active Bluetooth connection
 *
 * Disconnects the current BLE connection if one exists.
 * Shows "Disconnected" if connected, or "Not connected" if already disconnected.
 *
 * Follows Command Pattern for menu actions.
 */
class DisconnectAction : public MenuAction {
public:
    /**
     * @brief Construct a DisconnectAction
     * @param ble BleKeyboard instance for disconnect control
     * @param displayQueue Queue for display feedback
     */
    explicit DisconnectAction(BleKeyboard* ble, QueueHandle_t displayQueue);

    /**
     * @brief Execute the disconnect action
     *
     * Checks connection state, disconnects if connected, shows appropriate feedback.
     *
     * @param context The MenuItem that was selected (unused)
     */
    void execute(const MenuItem* context) override;

    /**
     * @brief Get confirmation message
     * @return nullptr (feedback via display request)
     */
    const char* getConfirmationMessage() override;

private:
    BleKeyboard* bleKeyboard;
    QueueHandle_t displayRequestQueue;
};
