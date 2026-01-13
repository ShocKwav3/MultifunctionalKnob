#pragma once

#include "BleKeyboard.h"
#include "Type/BlePairingState.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Forward declarations
struct DisplayRequest;

/**
 * @brief BLE connection state callback handlers
 *
 * Handles BLE connection and disconnection events with pairing conflict detection.
 * Extracted from main.cpp lambdas to reduce complexity and improve testability.
 *
 * These handlers are called from BleKeyboard callbacks on the BLE task thread.
 */
namespace BleCallbackHandler {
    /**
     * @brief Handle BLE device connection
     *
     * Called when a host device successfully connects.
     * Clears pairing mode flag and sends connection status to display.
     *
     * @param pairingState Shared pairing state for conflict detection
     * @param displayQueue Queue for sending display requests
     */
    void handleConnect(BlePairingState* pairingState, QueueHandle_t displayQueue);

    /**
     * @brief Handle BLE device disconnection
     *
     * Called when a host device disconnects. Detects pairing conflicts (reason 531)
     * when host auto-reconnects with encryption during pairing mode.
     *
     * On pairing conflict: Stops advertising and guides user to forget device.
     * On normal disconnect: Shows disconnected status.
     *
     * @param reason BLE disconnect reason code (531 = encryption/pairing failure)
     * @param pairingState Shared pairing state for conflict detection
     * @param displayQueue Queue for sending display requests
     * @param bleKeyboard BleKeyboard instance for stopping advertising
     */
    void handleDisconnect(int reason, BlePairingState* pairingState,
                         QueueHandle_t displayQueue, BleKeyboard* bleKeyboard);
}
