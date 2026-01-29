#include "EncoderEventHandler.h"
#include "Menu/Controller/MenuController.h"
#include "Config/log_config.h"
#include "System/PowerManager.h"
#include "Macro/Manager/MacroManager.h"
#include "state/HardwareState.h"
#include "Enum/MacroInputEnum.h"

EncoderEventHandler::EncoderEventHandler(QueueHandle_t queue, PowerManager* pm, HardwareState* hwState, MacroManager* macroMgr)
    : eventQueue(queue), powerManager(pm), hardwareState(hwState), macroManager(macroMgr) {
    // Validate dependencies
    if (!hwState || !macroMgr) {
        LOG_ERROR("EncoderEventHandler", "Constructor called with null dependencies");
    }
}

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

void EncoderEventHandler::notifyUserActivity() {
    if (powerManager) {
        powerManager->resetActivity();
    }
}

void EncoderEventHandler::taskEntry(void* param) {
    static_cast<EncoderEventHandler*>(param)->taskLoop();
}

void EncoderEventHandler::taskLoop() {
    EncoderInputEvent evt;

    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY)) {
            // Interface-enforced user activity notification
            notifyUserActivity();

            // Priority 1: Menu intercepts events when active
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

            // Priority 2: Try macro execution if macro mode active
            if (hardwareState->macroModeActive) {
                MacroInput input = static_cast<MacroInput>(mapEncoderEventToMacroInput(evt));
                if (macroManager->executeMacro(input)) {
                    LOG_INFO("EncoderEventHandler", "Macro executed for input");
                    continue;
                }
                LOG_INFO("EncoderEventHandler", "No macro assigned, falling through");
            }

            // Priority 3: Normal mode handling
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

uint8_t EncoderEventHandler::mapEncoderEventToMacroInput(const EncoderInputEvent& evt) {
    if (evt.type == EventEnum::EncoderInputEventTypes::ROTATE && evt.delta < 0) {
        return static_cast<uint8_t>(MacroInput::WHEEL_LEFT);
    }

    if (evt.type == EventEnum::EncoderInputEventTypes::ROTATE && evt.delta >= 0) {
        return static_cast<uint8_t>(MacroInput::WHEEL_RIGHT);
    }

    if (evt.type == EventEnum::EncoderInputEventTypes::SHORT_CLICK) {
        return static_cast<uint8_t>(MacroInput::WHEEL_BUTTON);
    }

    // Default fallback for LONG_CLICK or unknown types
    return static_cast<uint8_t>(MacroInput::WHEEL_BUTTON);
}
