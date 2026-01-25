#include "ButtonEventHandler.h"
#include "Config/log_config.h"
#include "Config/ConfigManager.h"
#include "Config/button_config.h"
#include "BLE/BleKeyboardService.h"
#include "System/PowerManager.h"

ButtonEventHandler::ButtonEventHandler(QueueHandle_t queue, ConfigManager* config, BleKeyboardService* bleService, PowerManager* pm)
    : eventQueue(queue)
    , configManager(config)
    , bleKeyboardService(bleService)
    , powerManager(pm)
    , cacheValid(false) {
    // Dependencies are required - validate they are not null
    if (!config || !bleService) {
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

            switch (evt.type) {
                case EventEnum::ButtonEventTypes::BUTTON_PRESSED:
                    LOG_DEBUG("ButtonEventHandler", "Button %d pressed", evt.buttonIndex);
                    executeButtonAction(evt.buttonIndex);
                    break;
                case EventEnum::ButtonEventTypes::BUTTON_RELEASED:
                    LOG_DEBUG("ButtonEventHandler", "Button %d released", evt.buttonIndex);
                    break;
            }
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
