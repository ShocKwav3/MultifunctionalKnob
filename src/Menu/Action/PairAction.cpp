#include "PairAction.h"
#include "BleKeyboard.h"
#include "Display/Model/DisplayRequest.h"
#include "Config/log_config.h"
#include "state/HardwareState.h"
#include "state/AppState.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern HardwareState hardwareState;
extern AppState appState;

PairAction::PairAction(BleKeyboard* ble, QueueHandle_t displayQueue)
    : bleKeyboard(ble), displayRequestQueue(displayQueue) {}

void PairAction::execute(const MenuItem* context) {
    LOG_INFO("PairAction", "Starting BLE pairing");

    // Disconnect if currently connected
    if (bleKeyboard->isConnected()) {
        LOG_INFO("PairAction", "Disconnecting before pairing");
        bleKeyboard->disconnect();
        delay(100); // Brief delay for disconnect to process
    }

    // Clear bonding info to prevent auto-reconnect from old device
    bleKeyboard->clearBonds();
    LOG_INFO("PairAction", "Bonding info cleared");
    delay(100); // Allow bond clearing to persist to NVS

    // Update global hardware state for pairing
    // This allows onDisconnect callback to detect auto-reconnect conflicts (reason 531)
    hardwareState.bleState.isPairingMode = true;
    hardwareState.bleState.isConnected = false;
    LOG_INFO("PairAction", "Pairing mode flag set");

    // Start BT flash timer for pairing animation
    if (appState.btFlashTimer != nullptr) {
        xTimerStart(appState.btFlashTimer, 0);
        LOG_INFO("PairAction", "BT flash timer started");
    }

    // Start advertising for pairing (NOT begin() - that's for initialization)
    bleKeyboard->startAdvertising();
    LOG_INFO("PairAction", "BLE advertising started");

    // Send display feedback - use SHOW_STATUS for clarity
    DisplayRequest req;
    req.type = DisplayRequestType::SHOW_STATUS;
    req.data.status.key = "BLE";
    req.data.status.value = "Pairing...";

    if (xQueueSend(displayRequestQueue, &req, pdMS_TO_TICKS(10)) != pdPASS) {
        LOG_ERROR("PairAction", "Failed to send display request");
    }

    // Update status screen to show pairing state (BT icon will flash)
    DisplayRequest normalModeReq;
    normalModeReq.type = DisplayRequestType::DRAW_NORMAL_MODE;
    normalModeReq.data.normalMode.hwState = hardwareState;
    xQueueSend(displayRequestQueue, &normalModeReq, pdMS_TO_TICKS(10));
}

const char* PairAction::getConfirmationMessage() {
    return nullptr; // Async action - feedback via BLE callbacks
}
