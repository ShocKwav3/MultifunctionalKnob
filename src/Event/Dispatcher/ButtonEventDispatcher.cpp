#include "ButtonEventDispatcher.h"
#include "Config/log_config.h"

ButtonEventDispatcher::ButtonEventDispatcher(QueueHandle_t queue)
    : eventQueue(queue) {}

void ButtonEventDispatcher::onButtonShortPress(uint8_t buttonIndex) {
    if (eventQueue) {
        ButtonEvent evt{ EventEnum::ButtonEventTypes::SHORT_PRESS, buttonIndex };
        if (xQueueSend(eventQueue, &evt, pdMS_TO_TICKS(10)) != pdPASS) {
            LOG_ERROR("ButtonEventDispatcher", "Failed to send SHORT_PRESS event to queue");
        }
    }
}

void ButtonEventDispatcher::onButtonLongPress(uint8_t buttonIndex) {
    if (eventQueue) {
        ButtonEvent evt{ EventEnum::ButtonEventTypes::LONG_PRESS, buttonIndex };
        if (xQueueSend(eventQueue, &evt, pdMS_TO_TICKS(10)) != pdPASS) {
            LOG_ERROR("ButtonEventDispatcher", "Failed to send LONG_PRESS event to queue");
        }
    }
}
