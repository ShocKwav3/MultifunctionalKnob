#include "ButtonEventHandler.h"
#include "Config/log_config.h"
#include "Config/ConfigManager.h"
#include "Config/button_config.h"
#include "Enum/ButtonActionEnum.h"

ButtonEventHandler::ButtonEventHandler(QueueHandle_t queue, ConfigManager* config, BleKeyboard* keyboard)
    : eventQueue(queue)
    , configManager(config)
    , bleKeyboard(keyboard)
    , cacheValid(false) {
    // Dependencies are required - validate they are not null
    if (!config || !keyboard) {
        LOG_ERROR("ButtonEventHandler", "Constructor called with null dependencies");
    }
    
    // Initialize cache to NONE (safe default)
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        actionCache[i] = ButtonAction::NONE;
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
    
    if (!bleKeyboard) {
        LOG_ERROR("ButtonEventHandler", "Cannot start task: bleKeyboard is null");
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

void ButtonEventHandler::taskEntry(void* param) {
    static_cast<ButtonEventHandler*>(param)->taskLoop();
}

void ButtonEventHandler::taskLoop() {
    ButtonEvent evt;

    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY)) {
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
    
    // Get action from RAM cache (fast: <100ns vs ~1-5ms NVS read)
    ButtonAction action = actionCache[buttonIndex];

    // Execute the action if BLE is connected
    if (!bleKeyboard->isConnected()) {
        LOG_DEBUG("ButtonEventHandler", "Button %d: BLE not connected, skipping action", buttonIndex);
        return;
    }

    // Map ButtonAction to HID command
    switch (action) {
        case ButtonAction::MUTE:
            bleKeyboard->write(KEY_MEDIA_MUTE);
            LOG_INFO("ButtonEventHandler", "Button %d: Mute", buttonIndex);
            break;
        case ButtonAction::PLAY:
        case ButtonAction::PAUSE:
            bleKeyboard->write(KEY_MEDIA_PLAY_PAUSE);
            LOG_INFO("ButtonEventHandler", "Button %d: Play/Pause", buttonIndex);
            break;
        case ButtonAction::NEXT:
            bleKeyboard->write(KEY_MEDIA_NEXT_TRACK);
            LOG_INFO("ButtonEventHandler", "Button %d: Next Track", buttonIndex);
            break;
        case ButtonAction::PREVIOUS:
            bleKeyboard->write(KEY_MEDIA_PREVIOUS_TRACK);
            LOG_INFO("ButtonEventHandler", "Button %d: Previous Track", buttonIndex);
            break;
        case ButtonAction::NONE:
            LOG_DEBUG("ButtonEventHandler", "Button %d: No action assigned", buttonIndex);
            break;
        default:
            LOG_ERROR("ButtonEventHandler", "Button %d: Unknown action %d", buttonIndex, static_cast<int>(action));
            break;
    }
}
