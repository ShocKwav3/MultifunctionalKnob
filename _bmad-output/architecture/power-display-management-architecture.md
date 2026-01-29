# Power & Display Management Architecture

**Epic:** 13 - Configurable Power & Display Management
**Date:** 2026-01-29
**Status:** Architectural Design

---

## Overview

This document defines the architectural patterns for configurable power and display management, introducing **two independent timer systems** that control device sleep and OLED display power separately.

**Key Principle:** Display timeout and sleep timer are **completely independent** subsystems that can be configured separately to support various power-saving strategies.

---

## Architectural Decisions

### Decision 1: Independent Timer Architecture

**Choice:** Two separate, parallel timer systems

**Rationale:**
- Display power consumption is the dominant power drain (OLED backlight + pixel current)
- Users may want device always ready (sleep=Never) but save OLED power (display timeout=60s)
- Separation of concerns: Display management vs system power management
- Enables flexible power strategies (see behavior matrix below)

**Implementation:**
- Each timer tracks inactivity independently
- Both timers reset on any user activity (buttons, encoder, menu)
- Display timeout does NOT trigger sleep timer countdown
- Sleep timer operates regardless of display state

---

### Decision 2: Display Power Control Pattern

**Choice:** Direct OLED power on/off commands (not backlight dimming)

**Rationale:**
- SSD1306 OLED controllers support hardware display on/off commands
- Power off maintains framebuffer contents (instant wake with content intact)
- True power saving (display off ≈ 0mA, display on ≈ 20-40mA depending on content)
- No progressive dimming needed (instant on/off is acceptable UX)

**Implementation:**
```cpp
// OLED power control via I2C commands
display.ssd1306_command(SSD1306_DISPLAYOFF);  // Power off, framebuffer preserved
display.ssd1306_command(SSD1306_DISPLAYON);   // Power on, framebuffer restored
```

---

### Decision 3: Configuration Storage

**Choice:** Two new NVS keys with uint type storage

**NVS Keys:**
- `"sleep_timer"` - uint8_t (minutes, 0 = Never, default = 5)
- `"display_timeout"` - uint16_t (seconds, 0 = Never, default = 60)

**Rationale:**
- Separate keys for separate concerns
- Value 0 consistently means "Never" across both timers
- uint8_t for sleep (max 255 minutes = 4.25 hours, sufficient)
- uint16_t for display (supports wide second range, future-proof)

**Default Values:**
- Sleep timer: 5 minutes (maintains current behavior)
- Display timeout: 60 seconds (recommended for OLED lifespan)

---

## System Architecture

### Timer Lifecycle

```
┌─────────────────────────────────────────────────────────┐
│                    USER ACTIVITY                         │
│         (button press, encoder, menu interaction)        │
└────────────┬────────────────────────────────┬───────────┘
             │                                │
             ▼                                ▼
   ┌─────────────────┐              ┌──────────────────┐
   │ Display Timeout │              │   Sleep Timer    │
   │     Timer       │              │                  │
   │   (Independent) │              │  (Independent)   │
   └────────┬────────┘              └────────┬─────────┘
            │                                │
            │ Timeout                        │ Timeout
            ▼                                ▼
   ┌─────────────────┐              ┌──────────────────┐
   │  Display Power  │              │   Deep Sleep     │
   │      OFF        │              │   (Device OFF)   │
   │ (Device AWAKE)  │              │                  │
   └────────┬────────┘              └──────────────────┘
            │
            │ Any Input
            ▼
   ┌─────────────────┐
   │  Display Power  │
   │      ON         │
   │  (Resume Work)  │
   └─────────────────┘
```

---

## Behavior Matrix

| Display Timeout | Sleep Timer | Result Behavior |
|----------------|-------------|-----------------|
| 60s | 5min | Display off after 60s; device sleeps at 5min |
| 30s | Never | Display off after 30s; device never sleeps (battery saver for always-ready use) |
| Never | 5min | Display always on; device sleeps at 5min (demo mode) |
| Never | Never | Display and device always on (USB-powered workstation mode) |
| 300s | 1min | Sleep occurs first (at 1min); display never times out independently |

**Critical Pattern:** Whichever timer expires first triggers its action. Both timers run in parallel, starting from the same user activity event.

---

## Implementation Patterns

### Pattern 1: Timer Management

**Inactivity Timer Structure:**
```cpp
struct InactivityTimer {
    uint32_t lastActivityMs;      // millis() at last activity
    uint32_t timeoutMs;            // Configured timeout in milliseconds
    bool enabled;                  // false if timeout = 0 (Never)

    bool isExpired() const {
        if (!enabled) return false;
        return (millis() - lastActivityMs) >= timeoutMs;
    }

    void reset() {
        lastActivityMs = millis();
    }

    uint32_t getRemainingSeconds() const {
        if (!enabled) return UINT32_MAX;
        uint32_t elapsed = millis() - lastActivityMs;
        if (elapsed >= timeoutMs) return 0;
        return (timeoutMs - elapsed) / 1000;
    }
};
```

**Global Timer Instances:**
```cpp
InactivityTimer displayTimeoutTimer;
InactivityTimer sleepTimer;
```

---

### Pattern 2: Activity Reset Hook

**All Input Handlers Must Call:**
```cpp
void resetInactivityTimers() {
    displayTimeoutTimer.reset();
    sleepTimer.reset();

    // If display was off, turn it back on
    if (!displayPowerState) {
        turnDisplayOn();
    }
}
```

**Hook Locations:**
- `EncoderEventHandler::handle()` - Before processing event
- `ButtonEventHandler::handle()` - Before processing event
- `MenuController::handleInput()` - On any menu interaction

---

### Pattern 3: Timer Check Loop

**Main Loop Responsibilities:**
```cpp
void loop() {
    // Check display timeout
    if (displayTimeoutTimer.isExpired() && displayPowerState) {
        turnDisplayOff();
    }

    // Check sleep countdown (60s warning)
    if (sleepTimer.isEnabled() && sleepTimer.getRemainingSeconds() <= 60) {
        if (displayPowerState && !countdownActive) {
            startSleepCountdown();
        }
    }

    // Check sleep timer
    if (sleepTimer.isExpired()) {
        enterDeepSleep();
    }

    // ... other loop tasks
}
```

---

### Pattern 4: Display Power Control

**Display Power Manager:**
```cpp
class DisplayPowerManager {
private:
    bool displayOn;
    Adafruit_SSD1306& display;

public:
    void turnOff() {
        if (!displayOn) return;
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        displayOn = false;
        LOG_INFO("Display", "Power OFF");
    }

    void turnOn() {
        if (displayOn) return;
        display.ssd1306_command(SSD1306_DISPLAYON);
        displayOn = true;
        LOG_INFO("Display", "Power ON");
        // Note: Framebuffer contents preserved, no redraw needed
    }

    bool isOn() const { return displayOn; }
};
```

---

### Pattern 5: Configuration Loading

**Initialization Sequence (main.cpp):**
```cpp
void setup() {
    // ... existing init ...

    // Load power configuration from NVS
    uint8_t sleepMinutes = configManager.loadSleepTimer();  // Default: 5
    uint16_t displaySeconds = configManager.loadDisplayTimeout();  // Default: 60

    // Configure timers
    if (sleepMinutes == 0) {
        sleepTimer.enabled = false;
    } else {
        sleepTimer.enabled = true;
        sleepTimer.timeoutMs = sleepMinutes * 60 * 1000;
    }

    if (displaySeconds == 0) {
        displayTimeoutTimer.enabled = false;
    } else {
        displayTimeoutTimer.enabled = true;
        displayTimeoutTimer.timeoutMs = displaySeconds * 1000;
    }

    // Initialize timer start times
    displayTimeoutTimer.reset();
    sleepTimer.reset();
}
```

---

## NVS Configuration API

**ConfigManager Extensions:**
```cpp
// Sleep Timer
uint8_t ConfigManager::loadSleepTimer() {
    return preferences.getUChar("sleep_timer", 5);  // Default 5 minutes
}

Error ConfigManager::saveSleepTimer(uint8_t minutes) {
    if (!preferences.putUChar("sleep_timer", minutes)) {
        return Error::NVS_WRITE_FAILED;
    }
    return Error::SUCCESS;
}

// Display Timeout
uint16_t ConfigManager::loadDisplayTimeout() {
    return preferences.getUShort("display_timeout", 60);  // Default 60 seconds
}

Error ConfigManager::saveDisplayTimeout(uint16_t seconds) {
    if (!preferences.putUShort("display_timeout", seconds)) {
        return Error::NVS_WRITE_FAILED;
    }
    return Error::SUCCESS;
}
```

---

## Sleep Countdown Architecture

**Countdown Trigger:**
- When sleep timer reaches 60 seconds remaining
- AND display is currently on
- Start countdown display

**Countdown Update:**
- Update display every 1 second
- Format: "Sleeping in M:SS" (e.g., "Sleeping in 1:00", "0:59", "0:58"...)
- Clear countdown on any user activity
- Clear countdown if display times out during countdown

**Countdown State:**
```cpp
struct CountdownState {
    bool active;
    uint32_t startTimeMs;

    void start() {
        active = true;
        startTimeMs = millis();
    }

    void stop() {
        active = false;
    }

    uint8_t getRemainingSeconds() const {
        if (!active) return 0;
        uint32_t elapsed = (millis() - startTimeMs) / 1000;
        return (elapsed < 60) ? (60 - elapsed) : 0;
    }
};
```

---

## Integration Points

### With Menu System
- Menu activity resets both timers
- Display timeout disabled while menu is active (menu keeps display on)
- Sleep timer continues running during menu (user can see countdown)

### With Encoder/Button Handlers
- All input handlers call `resetInactivityTimers()` before processing
- Display wake happens first, then input processing
- No special-casing needed in individual handlers

### With Deep Sleep
- Sleep timer expiration triggers `esp_deep_sleep_start()`
- Display automatically turns off as part of deep sleep
- Wake-up source: GPIO (wheel button press)

---

## Anti-Patterns to Avoid

❌ **DON'T:** Make display timeout depend on sleep timer
- They are independent systems
- Display off ≠ device sleep

❌ **DON'T:** Reset only one timer on activity
- Always reset both timers together
- User activity is system-wide, not subsystem-specific

❌ **DON'T:** Clear framebuffer when turning display off
- SSD1306 preserves framebuffer in hardware
- Display content should restore instantly on wake

❌ **DON'T:** Use progressive dimming before display off
- Instant off/on is acceptable UX for OLED
- Progressive dimming adds complexity without benefit

❌ **DON'T:** Block during countdown updates
- Use non-blocking timer checks in main loop
- Countdown update should not delay input processing

---

## Testing Strategy

### Unit Tests
- Timer expiration logic
- "Never" configuration handling (0 value)
- Remaining time calculation
- Reset behavior

### Integration Tests
- Display timeout independent of sleep
- Both timers reset on activity
- Display wake on input during timeout
- Sleep countdown during display timeout

### Manual Tests
- Configure various combinations from behavior matrix
- Verify power consumption differences (display on vs off)
- Verify OLED lifespan improvement with timeout enabled

---

## Performance Characteristics

**Memory Impact:**
- Timer structures: ~16 bytes each (32 bytes total)
- NVS keys: 3 bytes total
- Display power manager: minimal (bool + reference)

**CPU Impact:**
- Timer checks: ~10 CPU cycles per loop iteration (negligible)
- Display power commands: ~1-2ms I2C transaction (acceptable)
- Countdown update: ~50ms display write (1Hz, acceptable)

**Power Savings:**
- Display off: ~20-40mA saved (OLED dependent)
- Deep sleep: ~300-350mA saved (full ESP32 power down)

---

## Related Documents

- Epic: `_bmad-output/project-planning-artifacts/epics/epic-13-power-display-configuration.md`
- Stories: `_bmad-output/implementation-artifacts/story-13-*.md`
- PRD: `_bmad-output/project-planning-artifacts/prd.md` (FR21-26)

---
