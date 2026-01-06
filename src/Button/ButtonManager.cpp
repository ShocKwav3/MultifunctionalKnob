#include "ButtonManager.h"
#include "Event/Dispatcher/ButtonEventDispatcher.h"
#include "Arduino.h"

// ButtonManager uses FreeRTOS task-based polling (Story 6.3).
// Decision rationale:
// - Consistent with encoder button pattern (both use task-based polling)
// - Predictable 10ms polling interval independent of main loop
// - Simple implementation (no ISR complexity)
// - Clean separation from main loop

ButtonManager::ButtonManager(ButtonEventDispatcher* dispatcher)
    : eventDispatcher(dispatcher), buttonStates{} {}

void ButtonManager::init() {
    for (size_t i = 0; i < BUTTON_COUNT; i++) {
        pinMode(BUTTONS[i].pin, BUTTONS[i].activeLow ? INPUT_PULLUP : INPUT_PULLDOWN);
        buttonStates[i] = {false, false, 0};
    }

    // Create dedicated FreeRTOS task for button polling
    BaseType_t taskCreated = xTaskCreate(
        buttonTask,           // Task function
        "ButtonTask",         // Name
        2048,                 // Stack size (match encoder pattern)
        this,                 // Pass ButtonManager instance
        1,                    // Priority (same as encoder)
        nullptr              // Task handle (not needed)
    );
    
    if (taskCreated != pdPASS) {
        Serial.println("ERROR: Failed to create ButtonTask - insufficient memory");
        // Task creation failed - buttons will not work
    }
}

void ButtonManager::buttonTask(void* pvParameters) {
    ButtonManager* manager = (ButtonManager*)pvParameters;
    while (true) {
        manager->poll();      // Existing poll logic unchanged
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms interval (match encoder)
    }
}

void ButtonManager::poll() {
    if (!eventDispatcher) return;

    uint32_t currentTime = millis();

    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        bool reading = readButton(i);

        // Detect change - restart debounce timer
        if (reading != buttonStates[i].lastReading) {
            buttonStates[i].lastChangeTime = currentTime;
            buttonStates[i].lastReading = reading;
            continue;
        }

        // Not debounced yet - skip
        if (!isDebounced(i, currentTime)) continue;

        // State changed after debounce - call dispatcher
        if (reading != buttonStates[i].pressed) {
            buttonStates[i].pressed = reading;
            if (reading) {
                eventDispatcher->onButtonPressed(i);
            } else {
                eventDispatcher->onButtonReleased(i);
            }
        }
    }
}

bool ButtonManager::readButton(uint8_t index) {
    int pinState = digitalRead(BUTTONS[index].pin);
    return BUTTONS[index].activeLow ? (pinState == LOW) : (pinState == HIGH);
}

bool ButtonManager::isDebounced(uint8_t index, uint32_t currentTime) {
    return (currentTime - buttonStates[index].lastChangeTime) >= DEBOUNCE_MS;
}
