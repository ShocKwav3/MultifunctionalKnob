#pragma once

#include "Arduino.h"

#include "AbstractModeHandler.h"
#include "Enum/ModeEnum.h"
#include "Helper/ModeHelper.h"

class ModeSelectionHandler : public AbstractModeHandler {
public:
    ModeSelectionHandler(AppEventDispatcher* dispatcher);

    void handleRotate(int delta) override;
    void handleShortClick() override;
    void handleLongClick() override;
    const char* getModeName() const override;
private:
    int selectionIndex = 0;
};
