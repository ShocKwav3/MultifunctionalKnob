#include "AbstractModeHandler.cpp"

class ScrollModeHandler : public AbstractModeHandler {
public:
    void handleRotate(int delta) override {
        Serial.printf("Scrolling by %d\n", delta);
        // bleKeyboard.mouseMove(...);
    }

    void handleShortClick() override {
        Serial.println("ScrollMode: short click");
    }
};
