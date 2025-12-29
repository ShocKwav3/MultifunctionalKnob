# Pin Configuration

## GPIO Assignments

| GPIO | Function | Connected To | Direction | Notes |
|------|----------|--------------|-----------|-------|
| **0** | Encoder DT | Rotary Encoder Channel B | Input (Pull-up) | Quadrature signal |
| **1** | Encoder CLK | Rotary Encoder Channel A | Input (Pull-up) | Quadrature signal, ISR attached |
| **2** | Encoder SW | Rotary Encoder Button | Input (Pull-up) | Press detection |
| **6** | I2C SDA | OLED Display Data | Bidirectional | I2C bus |
| **7** | I2C SCL | OLED Display Clock | Output | I2C bus |
| **-1** | Encoder VCC | (Not used) | - | Power via external supply |

**Configuration Source:** [`include/Config/encoder_config.h`](../include/Config/encoder_config.h:1)

## Pin Electrical Characteristics

- **Input Mode:** Internal pull-up resistors enabled
- **ISR Triggers:** Both rising and falling edges (quadrature decoding)
- **I2C Speed:** Standard mode (100 kHz) or Fast mode (400 kHz)
- **Button Debouncing:** Software debounce via timing (50ms short, 1000ms long)
