#pragma once

#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include "functional"

class RotaryEncoderDriver {
public:
    RotaryEncoderDriver(
        uint8_t clkPin,
        uint8_t dtPin,
        int swPin,
        int vccPin,
        uint8_t steps
    );

    static RotaryEncoderDriver* getInstance(
        uint8_t clkPin,
        uint8_t dtPin,
        int swPin,
        int vccPin,
        uint8_t steps
    );

    void begin();
    void setOnShortClick(std::function<void()> callback);
    void setOnLongClick(std::function<void()> callback);
    void setOnValueChange(std::function<void(int32_t newValue)> callback);

    static void IRAM_ATTR readEncoderISR();

private:
    static RotaryEncoderDriver* rotaryEncoderDriverInstance;
    static AiEsp32RotaryEncoder* rotaryEncoderInstance;

    static void encoderTask(void* pvParameters);

    std::function<void()> onShortClickCallback = nullptr;
    std::function<void()> onLongClickCallback = nullptr;
    std::function<void(int32_t)> onValueChangeCallback = nullptr;

    void handleButton();
    void onShortClick();
    void onLongClick();
    void runLoop();

    uint8_t clkPin;
    uint8_t dtPin;
    int swPin;
    int vccPin;
    uint8_t steps;
};
