#include "PairAction.h"
#include "BleKeyboard.h"
#include "Display/Model/DisplayRequest.h"
#include "Config/log_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

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
}

const char* PairAction::getConfirmationMessage() {
    return nullptr; // Async action - feedback via BLE callbacks
}
