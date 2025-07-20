#include "RotaryEncoderEventHandler.h"

RotaryEncoderEventHandler::RotaryEncoderEventHandler(QueueHandle_t queue)
    : eventQueue(queue) {}

void RotaryEncoderEventHandler::setModeHandler(ModeHandlerInterface* handler) {
    if (!handler) {
        Serial.println("RotaryEncoderEventHandler: Invalid mode handler");
        return;
    }

    Serial.printf("RotaryEncoderEventHandler: Setting mode handler: %s\n", handler->getModeName());

    currentHandler = handler;
}

void RotaryEncoderEventHandler::start() {
    xTaskCreate(taskEntry, "RotaryEventTask", 4096, this, 1, nullptr);
}

void RotaryEncoderEventHandler::taskEntry(void* param) {
    static_cast<RotaryEncoderEventHandler*>(param)->taskLoop();
}

void RotaryEncoderEventHandler::taskLoop() {
    EncoderInputEvent evt;

    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY)) {
            if (!currentHandler) continue;

            switch (evt.type) {
                case EventEnum::EncoderInputEventTypes::ROTATE:
                    currentHandler->handleRotate(evt.delta);
                    break;
                case EventEnum::EncoderInputEventTypes::SHORT_CLICK:
                    currentHandler->handleShortClick();
                    break;
                case EventEnum::EncoderInputEventTypes::LONG_CLICK:
                    currentHandler->handleLongClick();
                    break;
            }
        }
    }
}
