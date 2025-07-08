#include "RotaryEncoderDriver.h"

RotaryEncoderDriver* RotaryEncoderDriver::rotaryEncoderDriverInstance = nullptr;
AiEsp32RotaryEncoder* RotaryEncoderDriver::rotaryEncoderInstance = nullptr;

RotaryEncoderDriver::RotaryEncoderDriver(
    uint8_t clkPin,
    uint8_t dtPin,
    int swPin,
    int vccPin,
    uint8_t steps
) : clkPin(clkPin), dtPin(dtPin), swPin(swPin), vccPin(vccPin), steps(steps) {}

RotaryEncoderDriver* RotaryEncoderDriver::getInstance(
    uint8_t clkPin,
    uint8_t dtPin,
    int swPin,
    int vccPin,
    uint8_t steps
) {
    if (!rotaryEncoderDriverInstance) {
        rotaryEncoderDriverInstance = new RotaryEncoderDriver(clkPin, dtPin, swPin, vccPin, steps);
    }
    return rotaryEncoderDriverInstance;
}

void RotaryEncoderDriver::begin() {
    if (!rotaryEncoderInstance) {
        rotaryEncoderInstance = new AiEsp32RotaryEncoder(clkPin, dtPin, swPin, vccPin, steps);
    }

    rotaryEncoderInstance->begin();
    rotaryEncoderInstance->setup(readEncoderISR);
    rotaryEncoderInstance->setBoundaries(0, 1000, false);
    rotaryEncoderInstance->setAcceleration(250);

    xTaskCreate(encoderTask, "RotaryEncoderTask", 4096, nullptr, 1, nullptr);
}

void IRAM_ATTR RotaryEncoderDriver::readEncoderISR() {
    if (rotaryEncoderInstance) {
        rotaryEncoderInstance->readEncoder_ISR();
    }
}

void RotaryEncoderDriver::encoderTask(void* pvParameters) {
    while (true) {
        if (rotaryEncoderDriverInstance) {
            rotaryEncoderDriverInstance->runLoop();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void RotaryEncoderDriver::runLoop() {
    static int32_t lastValue = 0;
    int32_t current = rotaryEncoderInstance->readEncoder();

    if (current != lastValue) {
        if (onValueChangeCallback) {
            onValueChangeCallback(current);
        }
        lastValue = current;
    }
    handleButton();
}

void RotaryEncoderDriver::handleButton() {
    static unsigned long lastTimeButtonDown = 0;
    static bool wasButtonDown = false;
    const unsigned long shortPressAfterMs = 50;
    const unsigned long longPressAfterMs = 1000;

    bool isDown = rotaryEncoderInstance->isEncoderButtonDown();

    if (isDown) {
        if (!wasButtonDown) {
            lastTimeButtonDown = millis();
        }
        wasButtonDown = true;
        return;
    }

    if (wasButtonDown) {
        unsigned long pressDuration = millis() - lastTimeButtonDown;
        if (pressDuration >= longPressAfterMs) {
            onLongClick();
        } else if (pressDuration >= shortPressAfterMs) {
            onShortClick();
        }
    }

    wasButtonDown = false;
}

void RotaryEncoderDriver::onShortClick() {
    if (onShortClickCallback) {
        onShortClickCallback();
    }
}

void RotaryEncoderDriver::onLongClick() {
    if (onLongClickCallback) {
        onLongClickCallback();
    }
}

void RotaryEncoderDriver::setOnShortClick(std::function<void()> callback) {
    onShortClickCallback = callback;
}

void RotaryEncoderDriver::setOnLongClick(std::function<void()> callback) {
    onLongClickCallback = callback;
}

void RotaryEncoderDriver::setOnValueChange(std::function<void(int32_t)> callback) {
    onValueChangeCallback = callback;
}
