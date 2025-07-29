#include "EncoderInputEventHandler.h"

EncoderInputEventHandler::EncoderInputEventHandler(QueueHandle_t queue)
    : eventQueue(queue) {}

void EncoderInputEventHandler::start() {
    xTaskCreate(taskEntry, "EncoderEventTask", 4096, this, 1, nullptr);
}

void EncoderInputEventHandler::taskEntry(void* param) {
    static_cast<EncoderInputEventHandler*>(param)->taskLoop();
}

void EncoderInputEventHandler::taskLoop() {
    EncoderInputEvent evt;

    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY)) {
            Serial.print("Encoder input event received: ");
            Serial.println(EnumToStringHelper::toString(evt.type));

            //maybe replace this block with a consumer?
            switch (evt.type) {
                case EventEnum::EncoderInputEventTypes::ROTATE:
                    //do something with rotation
                    break;
                case EventEnum::EncoderInputEventTypes::SHORT_CLICK:
                    //do something with short click
                    break;
                case EventEnum::EncoderInputEventTypes::LONG_CLICK:
                    //do something with long click
                    break;
            }
        }
    }
}
