#pragma once

#include "Arduino.h"

class EncoderModeHandlerInterface {
public:
    virtual void handleRotate(int delta) = 0;
    virtual void handleShortClick() = 0;
    virtual void handleLongClick() = 0;
    virtual const char* getModeName() const = 0;
    virtual ~EncoderModeHandlerInterface() = default;
};
