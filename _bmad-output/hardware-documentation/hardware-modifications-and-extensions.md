# Hardware Modifications and Extensions

## Possible Hardware Enhancements

1. **Battery Power:**
   - Add LiPo battery (3.7V) with charging circuit
   - Connect to 3.3V rail (bypass USB regulator)
   - Add power switch

2. **Additional Buttons:**
   - Add GPIO buttons for direct mode selection
   - Useful GPIOs: 3, 4, 5, 8, 9, 10

3. **Status LEDs:**
   - Add RGB LED for mode indication
   - PWM control for brightness
   - Available GPIOs: Multiple pins available

4. **External Antenna:**
   - Improve Bluetooth range with external antenna
   - Requires board rework (antenna switch)

5. **Haptic Feedback:**
   - Add vibration motor
   - Connect via transistor to GPIO
   - Provides tactile response

6. **Additional Encoders:**
   - Add second encoder for multi-axis control
   - ESP32-C3 has sufficient GPIOs
