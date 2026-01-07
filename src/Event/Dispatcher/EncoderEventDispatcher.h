#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Type/EncoderInputEvent.h"

class ConfigManager;

class EncoderEventDispatcher {
public:
    EncoderEventDispatcher(QueueHandle_t queue, ConfigManager* configManager);

    void onEncoderValueChange(int32_t newValue);
    void onShortClick();
    void onLongClick();

private:
    QueueHandle_t eventQueue;
    ConfigManager* configManager;
    int32_t lastValue = 500;  // Match encoder initial value from EncoderDriver::begin()
};
