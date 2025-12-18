#pragma once

#include "Display/Interface/DisplayInterface.h"

/**
 * @brief Factory for creating/retrieving the active display implementation
 *
 * Controls which display implementation is used based on build configuration.
 * Ensures only one display instance is created (Singleton pattern).
 */
class DisplayFactory {
public:
    /**
     * @brief Get the active Display Interface instance
     *
     * @return Reference to the active DisplayInterface implementation
     */
    static DisplayInterface& getDisplay();
};
