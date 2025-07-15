#include "ModeHandlerInterface.h"

class AbstractModeHandler : public ModeHandlerInterface {
public:
    void handleLongClick() override {
        Serial.println("Entering mode selection...");
        // Future: appState->isModeSelection = true;
    }
};
