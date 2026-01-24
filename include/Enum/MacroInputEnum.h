#pragma once

/**
 * @file MacroInputEnum.h
 * @brief Enumeration for macro input sources
 *
 * Defines the 7 macro input types that can trigger macro execution.
 * Enum values directly correspond to NVS storage indices (0-6).
 */

#include <stdint.h>

/**
 * @enum MacroInput
 * @brief Input sources that can trigger macros
 *
 * Each value maps to a NVS key index: "macro.0" through "macro.6"
 * Total count must match MACRO_INPUT_COUNT from macro_config.h
 */
enum class MacroInput : uint8_t {
    WHEEL_BUTTON = 0,  // Encoder button press
    WHEEL_LEFT = 1,    // Encoder rotate left
    WHEEL_RIGHT = 2,   // Encoder rotate right
    BUTTON_1 = 3,      // Button 1 press
    BUTTON_2 = 4,      // Button 2 press
    BUTTON_3 = 5,      // Button 3 press
    BUTTON_4 = 6       // Button 4 press
};
