#pragma once

#include "Arduino.h"

#include "AbstractModeHandler.h"

class ScrollModeHandler : public AbstractModeHandler {
public:
    ScrollModeHandler(AppEventDispatcher* dispatcher)
        : AbstractModeHandler(dispatcher) {}

    void handleRotate(int delta) override;
    void handleShortClick() override;
};
