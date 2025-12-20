#include "EncoderEventHandler.h"
#include "Menu/Controller/MenuController.h"
#include "Config/log_config.h"

EncoderEventHandler::EncoderEventHandler(QueueHandle_t queue)
    : eventQueue(queue) {}

void EncoderEventHandler::setModeHandler(EncoderModeBaseInterface* handler) {
    if (!handler) {
        LOG_ERROR("EncoderEventHandler", "Invalid mode handler");
        return;
    }

    LOG_INFO("EncoderEventHandler", "Setting mode handler: %s", handler->getModeName());

    currentHandler = handler;
}

void EncoderEventHandler::setMenuController(MenuController* controller) {
    menuController = controller;
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
            // Menu intercepts events when active
            if (menuController && menuController->isActive()) {
                switch (evt.type) {
                    case EventEnum::EncoderInputEventTypes::ROTATE:
                        menuController->handleRotation(evt.delta);
                        break;
                    case EventEnum::EncoderInputEventTypes::SHORT_CLICK:
                        menuController->handleSelect();
                        break;
                    case EventEnum::EncoderInputEventTypes::LONG_CLICK:
                        menuController->handleBack();
                        break;
                }
                continue;  // Event consumed by menu
            }

            // Long-press activates menu when inactive
            if (menuController && evt.type == EventEnum::EncoderInputEventTypes::LONG_CLICK) {
                menuController->activate();
                continue;  // Event consumed
            }

            // Normal mode handling
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
