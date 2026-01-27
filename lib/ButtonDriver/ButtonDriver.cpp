#include "ButtonDriver.h"
#include "Config/button_config.h"

ButtonDriver* ButtonDriver::instance = nullptr;

ButtonDriver::ButtonDriver() : buttonStates{}, shortPressCallbacks{}, longPressCallbacks{} {}

ButtonDriver* ButtonDriver::getInstance() {
    if (!instance) {
        instance = new ButtonDriver();
    }
    return instance;
}

void ButtonDriver::begin() {
    // Initialize GPIO pins
    for (size_t i = 0; i < BUTTON_COUNT; i++) {
        pinMode(BUTTONS[i].pin, BUTTONS[i].activeLow ? INPUT_PULLUP : INPUT_PULLDOWN);
        buttonStates[i] = {false, false, 0, 0};
    }

    // Create FreeRTOS task for button polling
    BaseType_t taskCreated = xTaskCreate(
        buttonTask,           // Task function
        "ButtonDriverTask",   // Name
        2048,                 // Stack size
        nullptr,              // Parameters
        1,                    // Priority
        nullptr              // Task handle
    );

    if (taskCreated != pdPASS) {
        Serial.println("ERROR: Failed to create ButtonDriverTask - insufficient memory");
    }
}

void ButtonDriver::buttonTask(void* pvParameters) {
    while (true) {
        if (instance) {
            instance->runLoop();
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms polling interval
    }
}

void ButtonDriver::runLoop() {
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        handleButton(i);
    }
}

void ButtonDriver::handleButton(uint8_t index) {
    uint32_t currentTime = millis();
    bool reading = readButton(index);

    // Detect change - restart debounce timer
    if (reading != buttonStates[index].lastReading) {
        buttonStates[index].lastChangeTime = currentTime;
        buttonStates[index].lastReading = reading;
        return;
    }

    // Not debounced yet - skip
    if (!isDebounced(index, currentTime)) {
        return;
    }

    // State changed after debounce
    if (reading != buttonStates[index].pressed) {
        if (reading) {
            // Button pressed - record start time
            buttonStates[index].pressStartTime = currentTime;
        } else {
            // Button released - calculate duration and dispatch
            uint32_t duration = currentTime - buttonStates[index].pressStartTime;

            if (duration >= BUTTON_LONG_PRESS_MIN_MS) {
                // Long press
                if (longPressCallbacks[index]) {
                    longPressCallbacks[index]();
                }
            } else if (duration >= BUTTON_SHORT_PRESS_MIN_MS) {
                // Short press
                if (shortPressCallbacks[index]) {
                    shortPressCallbacks[index]();
                }
            }
            // Ignore presses shorter than BUTTON_SHORT_PRESS_MIN_MS (noise/bounce)
        }

        buttonStates[index].pressed = reading;
    }
}

bool ButtonDriver::readButton(uint8_t index) {
    int pinState = digitalRead(BUTTONS[index].pin);
    return BUTTONS[index].activeLow ? (pinState == LOW) : (pinState == HIGH);
}

bool ButtonDriver::isDebounced(uint8_t index, uint32_t currentTime) {
    return (currentTime - buttonStates[index].lastChangeTime) >= DEBOUNCE_MS;
}

void ButtonDriver::setOnShortPress(uint8_t buttonIndex, std::function<void()> callback) {
    if (buttonIndex < BUTTON_COUNT) {
        shortPressCallbacks[buttonIndex] = callback;
    }
}

void ButtonDriver::setOnLongPress(uint8_t buttonIndex, std::function<void()> callback) {
    if (buttonIndex < BUTTON_COUNT) {
        longPressCallbacks[buttonIndex] = callback;
    }
}
