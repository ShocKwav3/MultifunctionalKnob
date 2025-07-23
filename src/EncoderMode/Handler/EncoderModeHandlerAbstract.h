#pragma once

#include "Arduino.h"

#include "EncoderModeHandlerInterface.h"
#include "Event/Dispatcher/AppEventDispatcher.h"
#include "Enum/EventEnum.h"

class EncoderModeHandlerAbstract : public EncoderModeHandlerInterface {
public:
    EncoderModeHandlerAbstract(AppEventDispatcher* dispatcher);

    virtual void handleLongClick() override;
    const char* getModeName() const override;

protected:
    AppEventDispatcher* appEventDispatcher;
};
