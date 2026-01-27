#pragma once

#include <Arduino.h>
#include <functional>
#include <cstdint>
#include <cstddef>

// Maximum buttons supported (matches button_config.h BUTTON_COUNT)
#define MAX_BUTTONS 5

class ButtonDriver {
public:
    ButtonDriver();

    static ButtonDriver* getInstance();

    void begin();
    void setOnShortPress(uint8_t buttonIndex, std::function<void()> callback);
    void setOnLongPress(uint8_t buttonIndex, std::function<void()> callback);

private:
    static ButtonDriver* instance;

    static void buttonTask(void* pvParameters);

    std::function<void()> shortPressCallbacks[MAX_BUTTONS];
    std::function<void()> longPressCallbacks[MAX_BUTTONS];

    struct ButtonState {
        bool pressed;
        bool lastReading;
        uint32_t lastChangeTime;
        uint32_t pressStartTime;
    };

    ButtonState buttonStates[MAX_BUTTONS];

    void runLoop();
    void handleButton(uint8_t index);
    bool readButton(uint8_t index);
    bool isDebounced(uint8_t index, uint32_t currentTime);
};
