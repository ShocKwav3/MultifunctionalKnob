#pragma once

#include <stdint.h>

// OLED Display Hardware Configuration (128x32 SSD1306)
constexpr uint8_t OLED_SCREEN_WIDTH = 128;
constexpr uint8_t OLED_SCREEN_HEIGHT = 32;
constexpr int8_t OLED_RESET = -1;
constexpr uint8_t OLED_I2C_ADDRESS = 0x3C;
constexpr uint8_t OLED_SDA_PIN = 6;
constexpr uint8_t OLED_SCL_PIN = 7;
constexpr uint32_t OLED_I2C_FREQUENCY = 400000;  // 400kHz
