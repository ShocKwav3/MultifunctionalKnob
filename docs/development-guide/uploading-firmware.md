# Uploading Firmware

## Upload to Device

### 1. Connect ESP32-C3 via USB

Ensure device is connected and recognized:

```bash
# List available devices
pio device list
```

### 2. Upload Firmware

**Auto-detect port:**
```bash
pio run --target upload
```

**Specific port:**
```bash
pio run --target upload --upload-port /dev/ttyACM0    # Linux/Mac
pio run --target upload --upload-port COM3            # Windows
```

### 3. Monitor Output

**Upload and Monitor (combined):**
```bash
pio run --target upload --target monitor
```

## Troubleshooting Upload

### Issue: Device Not Found

**Solutions:**
1. Check USB cable (must support data transfer)
2. Try different USB port
3. Install/update USB drivers
4. Press BOOT button while connecting (enter bootloader mode)

### Issue: Permission Denied (Linux)

**Solution:**
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER
# Log out and back in for changes to take effect
```

### Issue: Upload Fails Midway

**Solutions:**
1. Hold BOOT button during upload
2. Reset board after connecting
3. Lower upload speed in `platformio.ini`:
   ```ini
   upload_speed = 115200
   ```
