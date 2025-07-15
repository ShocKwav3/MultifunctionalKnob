#include "RotaryEncoderEventHandler.h"

RotaryEncoderEventHandler::RotaryEncoderEventHandler(QueueHandle_t queue)
    : eventQueue(queue) {}

void RotaryEncoderEventHandler::setModeHandler(ModeHandlerInterface* handler) {
    currentHandler = handler;
}

void RotaryEncoderEventHandler::start() {
    xTaskCreate(taskEntry, "RotaryEventTask", 4096, this, 1, nullptr);
}

void RotaryEncoderEventHandler::taskEntry(void* param) {
    static_cast<RotaryEncoderEventHandler*>(param)->taskLoop();
}

void RotaryEncoderEventHandler::taskLoop() {
    RotaryEncoderEventType evt;

    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY)) {
            if (!currentHandler) continue;

            switch (evt.type) {
                case EventEnum::RotaryEncoderEventTypes::ROTATE:
                    currentHandler->handleRotate(evt.delta);
                    break;
                case EventEnum::RotaryEncoderEventTypes::SHORT_CLICK:
                    currentHandler->handleShortClick();
                    break;
                case EventEnum::RotaryEncoderEventTypes::LONG_CLICK:
                    currentHandler->handleLongClick();
                    break;
            }
        }
    }
}
