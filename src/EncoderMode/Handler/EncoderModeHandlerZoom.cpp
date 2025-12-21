#include "EncoderModeHandlerZoom.h"
#include "Config/log_config.h"

static const char* TAG = "EncoderModeHandlerZoom";

EncoderModeHandlerZoom::EncoderModeHandlerZoom(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard)
    : EncoderModeHandlerAbstract(dispatcher, bleKeyboard) {}

void EncoderModeHandlerZoom::handleRotate(int delta) {
    LOG_DEBUG(TAG, "Rotating by %d", delta);

    if (!bleKeyboard->isConnected()) {
        LOG_DEBUG(TAG, "BLE keyboard not connected, skipping zoom command");
        return;
    }

    // Determine zoom direction: positive delta = zoom in (Ctrl++), negative = zoom out (Ctrl+-)
    const uint8_t zoomKey = (delta > 0) ? KEY_NUM_PLUS : KEY_NUM_MINUS;
    const int repeatCount = abs(delta);

    LOG_DEBUG(TAG, "Sending zoom %s command %d times", (delta > 0) ? "in" : "out", repeatCount);

    // Press Ctrl once before loop for efficiency
    bleKeyboard->press(KEY_LEFT_CTRL);

    // Send Plus or Minus for each delta step
    for (int i = 0; i < repeatCount; i++) {
        bleKeyboard->press(zoomKey);
        bleKeyboard->release(zoomKey);
    }

    // Release Ctrl once after loop
    bleKeyboard->release(KEY_LEFT_CTRL);
}

void EncoderModeHandlerZoom::handleShortClick() {
    LOG_DEBUG(TAG, "Short click detected");
}

const char* EncoderModeHandlerZoom::getModeName() const {
    return EncoderModeHelper::toString(EventEnum::EncoderModeEventTypes::ENCODER_MODE_ZOOM);
}
