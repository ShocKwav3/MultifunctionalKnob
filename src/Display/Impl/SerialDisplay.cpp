#include "SerialDisplay.h"
#include "Config/log_config.h"

SerialDisplay::SerialDisplay() {
    LOG_INFO(TAG, "Initialized");
}

void SerialDisplay::showMenu(const char* title, const char* const* items, uint8_t count, uint8_t selected, const HardwareState& hwState) {
    if (items == nullptr || count == 0) {
        return;
    }

    // Note: hwState parameter available but not used in serial output
    // Could be enhanced to show status bar info in serial output if desired

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

void SerialDisplay::drawNormalMode(const HardwareState& hwState) {
    Serial.println("=== Normal Mode Status ===");
    Serial.print("Mode: ");
    Serial.println(wheelModeToDisplayString(hwState.encoderWheelState.mode));
    Serial.print("Direction: ");
    Serial.println(wheelDirectionToDisplayString(hwState.encoderWheelState.direction));
    Serial.print("Battery: ");
    Serial.print(hwState.batteryPercent);
    Serial.println("%");
    Serial.print("BT Connected: ");
    Serial.println(hwState.bleState.isConnected ? "Yes" : "No");
    Serial.print("BT Pairing: ");
    Serial.println(hwState.bleState.isPairingMode ? "Yes" : "No");
    Serial.println("=========================");
}
