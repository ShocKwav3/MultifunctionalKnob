#pragma once

#include "Arduino.h"

#include "AbstractModeHandler.h"
#include "Enum/ModeEnum.h"
#include "Helper/ModeHelper.h"

class ScrollModeHandler : public AbstractModeHandler {
public:
    ScrollModeHandler(AppEventDispatcher* dispatcher);

    void handleRotate(int delta) override;
    void handleShortClick() override;
    const char* getModeName() const override;
};
