#include "RotaryEncoderDriver.h"

RotaryEncoderDriver* RotaryEncoderDriver::rotaryEncoderDriverInstance = nullptr;
AiEsp32RotaryEncoder* RotaryEncoderDriver::rotaryEncoderInstance = nullptr;

RotaryEncoderDriver::RotaryEncoderDriver(
    uint8_t clkPin,
    uint8_t dtPin,
    int swPin,
    int vccPin,
    uint8_t steps
): clkPin(clkPin), dtPin(dtPin), swPin(swPin), vccPin(vccPin), steps(steps) {}

void RotaryEncoderDriver::encoderTask(void* pvParameters) {
    for (;;) {
        if (rotaryEncoderDriverInstance) {
            rotaryEncoderDriverInstance->runLoop();
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // ~100Hz
    }
}

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

    xTaskCreate(
        encoderTask,
        "EncoderTask",
        4096,
        nullptr,
        1,
        nullptr
    );
}

void IRAM_ATTR RotaryEncoderDriver::readEncoderISR() {
    if (rotaryEncoderInstance) {
        rotaryEncoderInstance->readEncoder_ISR();
    }
}

void RotaryEncoderDriver::runLoop() {
        if (rotaryEncoderInstance->encoderChanged()) {
        int32_t newValue = rotaryEncoderInstance->readEncoder();
        if (onValueChangeCallback) {
            onValueChangeCallback(newValue);
        } else {
            Serial.print("Value: ");
            Serial.println(newValue);
        }
    }
    handleButton();
}

void RotaryEncoderDriver::handleButton() {
    static unsigned long lastTimeButtonDown = 0;
    static bool wasButtonDown = false;
    unsigned long shortPressAfterMs = 50;
    unsigned long longPressAfterMs = 1000;

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
    } else {
        Serial.print("button SHORT press ");
        Serial.println(millis());
    }
}

void RotaryEncoderDriver::onLongClick() {
    if (onLongClickCallback) {
        onLongClickCallback();
    } else {
        Serial.print("button LONG press ");
        Serial.println(millis());
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
