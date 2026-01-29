#include "ButtonDriver.h"
#include "Config/button_config.h"
#include "Config/log_config.h"
#include "driver/gpio.h"

ButtonDriver* ButtonDriver::instance = nullptr;

ButtonDriver::ButtonDriver() : shortPressCallbacks{}, longPressCallbacks{}, wasButtonDown{}, lastTimeButtonDown{} {}

ButtonDriver* ButtonDriver::getInstance() {
    if (!instance) {
        instance = new ButtonDriver();
    }
    return instance;
}

void ButtonDriver::begin() {
    // Initialize GPIO pins with explicit pull-up configuration
    for (size_t i = 0; i < BUTTON_COUNT; i++) {
        uint8_t pin = BUTTONS[i].pin;
        pinMode(pin, INPUT);
        if (BUTTONS[i].activeLow) {
            // Explicitly enable internal pull-up using ESP-IDF
            gpio_pullup_en((gpio_num_t)pin);
            gpio_pulldown_dis((gpio_num_t)pin);
        } else {
            gpio_pulldown_en((gpio_num_t)pin);
            gpio_pullup_dis((gpio_num_t)pin);
        }
    }

    // Longer delay to let pull-ups charge any parasitic capacitance
    delay(100);

    // Initialize state to ACTUAL current reading (prevents false press at boot)
    unsigned long now = millis();
    for (size_t i = 0; i < BUTTON_COUNT; i++) {
        wasButtonDown[i] = isButtonDown(i);
        // If button is DOWN at boot, set start time so we don't get bogus duration
        lastTimeButtonDown[i] = wasButtonDown[i] ? now : 0;
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

// Mirrors EncoderDriver::handleButton() pattern exactly
void ButtonDriver::handleButton(uint8_t index) {
    bool isDown = isButtonDown(index);

    if (isDown) {
        if (!wasButtonDown[index]) {
            // First detection of button down - record start time
            lastTimeButtonDown[index] = millis();
        }
        wasButtonDown[index] = true;
        return;
    }

    // Button is UP
    if (wasButtonDown[index]) {
        // Was down, now up = released - calculate duration and dispatch
        unsigned long pressDuration = millis() - lastTimeButtonDown[index];

        if (pressDuration >= BUTTON_LONG_PRESS_MIN_MS) {
            if (longPressCallbacks[index]) {
                longPressCallbacks[index]();
            }
        } else if (pressDuration >= BUTTON_SHORT_PRESS_MIN_MS) {
            if (shortPressCallbacks[index]) {
                shortPressCallbacks[index]();
            }
        }
        // Ignore presses shorter than BUTTON_SHORT_PRESS_MIN_MS (noise)
    }

    wasButtonDown[index] = false;
}

bool ButtonDriver::isButtonDown(uint8_t index) {
    int pinState = digitalRead(BUTTONS[index].pin);
    return BUTTONS[index].activeLow ? (pinState == LOW) : (pinState == HIGH);
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
