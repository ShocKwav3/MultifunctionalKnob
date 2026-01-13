#pragma once

#include "MenuAction.h"
#include "BleKeyboard.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Forward declarations
struct DisplayRequest;
struct BlePairingState;

/**
 * @brief Menu action to initiate Bluetooth pairing
 * 
 * Disconnects any existing connection and starts BLE advertising
 * to make the device discoverable for pairing. User remains in menu
 * after pairing completes.
 * 
 * Follows Command Pattern for menu actions.
 */
class PairAction : public MenuAction {
public:
    /**
     * @brief Construct a PairAction
     * @param ble BleKeyboard instance for pairing control
     * @param displayQueue Queue for display feedback
     * @param pairingState Shared pairing state for conflict detection
     */
    explicit PairAction(BleKeyboard* ble, QueueHandle_t displayQueue, BlePairingState* pairingState);

    /**
     * @brief Execute the pairing action
     *
     * Disconnects if connected, clears bonds, starts advertising, shows "BLE: Pairing..." status.
     * Sets pairing mode flag to detect auto-reconnect conflicts (reason 531).
     * Execution is async - connection feedback comes via BLE callbacks.
     *
     * @param context The MenuItem that was selected (unused)
     */
    void execute(const MenuItem* context) override;

    /**
     * @brief Get confirmation message
     * @return nullptr (async action - feedback via callbacks)
     */
    const char* getConfirmationMessage() override;

private:
    BleKeyboard* bleKeyboard;
    QueueHandle_t displayRequestQueue;
    BlePairingState* pairingState;
};
