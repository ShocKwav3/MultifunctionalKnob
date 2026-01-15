#include "OLEDDisplay.h"
#include "state/HardwareState.h"
#include "../Bitmaps.h"
#include "Config/log_config.h"
#include <Wire.h>

OLEDDisplay::OLEDDisplay()
    : display(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, OLED_RESET)
    , initialized(false) {
}

void OLEDDisplay::ensureInitialized() {
    if (initialized) {
        return;
    }

    // Initialize I2C with correct frequency before display.begin()
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN, OLED_I2C_FREQUENCY);

    // display.begin() will use already-initialized Wire (periphBegin=true is default)
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
        LOG_ERROR(TAG, "SSD1306 initialization failed");
        return;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);  // Prevent long text from wrapping
    display.cp437(true);
    display.display();

    initialized = true;
    LOG_INFO(TAG, "Initialized 128x32 SSD1306");
}

void OLEDDisplay::showMenu(const char* title, const char* const* items, uint8_t count, uint8_t selected, const HardwareState& state) {
    ensureInitialized();

    if (!initialized || items == nullptr || count == 0) {
        return;
    }

    display.clearDisplay();

    // Draw status bar (y=0-7) with BT icon and mode indicator
    drawMenuModeStatusBar(state);

    // Display menu items starting below status bar (y=8)
    // With 24 pixels available (y=8-31) and 8px font, we can show 3 items
    uint8_t startY = 8;
    uint8_t lineHeight = 8; // 8px font height (no extra spacing needed)
    uint8_t maxVisibleItems = 3; // Show 3 items in the 24px available space

    // Calculate scrolling window to keep selected item visible
    // Window shows 3 items, centered around selection when possible
    uint8_t windowStart = 0;
    if (count > maxVisibleItems) {
        if (selected >= maxVisibleItems - 1) {
            windowStart = selected - (maxVisibleItems - 2);  // Keep selected near top
            if (windowStart + maxVisibleItems > count) {
                windowStart = count - maxVisibleItems;  // Clamp to end
            }
        }
    }

    // Render menu items with highlighting for selected item
    for (uint8_t i = 0; i < maxVisibleItems && (windowStart + i) < count; i++) {
        uint8_t itemIdx = windowStart + i;
        uint8_t itemY = startY + (i * lineHeight);
        bool isSelected = (itemIdx == selected);

        // Apply highlighting to selected item (inverted colors)
        if (isSelected) {
            // Draw white filled rectangle behind selected item
            display.fillRect(0, itemY, OLED_SCREEN_WIDTH, lineHeight, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            // Normal colors for non-selected items
            display.setTextColor(SSD1306_WHITE);
        }

        // Draw arrow indicator for selected item
        display.setCursor(0, itemY);
        if (isSelected) {
            display.print(">");
        }

        // Draw menu item text
        display.setCursor(isSelected ? 10 : 4, itemY);  // Offset for arrow
        display.print(items[itemIdx] != nullptr ? items[itemIdx] : "");

        // Reset text color for next item
        display.setTextColor(SSD1306_WHITE);
    }

    display.display();
}

void OLEDDisplay::showMessage(const char* message) {
    ensureInitialized();

    if (!initialized) {
        return;
    }

    display.clearDisplay();

    // Center message vertically (y=12 for 32px height)
    if (message != nullptr) {
        centerText(message, 12);
    }

    display.display();
}

void OLEDDisplay::showConfirmation(const char* message) {
    ensureInitialized();

    if (!initialized) {
        return;
    }

    display.clearDisplay();

    // Show checkmark or "OK" at top
    display.setCursor(0, 0);
    display.print("[OK]");

    // Show message below
    if (message != nullptr) {
        display.setCursor(0, 12);
        display.print(message);
    }

    display.display();
}

void OLEDDisplay::showStatus(const char* key, const char* value) {
    ensureInitialized();

    if (!initialized) {
        return;
    }

    display.clearDisplay();

    // Show key on first line
    if (key != nullptr) {
        display.setCursor(0, 0);
        display.print(key);
        display.print(":");
    }

    // Show value on second line
    if (value != nullptr) {
        display.setCursor(0, 12);
        display.print(value);
    }

    display.display();
}

void OLEDDisplay::clear() {
    ensureInitialized();

    if (!initialized) {
        return;
    }

    display.clearDisplay();
    display.display();
}

void OLEDDisplay::drawNormalMode(const HardwareState& state) {
    ensureInitialized();

    if (!initialized) {
        return;
    }

    display.clearDisplay();
    drawStatusBar(state);
    drawModeIndicator(state.encoderWheelState.mode);
    drawDirectionIndicator(state.encoderWheelState.direction);
    display.display();
}

void OLEDDisplay::drawStatusBar(const HardwareState& state) {
    // STATUS BAR (y=0-7): BT icon + battery
    // Draw BT icon with flashing effect when pairing
    if (state.bleState.isConnected) {
        // Solid icon when connected
        display.drawBitmap(0, 0, btIcon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    } else if (state.bleState.isPairingMode) {
        // Flashing icon when pairing (time-based toggle for consistency)
        if ((millis() / 500) % 2 == 0) {  // Toggle every 500ms (1Hz blink rate)
            display.drawBitmap(0, 0, btIcon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
        }
    }

    // Draw battery percentage on right side of status bar
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    char batteryStr[8];
    snprintf(batteryStr, sizeof(batteryStr), "%d%%", state.batteryPercent);

    // Calculate x position to right-align battery text
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(batteryStr, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(OLED_SCREEN_WIDTH - w - 2, 0);
    display.print(batteryStr);
}

void OLEDDisplay::drawModeIndicator(WheelMode mode) {
    // MAIN AREA (y=8-23): Large mode indicator (S/V/Z)
    display.setTextSize(2);
    const char* modeChar;
    switch (mode) {
        case WheelMode::SCROLL:
            modeChar = "S";
            break;
        case WheelMode::VOLUME:
            modeChar = "V";
            break;
        case WheelMode::ZOOM:
            modeChar = "Z";
            break;
        default:
            modeChar = "?";
            break;
    }

    // Center mode indicator horizontally and vertically in main area
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(modeChar, 0, 0, &x1, &y1, &w, &h);
    int16_t modeX = (OLED_SCREEN_WIDTH - w) / 2;
    int16_t modeY = 8 + ((16 - h) / 2);  // 16 = height of main area (y=8-23)
    display.setCursor(modeX, modeY);
    display.print(modeChar);
}

void OLEDDisplay::drawDirectionIndicator(WheelDirection direction) {
    // BOTTOM ROW (y=24-31): Direction indicator using arrow bitmaps
    display.setTextSize(1);

    // Draw direction arrow icon (up for normal, down for reversed)
    if (direction == WheelDirection::NORMAL) {
        display.drawBitmap(0, 24, arrowUpIcon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    } else {
        display.drawBitmap(0, 24, arrowDownIcon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    }
}

void OLEDDisplay::drawMenuModeStatusBar(const HardwareState& state) {
    // STATUS BAR for MENU MODE (y=0-7): BT icon (left) + mode (center) + battery (right)

    // Draw BT icon on left side with flashing effect when pairing
    if (state.bleState.isConnected) {
        // Solid icon when connected
        display.drawBitmap(0, 0, btIcon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    } else if (state.bleState.isPairingMode) {
        // Flashing icon when pairing (time-based toggle for consistency)
        if ((millis() / 500) % 2 == 0) {  // Toggle every 500ms (1Hz blink rate)
            display.drawBitmap(0, 0, btIcon, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
        }
    }

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Draw mode indicator in center
    const char* modeChar;
    switch (state.encoderWheelState.mode) {
        case WheelMode::SCROLL:
            modeChar = "S";
            break;
        case WheelMode::VOLUME:
            modeChar = "V";
            break;
        case WheelMode::ZOOM:
            modeChar = "Z";
            break;
        default:
            modeChar = "?";
            break;
    }

    // Calculate center position for mode indicator
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(modeChar, 0, 0, &x1, &y1, &w, &h);
    int16_t modeX = (OLED_SCREEN_WIDTH - w) / 2;
    display.setCursor(modeX, 0);
    display.print(modeChar);

    // Draw battery percentage on right side
    char batteryStr[8];
    snprintf(batteryStr, sizeof(batteryStr), "%d%%", state.batteryPercent);

    // Calculate x position to right-align battery text
    display.getTextBounds(batteryStr, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(OLED_SCREEN_WIDTH - w - 2, 0);
    display.print(batteryStr);

    // Draw horizontal separator line at y=7 (bottom of status bar)
    display.drawLine(0, 7, OLED_SCREEN_WIDTH, 7, SSD1306_WHITE);
}

void OLEDDisplay::centerText(const char* text, uint8_t y) {
    if (text == nullptr) {
        return;
    }

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);

    // Use int16_t to handle negative values when text > screen width
    int16_t x = (OLED_SCREEN_WIDTH - w) / 2;
    if (x < 0) x = 0;  // Clamp to left edge if text too wide
    display.setCursor(x, y);
    display.print(text);
}
