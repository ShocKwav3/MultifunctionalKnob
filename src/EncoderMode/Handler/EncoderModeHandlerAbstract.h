#pragma once

#include "Arduino.h"
#include "BleKeyboard.h"

#include "EncoderModeHandlerInterface.h"
#include "Event/Dispatcher/AppEventDispatcher.h"
#include "Enum/EventEnum.h"

class EncoderModeHandlerAbstract : public EncoderModeHandlerInterface {
public:
    EncoderModeHandlerAbstract(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard);

    virtual void handleLongClick() override;
    const char* getModeName() const override;

protected:
    AppEventDispatcher* appEventDispatcher;
    BleKeyboard* bleKeyboard;
};
