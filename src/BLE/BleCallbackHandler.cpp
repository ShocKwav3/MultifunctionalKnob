#include "BleCallbackHandler.h"
#include "Display/Model/DisplayRequest.h"
#include "Config/log_config.h"

namespace BleCallbackHandler {

void handleConnect(BlePairingState* pairingState, QueueHandle_t displayQueue) {
    LOG_INFO("BleCallbackHandler", "BLE device connected");

    // Clear pairing mode flag on successful connection
    if (pairingState) {
        pairingState->isInPairingMode = false;
    }

    // Send connection status to display
    DisplayRequest req;
    req.type = DisplayRequestType::SHOW_STATUS;
    req.data.status.key = "BLE";
    req.data.status.value = "Connected";

    if (xQueueSend(displayQueue, &req, pdMS_TO_TICKS(10)) != pdPASS) {
        LOG_ERROR("BleCallbackHandler", "Failed to send connection display request");
    }

    // NOTE: Menu does NOT exit on connection (user stays in menu)
}

void handleDisconnect(int reason, BlePairingState* pairingState,
                     QueueHandle_t displayQueue, BleKeyboard* bleKeyboard) {
    LOG_INFO("BleCallbackHandler", "BLE device disconnected (reason: %d)", reason);

    // Detect pairing conflict: host auto-reconnected with encryption during pairing
    // Reason 531 (0x0213) = encryption/pairing failure because ESP32 cleared bonds
    // but host still has old pairing keys
    const int BLE_PAIRING_CONFLICT_REASON = 531;

    if (pairingState && pairingState->isInPairingMode && reason == BLE_PAIRING_CONFLICT_REASON) {
        LOG_INFO("BleCallbackHandler", "Pairing conflict detected - host tried encrypted reconnect after bond clearing");

        // Stop advertising to prevent battery-draining reconnect loop
        if (bleKeyboard) {
            bleKeyboard->stopAdvertising();
        }
        pairingState->isInPairingMode = false;

        // Guide user to resolve the conflict
        DisplayRequest req;
        req.type = DisplayRequestType::SHOW_STATUS;
        req.data.status.key = "Pairing";
        req.data.status.value = "Forget device on host, retry Pair";

        if (xQueueSend(displayQueue, &req, pdMS_TO_TICKS(10)) != pdPASS) {
            LOG_ERROR("BleCallbackHandler", "Failed to send pairing conflict display request");
        }
        return;
    }

    // Normal disconnect
    DisplayRequest req;
    req.type = DisplayRequestType::SHOW_STATUS;
    req.data.status.key = "BLE";
    req.data.status.value = "Disconnected";

    if (xQueueSend(displayQueue, &req, pdMS_TO_TICKS(10)) != pdPASS) {
        LOG_ERROR("BleCallbackHandler", "Failed to send disconnect display request");
    }
}

} // namespace BleCallbackHandler
