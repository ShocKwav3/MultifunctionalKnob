#pragma once

/**
 * @file macro_config.h
 * @brief Configuration constants for the macro button system
 *
 * Defines hardware pin mappings and macro system parameters.
 * Supports 7 macro inputs: wheel button, wheel left/right, and 4 buttons.
 */

#include <stdint.h>

// Macro Button Hardware Configuration
constexpr uint8_t MACRO_BUTTON_PIN = 10;
constexpr bool MACRO_BUTTON_ACTIVE_LOW = true;

// Macro System Configuration
constexpr uint8_t MACRO_INPUT_COUNT = 7;
