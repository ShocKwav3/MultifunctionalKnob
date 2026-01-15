#include "BleCallbackHandler.h"
#include "Display/Model/DisplayRequest.h"
#include "Config/log_config.h"
#include "state/HardwareState.h"
#include "freertos/timers.h"

extern HardwareState hardwareState;
extern TimerHandle_t btFlashTimer;

namespace BleCallbackHandler {

void handleConnect(QueueHandle_t displayQueue) {
    LOG_INFO("BleCallbackHandler", "BLE device connected");

    // Update global hardware state
    hardwareState.bleState.isConnected = true;
    hardwareState.bleState.isPairingMode = false;

    // Stop BT flash timer (pairing complete)
    if (btFlashTimer != nullptr) {
        xTimerStop(btFlashTimer, 0);
    }

    // Send connection status to display
    DisplayRequest req;
    req.type = DisplayRequestType::SHOW_STATUS;
    req.data.status.key = "BLE";
    req.data.status.value = "Connected";

    if (xQueueSend(displayQueue, &req, pdMS_TO_TICKS(10)) != pdPASS) {
        LOG_ERROR("BleCallbackHandler", "Failed to send connection display request");
    }

    // Update status screen with new BT state
    DisplayRequest normalModeReq;
    normalModeReq.type = DisplayRequestType::DRAW_NORMAL_MODE;
    normalModeReq.data.normalMode.state = hardwareState;
    xQueueSend(displayQueue, &normalModeReq, pdMS_TO_TICKS(10));

    // NOTE: Menu does NOT exit on connection (user stays in menu)
}

void handleDisconnect(int reason, QueueHandle_t displayQueue, BleKeyboard* bleKeyboard) {
    LOG_INFO("BleCallbackHandler", "BLE device disconnected (reason: %d)", reason);

    // Detect pairing conflict: host auto-reconnected with encryption during pairing
    // Reason 531 (0x0213) = encryption/pairing failure because ESP32 cleared bonds
    // but host still has old pairing keys
    const int BLE_PAIRING_CONFLICT_REASON = 531;

    if (hardwareState.bleState.isPairingMode && reason == BLE_PAIRING_CONFLICT_REASON) {
        LOG_INFO("BleCallbackHandler", "Pairing conflict detected - host tried encrypted reconnect after bond clearing");

        // Stop advertising to prevent battery-draining reconnect loop
        if (bleKeyboard) {
            bleKeyboard->stopAdvertising();
        }
        hardwareState.bleState.isPairingMode = false;

        // Stop BT flash timer (pairing aborted due to conflict)
        if (btFlashTimer != nullptr) {
            xTimerStop(btFlashTimer, 0);
        }

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

    // Update global hardware state
    hardwareState.bleState.isConnected = false;
    hardwareState.bleState.isPairingMode = false;

    // Stop BT flash timer (if it was running)
    if (btFlashTimer != nullptr) {
        xTimerStop(btFlashTimer, 0);
    }

    // Normal disconnect
    DisplayRequest req;
    req.type = DisplayRequestType::SHOW_STATUS;
    req.data.status.key = "BLE";
    req.data.status.value = "Disconnected";

    if (xQueueSend(displayQueue, &req, pdMS_TO_TICKS(10)) != pdPASS) {
        LOG_ERROR("BleCallbackHandler", "Failed to send disconnect display request");
    }

    // Update status screen with new BT state
    DisplayRequest normalModeReq;
    normalModeReq.type = DisplayRequestType::DRAW_NORMAL_MODE;
    normalModeReq.data.normalMode.state = hardwareState;
    xQueueSend(displayQueue, &normalModeReq, pdMS_TO_TICKS(10));
}

} // namespace BleCallbackHandler
