# Microcontroller Specifications

## ESP32-C3 Super Mini (Nologo Variant)

**Core Specifications:**
- **SoC:** Espressif ESP32-C3
- **CPU:** 32-bit RISC-V single-core @ 160 MHz
- **RAM:** 400 KB SRAM
- **Flash:** 4 MB (typical)
- **Wireless:** 2.4 GHz WiFi + Bluetooth 5 (BLE)
- **USB:** Built-in USB Serial/JTAG (no external USB-UART chip needed)
- **Form Factor:** Super Mini (~22.5mm x 18mm)
- **Supply Voltage:** 3.3V (USB 5V with onboard regulator)

**Key Features:**
- Native USB support (USB CDC, DFU)
- Hardware AES, SHA, RSA acceleration
- Low power consumption
- 22 programmable GPIOs
- Built-in antenna

**Board Definition:** [`boards/nologo_esp32c3_super_mini.json`](../boards/nologo_esp32c3_super_mini.json:1)
