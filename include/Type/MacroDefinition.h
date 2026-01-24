#pragma once

/**
 * @file MacroDefinition.h
 * @brief Data structure for HID macro key combinations
 *
 * MacroDefinition stores a keyboard shortcut as modifier + keycode.
 * Uses packed uint16_t format for compact NVS storage.
 */

#include <stdint.h>

/**
 * @struct MacroDefinition
 * @brief Represents a HID keyboard macro (modifier + keycode)
 *
 * Storage format:
 * - modifiers: HID modifier bitmask (e.g., KEY_LEFT_CTRL, KEY_LEFT_SHIFT)
 * - keycode: HID keycode (e.g., 'C' for Ctrl+C)
 * - Empty macro: both fields are 0x00 (0x0000 when packed)
 *
 * Example: Ctrl+C = {KEY_LEFT_CTRL, 'C'}
 */
struct MacroDefinition {
    uint8_t modifiers;  // HID modifier bitmask
    uint8_t keycode;    // HID keycode

    /**
     * @brief Check if macro is empty (not assigned)
     * @return true if both modifiers and keycode are zero
     */
    bool isEmpty() const {
        return modifiers == 0 && keycode == 0;
    }

    /**
     * @brief Pack macro into uint16_t for NVS storage
     * @return Packed value: (modifiers << 8) | keycode
     */
    uint16_t toPacked() const {
        return (static_cast<uint16_t>(modifiers) << 8) | keycode;
    }

    /**
     * @brief Unpack uint16_t from NVS into MacroDefinition
     * @param packed Packed value from NVS
     * @return MacroDefinition with extracted modifiers and keycode
     */
    static MacroDefinition fromPacked(uint16_t packed) {
        return MacroDefinition{
            static_cast<uint8_t>((packed >> 8) & 0xFF),  // modifiers (high byte)
            static_cast<uint8_t>(packed & 0xFF)           // keycode (low byte)
        };
    }
};
