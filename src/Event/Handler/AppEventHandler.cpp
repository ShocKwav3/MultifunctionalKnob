#include "AppEventHandler.h"

AppEventHandler::AppEventHandler(QueueHandle_t queue)
    : eventQueue(queue) {}

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
            Serial.println(EnumToStringHelper::toString(evt.type));

            // maybe replace this block with a consumer?
            switch (evt.type) {
                case EventEnum::EncoderModeAppEventTypes::ENCODER_MODE_SELECTION:
                    //enters mode selection
                    break;
                case EventEnum::EncoderModeAppEventTypes::ENCODER_MODE_SELECTION_CANCELLED:
                    //cancel mode selection
                    break;
                default:
                    if (static_cast<int>(evt.type) < static_cast<int>(EventEnum::EncoderModeAppEventTypes::__ENCODER_MODE_SELECTION_LIMIT)) {
                    //set mode
                    }
                    break;
            }
        }
    }
}
