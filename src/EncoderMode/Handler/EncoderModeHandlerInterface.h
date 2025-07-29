#pragma once

#include "Component/Interface/EncoderInputHandlerInterface.h"

class EncoderModeHandlerInterface: public EncoderInputHandlerInterface {
public:
    virtual ~EncoderModeHandlerInterface() = default;

    virtual void handleRotate(int delta) override = 0;
    virtual void handleShortClick() override = 0;
    virtual void handleLongClick() override = 0;

    virtual const char* getModeName() const;
};
