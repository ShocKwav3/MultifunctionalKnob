#pragma once

#include <cstdint>
#include "Config/button_config.h"

class ButtonEventDispatcher;

class ButtonManager {
public:
    explicit ButtonManager(ButtonEventDispatcher* dispatcher);

    void init();

private:
    ButtonEventDispatcher* eventDispatcher;

    // FreeRTOS task function for button polling
    static void buttonTask(void* pvParameters);

    // Internal polling method - called by FreeRTOS task only
    void poll();

    struct ButtonState {
        bool pressed;
        bool lastReading;
        uint32_t lastChangeTime;
    };

    ButtonState buttonStates[BUTTON_COUNT];

    // Helper methods
    bool readButton(uint8_t index);
    bool isDebounced(uint8_t index, uint32_t currentTime);
};
