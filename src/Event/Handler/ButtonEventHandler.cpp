#include "ButtonEventHandler.h"
#include "Config/log_config.h"

ButtonEventHandler::ButtonEventHandler(QueueHandle_t queue)
    : eventQueue(queue) {}

void ButtonEventHandler::start() {
    xTaskCreate(taskEntry, "ButtonEventTask", 4096, this, 1, nullptr);
}

void ButtonEventHandler::taskEntry(void* param) {
    static_cast<ButtonEventHandler*>(param)->taskLoop();
}

void ButtonEventHandler::taskLoop() {
    ButtonEvent evt;

    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY)) {
            // For MVP: just log button events
            // Future stories will add actual button action handling
            switch (evt.type) {
                case EventEnum::ButtonEventTypes::BUTTON_PRESSED:
                    LOG_INFO("ButtonEventHandler", "Button %d pressed", evt.buttonIndex);
                    break;
                case EventEnum::ButtonEventTypes::BUTTON_RELEASED:
                    LOG_INFO("ButtonEventHandler", "Button %d released", evt.buttonIndex);
                    break;
            }
        }
    }
}
