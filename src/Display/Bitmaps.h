#pragma once

#include <Arduino.h>
#include <stdint.h>

/**
 * @brief Icon bitmaps for OLED status display
 *
 * All bitmaps stored in PROGMEM (flash) to save RAM.
 * Icons are 8x8 pixels, designed for 128x32 OLED display.
 */

// Bluetooth icon (8x8) - Bluetooth symbol
const unsigned char PROGMEM btIcon[] = {
    0b00010000,
    0b00011000,
    0b01010100,
    0b00111000,
    0b00111000,
    0b01010100,
    0b00011000,
    0b00010000
};

// Battery icon (8x8) - Battery outline
const unsigned char PROGMEM batteryIcon[] = {
    0b00111100,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01111110,
    0b01111110
};

// Arrow up icon (8x8) - Up arrow for normal direction
const unsigned char PROGMEM arrowUpIcon[] = {
    0b00010000,
    0b00111000,
    0b01111100,
    0b11111110,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000
};

// Arrow down icon (8x8) - Down arrow for reversed direction
const unsigned char PROGMEM arrowDownIcon[] = {
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b11111110,
    0b01111100,
    0b00111000,
    0b00010000
};

// Icon dimensions
constexpr uint8_t ICON_WIDTH = 8;
constexpr uint8_t ICON_HEIGHT = 8;
