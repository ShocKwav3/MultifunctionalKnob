#include "Display/DisplayFactory.h"
#include "Display/Impl/SerialDisplay.h"

// TODO: Add conditional include for OLED display when implemented
// #ifdef USE_OLED_DISPLAY
// #include "Display/Impl/OledDisplay.h"
// #endif

DisplayInterface& DisplayFactory::getDisplay() {
    // TODO: Use build flags to select implementation
    // #ifdef USE_OLED_DISPLAY
    // static OledDisplay display;
    // #else
    static SerialDisplay display;
    // #endif

    return display;
}
