#include "DisconnectAction.h"
#include "BleKeyboard.h"
#include "Display/Model/DisplayRequest.h"
#include "Config/log_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

DisconnectAction::DisconnectAction(BleKeyboard* ble, QueueHandle_t displayQueue)
    : bleKeyboard(ble), displayRequestQueue(displayQueue) {}

void DisconnectAction::execute(const MenuItem* context) {
    DisplayRequest req;
    req.type = DisplayRequestType::SHOW_STATUS;
    req.data.status.key = "Status";

    if (bleKeyboard->isConnected()) {
        LOG_INFO("DisconnectAction", "Disconnecting BLE");
        bleKeyboard->disconnect();
        req.data.status.value = "Disconnected";
    } else {
        LOG_INFO("DisconnectAction", "Already disconnected");
        req.data.status.value = "Not connected";
    }

    if (xQueueSend(displayRequestQueue, &req, pdMS_TO_TICKS(10)) != pdPASS) {
        LOG_ERROR("DisconnectAction", "Failed to send display request");
    }
}

const char* DisconnectAction::getConfirmationMessage() {
    return nullptr; // Feedback via display request
}
