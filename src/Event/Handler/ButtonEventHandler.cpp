#include "ButtonEventHandler.h"
#include "Config/log_config.h"
#include "Config/ConfigManager.h"
#include "Config/button_config.h"
#include "BLE/BleKeyboardService.h"
#include "System/PowerManager.h"
#include "Macro/Manager/MacroManager.h"
#include "state/HardwareState.h"
#include "Enum/MacroInputEnum.h"

ButtonEventHandler::ButtonEventHandler(QueueHandle_t queue, ConfigManager* config, BleKeyboardService* bleService, PowerManager* pm, HardwareState* hwState, MacroManager* macroMgr)
    : eventQueue(queue)
    , configManager(config)
    , bleKeyboardService(bleService)
    , powerManager(pm)
    , hardwareState(hwState)
    , macroManager(macroMgr)
    , cacheValid(false) {
    // Dependencies are required - validate they are not null
    if (!config || !bleService || !hwState || !macroMgr) {
        LOG_ERROR("ButtonEventHandler", "Constructor called with null dependencies");
    }

    // Initialize cache to NONE (safe default - ID 0)
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        actionCache[i] = 0;  // ID 0 = NONE action
    }
}

void ButtonEventHandler::start() {
    // Defense-in-depth: Validate all dependencies before starting FreeRTOS task
    if (!eventQueue) {
        LOG_ERROR("ButtonEventHandler", "Cannot start task: eventQueue is null");
        return;
    }

    if (!configManager) {
        LOG_ERROR("ButtonEventHandler", "Cannot start task: configManager is null");
        return;
    }

    if (!bleKeyboardService) {
        LOG_ERROR("ButtonEventHandler", "Cannot start task: bleKeyboardService is null");
        return;
    }

    if (!hardwareState) {
        LOG_ERROR("ButtonEventHandler", "Cannot start task: hardwareState is null");
        return;
    }

    if (!macroManager) {
        LOG_ERROR("ButtonEventHandler", "Cannot start task: macroManager is null");
        return;
    }

    xTaskCreate(taskEntry, "ButtonEventTask", 4096, this, 1, nullptr);
    LOG_INFO("ButtonEventHandler", "Task started successfully");
}

void ButtonEventHandler::invalidateCache() {
    cacheValid = false;
    LOG_DEBUG("ButtonEventHandler", "Button action cache invalidated");
}

void ButtonEventHandler::loadCache() {
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        actionCache[i] = configManager->loadButtonAction(i);
    }
    cacheValid = true;
    LOG_INFO("ButtonEventHandler", "Button action cache loaded from NVS");
}

void ButtonEventHandler::notifyUserActivity() {
    if (powerManager) {
        powerManager->resetActivity();
    }
}

void ButtonEventHandler::taskEntry(void* param) {
    static_cast<ButtonEventHandler*>(param)->taskLoop();
}

void ButtonEventHandler::taskLoop() {
    ButtonEvent evt;

    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY)) {
            // Interface-enforced user activity notification
            notifyUserActivity();

            // Priority 1: Macro button long press toggles macro mode
            if (evt.buttonIndex == MACRO_BUTTON_INDEX && evt.type == EventEnum::ButtonEventTypes::LONG_PRESS) {
                macroManager->toggleMacroMode();
                hardwareState->macroModeActive = macroManager->isMacroModeActive();
                LOG_INFO("ButtonEventHandler", "Macro mode toggled to %s",
                         hardwareState->macroModeActive ? "ON" : "OFF");
                continue;
            }

            // Ignore short press on macro button
            if (evt.buttonIndex == MACRO_BUTTON_INDEX && evt.type == EventEnum::ButtonEventTypes::SHORT_PRESS) {
                LOG_DEBUG("ButtonEventHandler", "Macro button short press ignored");
                continue;
            }

            // Only process short press events for regular buttons
            if (evt.type != EventEnum::ButtonEventTypes::SHORT_PRESS) {
                LOG_DEBUG("ButtonEventHandler", "Button %d long press (no action)", evt.buttonIndex);
                continue;
            }

            // Only process regular buttons when NOT in macro button context
            if (evt.buttonIndex == MACRO_BUTTON_INDEX) {
                continue;
            }

            LOG_DEBUG("ButtonEventHandler", "Button %d short press", evt.buttonIndex);

            // Priority 2: Try macro execution if macro mode active
            if (hardwareState->macroModeActive) {
                MacroInput input = static_cast<MacroInput>(mapButtonIndexToMacroInput(evt.buttonIndex));
                if (macroManager->executeMacro(input)) {
                    LOG_DEBUG("ButtonEventHandler", "Macro executed for button %d", evt.buttonIndex);
                    continue;
                }
                LOG_DEBUG("ButtonEventHandler", "No macro assigned for button %d, falling through", evt.buttonIndex);
            }

            // Priority 3: Normal button action
            executeButtonAction(evt.buttonIndex);
        }
    }
}

void ButtonEventHandler::executeButtonAction(uint8_t buttonIndex) {
    // Load cache if invalid (first press or after config change)
    if (!cacheValid) {
        loadCache();
    }

    // Get action ID from RAM cache (fast: <100ns vs ~1-5ms NVS read)
    ButtonActionId actionId = actionCache[buttonIndex];

    // Execute via service - service handles connection check, validation, and execution
    if (!bleKeyboardService->executeMediaKey(actionId)) {
        LOG_DEBUG("ButtonEventHandler", "Button %d: Action %d execution failed or skipped", buttonIndex, actionId);
    }
}

uint8_t ButtonEventHandler::mapButtonIndexToMacroInput(uint8_t buttonIndex) {
    switch (buttonIndex) {
        case 0:
            return static_cast<uint8_t>(MacroInput::BUTTON_1);
        case 1:
            return static_cast<uint8_t>(MacroInput::BUTTON_2);
        case 2:
            return static_cast<uint8_t>(MacroInput::BUTTON_3);
        case 3:
            return static_cast<uint8_t>(MacroInput::BUTTON_4);
        default:
            // Default to BUTTON_1 for safety
            return static_cast<uint8_t>(MacroInput::BUTTON_1);
    }
}
