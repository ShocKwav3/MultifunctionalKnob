#include "EncoderEventHandler.h"

EncoderEventHandler::EncoderEventHandler(QueueHandle_t queue)
    : eventQueue(queue) {}

void EncoderEventHandler::setModeHandler(EncoderModeBaseInterface* handler) {
    if (!handler) {
        Serial.println("EncoderEventHandler: Invalid mode handler");
        return;
    }

    Serial.printf("EncoderEventHandler: Setting mode handler: %s\n", handler->getModeName());

    currentHandler = handler;
}

void EncoderEventHandler::start() {
    xTaskCreate(taskEntry, "EncoderEventTask", 4096, this, 1, nullptr);
}

void EncoderEventHandler::taskEntry(void* param) {
    static_cast<EncoderEventHandler*>(param)->taskLoop();
}

void EncoderEventHandler::taskLoop() {
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
