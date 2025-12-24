# Performance Optimization

## Memory Usage

**Check Memory Usage:**
```bash
pio run --verbose
```

Look for output:
```
RAM:   [=         ]  XX.X% (used XXXXX bytes from XXXXXX bytes)
Flash: [===       ]  XX.X% (used XXXXXX bytes from XXXXXXX bytes)
```

**Optimization Tips:**
- Use `const` for read-only data (stored in flash)
- Minimize global variables
- Use stack allocation for temporary objects
- Free dynamically allocated memory when done

## CPU Performance

**FreeRTOS Task Priorities:**
- Encoder handler: Default priority
- App event handler: Default priority
- Adjust if real-time requirements change

**Monitor with Stats:**
- Enable StatsMonitor library (future)
- Track task execution times
- Identify bottlenecks

## Power Optimization

**Current Consumption:**
- Active BLE: ~100-150 mA
- Deep sleep possible (not implemented)

**Future Improvements:**
- Implement light sleep between events
- Reduce BLE advertising interval
- Turn off OLED when idle
