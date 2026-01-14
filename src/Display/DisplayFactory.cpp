#include "Display/DisplayFactory.h"
#include "Display/Impl/SerialDisplay.h"

#ifdef USE_OLED_DISPLAY
#include "Display/Impl/OLEDDisplay.h"
#endif

DisplayInterface& DisplayFactory::getDisplay() {
#ifdef USE_OLED_DISPLAY
    static OLEDDisplay display;
#else
    static SerialDisplay display;
#endif

    return display;
}
