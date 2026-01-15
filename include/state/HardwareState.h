#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "Type/BleStateType.h"
#include "Type/EncoderWheelStateType.h"

/**
 * @brief Hardware state for normal mode display
 *
 * Contains all hardware state information needed to render the status screen:
 * - Encoder wheel state (mode and direction)
 * - Battery level (placeholder for now)
 * - Bluetooth state (consolidated connection and pairing status)
 *
 * This structure is independent of AppState and serves as the single
 * source of truth for hardware state.
 */
struct HardwareState {
    EncoderWheelStateType encoderWheelState;  ///< Encoder wheel mode and direction
    uint8_t batteryPercent;                   ///< Battery level 0-100 (placeholder)
    BleStateType bleState;                    ///< Consolidated BLE state (connection + pairing)
};

// Global hardware state instance (defined in main.cpp)
extern HardwareState hardwareState;
