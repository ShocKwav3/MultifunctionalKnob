#pragma once

#include "EncoderMode/Interface/EncoderModeBaseInterface.h"

class EncoderModeHandlerInterface: public EncoderModeBaseInterface {
public:
    virtual ~EncoderModeHandlerInterface() = default;

    virtual void handleRotate(int delta) override = 0;
    virtual void handleShortClick() override = 0;
    virtual void handleLongClick() override = 0;

    virtual const char* getModeName() const override = 0;
};
