# Hardware

## Bill of materials

| Qty | Component | Specification | Role |
| --- | --- | --- | --- |
| 1 | ESP32-C3 Super Mini | Nologo variant, USB-C | Microcontroller, BLE radio, USB serial |
| 1 | Rotary encoder | EC11 or compatible, with push button | Wheel, menu navigation, wake source |
| 1 | OLED display | SSD1306, 128x32, I2C | Status and menu |
| 5 | Push buttons | Momentary, normally open | Four media-key buttons and the macro button |
| 1 | USB-C cable | Data capable | Power and programming |

## Microcontroller

- ESP32-C3: 32-bit RISC-V, single core, 160 MHz, 400 KB SRAM, 4 MB flash, Bluetooth 5 (LE).
- Native USB Serial/JTAG: the board needs no USB-UART chip or driver.
- Board definition: `boards/nologo_esp32c3_super_mini.json`.

## Pin map

| GPIO | Function | Connected to | Defined in |
| --- | --- | --- | --- |
| 0 | Encoder DT (channel B) | Encoder | `include/Config/encoder_config.h` |
| 1 | Encoder CLK (channel A) | Encoder | `include/Config/encoder_config.h` |
| 2 | Encoder button, deep-sleep wake source | Encoder | `include/Config/encoder_config.h` |
| 3 | Button Top Left | Button to GND | `include/Config/button_config.h` |
| 4 | Button Top Right | Button to GND | `include/Config/button_config.h` |
| 5 | Macro button | Button to GND | `include/Config/button_config.h` |
| 6 | I2C SDA | OLED | `include/Config/display_config.h` |
| 7 | I2C SCL | OLED | `include/Config/display_config.h` |
| 10 | Button Bottom Left | Button to GND | `include/Config/button_config.h` |
| 20 | Button Bottom Right | Button to GND | `include/Config/button_config.h` |

- All inputs use the internal pull-up resistors. The buttons are active-low.
- The encoder has 4 steps for each detent (`ENCODER_STEPS`).
- The OLED uses I2C address `0x3C` at 400 kHz, with no reset pin.
- Before you assign a new pin, check the strapping pins and the USB pins in the ESP32-C3 datasheet.

## Wiring

1. Connect the encoder CLK to GPIO 1, DT to GPIO 0 and SW to GPIO 2. Connect + to 3.3 V and GND to GND.
2. Connect each button between its GPIO and GND.
3. Connect the OLED VCC to 3.3 V and GND to GND. Connect SDA to GPIO 6 and SCL to GPIO 7.
4. Connect the board to the computer with the USB-C cable. The board shows as a USB serial device.

## Power

- The board gets 5 V from USB-C. The onboard regulator supplies 3.3 V to the ESP32-C3 and the peripherals.
- After a period of inactivity, the firmware turns off the display and enters deep sleep. Only the encoder button (GPIO 2 low) wakes it.
- There is no battery monitoring yet. The battery level on the display is a fixed value.
- Earlier estimates, not measured: about 100–150 mA in operation with BLE, and up to about 200 mA during BLE transmission.

## Troubleshooting

### The OLED stays blank

1. Check the SDA and SCL connections (GPIO 6 and 7).
2. Check the 3.3 V supply of the display.
3. Make sure that the display uses address `0x3C`. Scan the I2C bus if you are not sure.

### The encoder does not respond

1. Check the CLK, DT and SW connections (GPIO 1, 0 and 2).
2. Check the encoder supply.
3. With a multimeter, check that SW closes when you press it.

### The device does not pair

1. Look at the serial output for the BLE start messages.
2. Remove the old pairing on the host, then use **Bluetooth > Pair** on the device.
3. Make sure that the USB supply is stable. BLE transmission draws peak current.

### No serial port appears

1. Use a USB cable that transfers data, not a charge-only cable.
2. Hold the BOOT button while you connect the USB cable, to start the bootloader.
3. Press reset after you connect the cable.

## References

- [ESP32-C3 datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
- [ESP32-C3 technical reference manual](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)
- [SSD1306 datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [EC11 rotary encoder](https://tech.alpsalpine.com/prod/e/html/encoder/incremental/ec11/ec11_list.html)
