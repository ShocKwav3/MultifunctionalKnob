#pragma once

#include "Component/Interface/EncoderInputHandlerInterface.h"

class EncoderModeBaseInterface : public EncoderInputHandlerInterface {
public:
    virtual ~EncoderModeBaseInterface() = default;

    virtual void handleRotate(int delta) override = 0;
    virtual void handleShortClick() override = 0;
    virtual void handleLongClick() override = 0;

    virtual const char* getModeName() const = 0;
};
