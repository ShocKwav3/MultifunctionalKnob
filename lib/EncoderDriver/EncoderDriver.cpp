#include "EncoderDriver.h"

EncoderDriver* EncoderDriver::encoderDriverInstance = nullptr;
AiEsp32RotaryEncoder* EncoderDriver::encoderInstance = nullptr;

EncoderDriver::EncoderDriver(
    uint8_t clkPin,
    uint8_t dtPin,
    int swPin,
    int vccPin,
    uint8_t steps
) : clkPin(clkPin), dtPin(dtPin), swPin(swPin), vccPin(vccPin), steps(steps) {}

EncoderDriver* EncoderDriver::getInstance(
    uint8_t clkPin,
    uint8_t dtPin,
    int swPin,
    int vccPin,
    uint8_t steps
) {
    if (!encoderDriverInstance) {
        encoderDriverInstance = new EncoderDriver(clkPin, dtPin, swPin, vccPin, steps);
    }
    return encoderDriverInstance;
}

void EncoderDriver::begin() {
    if (!encoderInstance) {
        encoderInstance = new AiEsp32RotaryEncoder(clkPin, dtPin, swPin, vccPin, steps);
    }

    encoderInstance->begin();
    encoderInstance->setup(readEncoderISR);
    encoderInstance->setBoundaries(0, 1000, false);
    encoderInstance->setAcceleration(250);

    xTaskCreate(encoderTask, "RotaryEncoderTask", 4096, nullptr, 1, nullptr);
}

void IRAM_ATTR EncoderDriver::readEncoderISR() {
    if (encoderInstance) {
        encoderInstance->readEncoder_ISR();
    }
}

void EncoderDriver::encoderTask(void* pvParameters) {
    while (true) {
        if (encoderDriverInstance) {
            encoderDriverInstance->runLoop();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void EncoderDriver::runLoop() {
    static int32_t lastValue = 0;
    int32_t current = encoderInstance->readEncoder();

    if (current != lastValue) {
        if (onValueChangeCallback) {
            onValueChangeCallback(current);
        }
        lastValue = current;
    }
    handleButton();
}

void EncoderDriver::handleButton() {
    static unsigned long lastTimeButtonDown = 0;
    static bool wasButtonDown = false;
    const unsigned long shortPressAfterMs = 50;
    const unsigned long longPressAfterMs = 1000;

    bool isDown = encoderInstance->isEncoderButtonDown();

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

void EncoderDriver::onShortClick() {
    if (onShortClickCallback) {
        onShortClickCallback();
    }
}

void EncoderDriver::onLongClick() {
    if (onLongClickCallback) {
        onLongClickCallback();
    }
}

void EncoderDriver::setOnShortClick(std::function<void()> callback) {
    onShortClickCallback = callback;
}

void EncoderDriver::setOnLongClick(std::function<void()> callback) {
    onLongClickCallback = callback;
}

void EncoderDriver::setOnValueChange(std::function<void(int32_t)> callback) {
    onValueChangeCallback = callback;
}
