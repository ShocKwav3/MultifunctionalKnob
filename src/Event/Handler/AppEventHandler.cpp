#include "AppEventHandler.h"

AppEventHandler::AppEventHandler(QueueHandle_t queue, EncoderModeManager* encoderModeManager)
    : eventQueue(queue), encoderModeManager(encoderModeManager) {}

void AppEventHandler::start() {
    xTaskCreate(taskEntry, "AppEventHandlerTask", 4096, this, 1, nullptr);
}

void AppEventHandler::taskEntry(void* param) {
    static_cast<AppEventHandler*>(param)->taskLoop();
}

void AppEventHandler::taskLoop() {
    AppEvent evt;

    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY)) {
            Serial.print("AppEvent received: ");
            Serial.println(EncoderModeHelper::toString(evt.type));

            if (evt.type == EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION) {
                Serial.print("AppEvent mode selection");
                encoderModeManager->enterModeSelection();
                //Serial.println(static_cast<int>(evt.type));
            }
            else if (evt.type == EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION_CANCELLED) {
                Serial.print("AppEvent mode selection cancelled");
                encoderModeManager->cancelModeSelection();
                //Serial.println(static_cast<int>(evt.type));
            }
            else if (static_cast<int>(evt.type) < static_cast<int>(EventEnum::EncoderModeEventTypes::__ENCODER_MODE_SELECTION_LIMIT)) {
                Serial.print("AppEvent mode selection limit: ");
                encoderModeManager->setMode(evt.type);  // e.g., SCROLL, VOLUME, etc.
                //Serial.println(static_cast<int>(evt.type));
            }
        }
    }
}
