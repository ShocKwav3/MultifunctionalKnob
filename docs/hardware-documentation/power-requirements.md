# Power Requirements

## Power Supply

**Input:** USB 5V via USB-C connector

**Onboard Regulation:**
- USB 5V → 3.3V LDO regulator (built-in on Super Mini)
- 3.3V rail powers ESP32-C3, peripherals

## Current Consumption

**Estimated Operating Current:**
- **ESP32-C3 Active (WiFi/BT off):** ~40 mA
- **ESP32-C3 + BLE Active:** ~80-120 mA (depending on TX power)
- **OLED Display:** ~15-30 mA (depending on pixel activity)
- **Total (typical operation):** ~100-150 mA @ 5V USB

**Peak Current:** ~200 mA during BLE transmission

**Notes:**
- No deep sleep implemented (continuous operation)
- Power optimization possible via sleep modes
- Battery operation possible with 3.3V LiPo + voltage regulator
