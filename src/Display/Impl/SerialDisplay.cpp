#include "SerialDisplay.h"
#include "Config/log_config.h"

SerialDisplay::SerialDisplay() {
    LOG_INFO(TAG, "Initialized");
}

void SerialDisplay::showMenu(const char* title, const char** items, uint8_t count, uint8_t selected) {
    if (items == nullptr || count == 0) {
        return;
    }

    if (title != nullptr) {
        Serial.println(title);
    }

    for (uint8_t i = 0; i < count; i++) {
        Serial.print(i == selected ? "> " : "  ");
        Serial.println(items[i] != nullptr ? items[i] : "");
    }
}

void SerialDisplay::showMessage(const char* message) {
    Serial.print("[MSG] ");
    Serial.println(message != nullptr ? message : "");
}

void SerialDisplay::showConfirmation(const char* message) {
    Serial.print("[OK] ");
    Serial.println(message != nullptr ? message : "");
}

void SerialDisplay::showStatus(const char* key, const char* value) {
    Serial.print(key != nullptr ? key : "");
    Serial.print(": ");
    Serial.println(value != nullptr ? value : "");
}

void SerialDisplay::clear() {
    Serial.println("---");
}
