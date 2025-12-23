#pragma once

#include "Config/button_config.h"
#include <cstdint>

class ButtonEventDispatcher;

class ButtonManager {
public:
    explicit ButtonManager(ButtonEventDispatcher* dispatcher);

    void init();
    void poll();

private:
    ButtonEventDispatcher* eventDispatcher;

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
