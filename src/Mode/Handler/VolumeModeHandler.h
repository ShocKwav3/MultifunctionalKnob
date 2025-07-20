#pragma once

#include "Arduino.h"

#include "AbstractModeHandler.h"
#include "Enum/ModeEnum.h"
#include "Helper/ModeHelper.h"

class VolumeModeHandler : public AbstractModeHandler {
public:
    VolumeModeHandler(AppEventDispatcher* dispatcher);

    void handleRotate(int delta) override;
    void handleShortClick() override;
    const char* getModeName() const override;
};
