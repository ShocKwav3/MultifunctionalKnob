# Troubleshooting Development Issues

## Build Errors

### Error: "No such file or directory"

**Cause:** Missing include path or file

**Solution:**
1. Verify file exists in `include/` or `lib/`
2. Check include path in `#include` statement
3. Rebuild from clean: `./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t clean && ./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run`

### Error: "Multiple definition of..."

**Cause:** Function/variable defined in header without `inline` or `static`

**Solution:**
1. Move implementation to `.cpp` file
2. Use `inline` for small functions in headers
3. Use `static` or anonymous namespace for file-scope variables

## Runtime Errors

### Hard Fault / Guru Meditation

**Symptoms:** Device crashes, stack trace in serial output

**Debug:**
1. Use exception decoder filter
2. Check for null pointer dereferences
3. Verify stack size for tasks
4. Look for buffer overflows

### Bluetooth Not Working

**Symptoms:** Device not advertising, pairing fails

**Debug:**
1. Check serial output for BLE initialization
2. Verify NimBLE library version
3. Check for conflicting WiFi usage
4. Increase BLE TX power if range is issue
