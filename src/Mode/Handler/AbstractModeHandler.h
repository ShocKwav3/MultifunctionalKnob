#pragma once

#include "Arduino.h"

#include "ModeHandlerInterface.h"
#include "Event/Dispatcher/AppEventDispatcher.h"

class AbstractModeHandler : public ModeHandlerInterface {
public:
    AbstractModeHandler(AppEventDispatcher* dispatcher);

    virtual void handleLongClick() override;
    const char* getModeName() const override;

protected:
    AppEventDispatcher* appEventDispatcher;
};
