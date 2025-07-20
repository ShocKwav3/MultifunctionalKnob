#include "AppEventDispatcher.h"

AppEventDispatcher::AppEventDispatcher(QueueHandle_t queue)
    : appEventQueue(queue) {}

void AppEventDispatcher::dispatchAppEvent(ModeEnum::AppEventTypes type) {
    if (appEventQueue) {
        AppEvent event{ type };

        Serial.print("Dispatching AppEvent: ");
        Serial.println(ModeHelper::toString(type));

        xQueueSend(appEventQueue, &event, 0);
    }
}
