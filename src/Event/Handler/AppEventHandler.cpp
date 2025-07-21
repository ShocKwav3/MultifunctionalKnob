#include "AppEventHandler.h"

AppEventHandler::AppEventHandler(QueueHandle_t queue, ModeManager* modeManager)
    : eventQueue(queue), modeManager(modeManager) {}

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
            Serial.println(ModeHelper::toString(evt.type));

            if (evt.type == EventEnum::AppEventTypes::MODE_SELECTION) {
                Serial.print("AppEvent mode selection");
                modeManager->enterModeSelection();
                //Serial.println(static_cast<int>(evt.type));
            }
            else if (evt.type == EventEnum::AppEventTypes::MODE_SELECTION_CANCELLED) {
                Serial.print("AppEvent mode selection cancelled");
                modeManager->cancelModeSelection();
                //Serial.println(static_cast<int>(evt.type));
            }
            else if (static_cast<int>(evt.type) < static_cast<int>(EventEnum::AppEventTypes::__MODE_SELECTION_LIMIT)) {
                Serial.print("AppEvent mode selection limit: ");
                modeManager->setMode(evt.type);  // e.g., SCROLL, VOLUME, etc.
                //Serial.println(static_cast<int>(evt.type));
            }
        }
    }
}
