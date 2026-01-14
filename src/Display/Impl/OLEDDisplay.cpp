#include "OLEDDisplay.h"
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

void OLEDDisplay::showMenu(const char* title, const char* const* items, uint8_t count, uint8_t selected) {
    ensureInitialized();

    if (!initialized || items == nullptr || count == 0) {
        return;
    }

    display.clearDisplay();

    // Display title on first line (0-7 pixels)
    if (title != nullptr) {
        display.setCursor(0, 0);
        display.print(title);
    }

    // Display menu items starting from line 2 (y=10)
    // With 32 pixels height and 8 pixel font, we can show 2-3 items comfortably
    uint8_t startY = 10;
    uint8_t lineHeight = 11; // 8px font + 3px spacing
    uint8_t maxVisibleItems = 2; // Show only 2 items to fit in 32px height

    // Calculate which items to show (center around selected)
    uint8_t startIdx = 0;
    if (selected > 0 && count > maxVisibleItems) {
        startIdx = selected;
        if (startIdx + maxVisibleItems > count) {
            startIdx = count - maxVisibleItems;
        }
    }

    for (uint8_t i = 0; i < maxVisibleItems && (startIdx + i) < count; i++) {
        uint8_t itemIdx = startIdx + i;
        display.setCursor(0, startY + (i * lineHeight));

        // Show selection indicator
        display.print(itemIdx == selected ? "> " : "  ");
        display.print(items[itemIdx] != nullptr ? items[itemIdx] : "");
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
