#pragma once

#include <Arduino.h> // TODO: Remove later

class ModeHandlerInterface {
public:
    virtual void handleRotate(int delta) = 0;
    virtual void handleShortClick() = 0;
    virtual void handleLongClick() = 0;
    virtual ~ModeHandlerInterface() = default;
};
