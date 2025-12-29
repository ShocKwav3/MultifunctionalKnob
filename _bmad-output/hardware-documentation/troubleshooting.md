# Troubleshooting

## Common Hardware Issues

### Issue: OLED Display Not Working

**Symptoms:** Blank display, no initialization

**Checks:**
1. Verify I2C address (0x3C) using I2C scanner
2. Check SDA/SCL connections (GPIO 6, 7)
3. Verify 3.3V power to display
4. Check pull-up resistors (usually built-in on display module)
5. Test with different I2C speed

### Issue: Encoder Not Responding

**Symptoms:** No rotation detected, no click events

**Checks:**
1. Verify CLK, DT, SW pin connections (GPIO 1, 0, 2)
2. Check encoder power connection (3.3V, GND)
3. Verify pull-up resistors enabled in code
4. Test encoder with multimeter (continuity when pressed, voltage change on rotation)
5. Check encoder steps configuration (4 steps per detent)

### Issue: Bluetooth Not Pairing

**Symptoms:** Device not visible, pairing fails

**Checks:**
1. Verify NimBLE library initialization in serial output
2. Check Bluetooth is enabled on host device
3. Confirm device name "KnobKoKy" in BLE scan
4. Remove old pairings and retry
5. Check ESP32-C3 antenna connection (built-in PCB antenna)
6. Verify sufficient power supply (BLE draws significant current)

### Issue: USB Serial Not Detected

**Symptoms:** No COM port, cannot upload

**Checks:**
1. Try different USB cable (must support data, not charge-only)
2. Verify USB drivers installed (CP210x or CH340, depending on board)
3. Check USB CDC flags in `platformio.ini`
4. Hold BOOT button while plugging in USB (force bootloader mode)
5. Reset board after connecting
