#include "ButtonEventDispatcher.h"
#include "Config/log_config.h"

ButtonEventDispatcher::ButtonEventDispatcher(QueueHandle_t queue)
    : eventQueue(queue) {}

void ButtonEventDispatcher::onButtonPressed(uint8_t buttonIndex) {
    if (eventQueue) {
        ButtonEvent evt{ EventEnum::ButtonEventTypes::BUTTON_PRESSED, buttonIndex };
        if (xQueueSend(eventQueue, &evt, pdMS_TO_TICKS(10)) != pdPASS) {
            LOG_ERROR("ButtonEventDispatcher", "Failed to send BUTTON_PRESSED event to queue");
        }
    }
}

void ButtonEventDispatcher::onButtonReleased(uint8_t buttonIndex) {
    if (eventQueue) {
        ButtonEvent evt{ EventEnum::ButtonEventTypes::BUTTON_RELEASED, buttonIndex };
        if (xQueueSend(eventQueue, &evt, pdMS_TO_TICKS(10)) != pdPASS) {
            LOG_ERROR("ButtonEventDispatcher", "Failed to send BUTTON_RELEASED event to queue");
        }
    }
}
