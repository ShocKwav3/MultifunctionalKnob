---
epicNumber: 14
title: "LED Strip Control"
complexity: "Medium-High"
dependencies: None
frsCovered: "FR23, FR24, FR25, FR26, FR27, FR28"
stories: 5
---

# Epic 14: LED Strip Control

**Goal:** User can control an external LED strip (power, brightness, mode, color) via the menu.

**FRs covered:** FR23, FR24, FR25, FR26, FR27, FR28

**User Outcome:** User navigates to LED menu, toggles power, adjusts brightness with wheel (seeing live preview), selects modes and colors. All settings persist to NVS.

**Implementation Notes:** Requires FastLED library. Independent of other epics but benefits from menu patterns.

---

## Functional Requirements

**Feature 8 - LED Strip Control:**
- FR23: User can toggle LED strip power (On/Off) via menu
- FR24: User can adjust LED brightness via wheel-controlled percentage
- FR25: User can select LED mode via menu (modes TBD at implementation)
- FR26: User can select LED color via menu (colors TBD at implementation)
- FR27: System persists all LED settings to NVS
- FR28: Brightness changes show real-time preview on LED strip

---

## Story 14.1: Add FastLED Library and LED Menu Structure

As a **user wanting to control LED strips**,
I want **an LED menu item with Power, Brightness, Mode, and Color sub-options**,
So that **I can access all LED settings from one place**.

**Acceptance Criteria:**

**Given** FastLED library is added to `platformio.ini`
**When** I navigate to the main menu
**Then** I see an "LED" menu item
**And** selecting it shows sub-items: "Power", "Brightness", "Mode", "Color"
**And** I can navigate back to the main menu

---

## Story 14.2: Implement LED Power Toggle with NVS Persistence

As a **user controlling the LED strip**,
I want **to toggle LED power On/Off from the menu**,
So that **I can turn the strip on or off and have it remember my choice**.

**Acceptance Criteria:**

**Given** I navigate to LED → Power
**When** I select "On" or "Off"
**Then** the LED strip turns on or off immediately
**And** the setting is saved to NVS with key "led.power"
**And** after power cycle, the LED power state is restored

---

## Story 14.3: Implement Wheel-Controlled Brightness with Live Preview

As a **user adjusting LED brightness**,
I want **to use the wheel to change brightness and see the result in real-time**,
So that **I can find the perfect brightness level visually**.

**Acceptance Criteria:**

**Given** I navigate to LED → Brightness
**When** I rotate the wheel
**Then** the brightness percentage changes (0-100%)
**And** the display shows the current percentage
**And** the LED strip updates brightness in real-time (live preview)
**When** I press the wheel button to confirm
**Then** the brightness is saved to NVS with key "led.brightness"
**And** the menu exits or returns to parent

---

## Story 14.4: Implement LED Mode Selection with NVS Persistence

As a **user wanting different LED effects**,
I want **to select an LED mode from a list**,
So that **the strip displays my preferred pattern**.

**Acceptance Criteria:**

**Given** I navigate to LED → Mode
**When** I see the list of available modes
**Then** I can scroll through modes using the wheel
**When** I select a mode
**Then** the LED strip applies the mode immediately
**And** the selection is saved to NVS with key "led.mode"
**And** after power cycle, the mode is restored

---

## Story 14.5: Implement LED Color Selection with NVS Persistence

As a **user wanting to customize LED color**,
I want **to select a color from a list**,
So that **the strip displays my preferred color**.

**Acceptance Criteria:**

**Given** I navigate to LED → Color
**When** I see the list of available colors
**Then** I can scroll through colors using the wheel
**When** I select a color
**Then** the LED strip applies the color immediately
**And** the selection is saved to NVS with key "led.color"
**And** after power cycle, the color is restored

---

## Implementation Checklist

- [ ] Add FastLED library to platformio.ini
- [ ] Create LED configuration header with pin, count, and supported modes
- [ ] Create `LEDController` class for FastLED management
- [ ] Add LED menu structure to `MenuTree.h`
- [ ] Create `LED::Power` sub-menu with On/Off options
- [ ] Implement `SetLEDPowerAction` menu action
- [ ] Create `LED::Brightness` sub-menu for wheel control
- [ ] Implement `SetLEDBrightnessAction` with live preview
- [ ] Create `LED::Mode` sub-menu with mode selection
- [ ] Implement LED mode list (suggest: Solid, Rainbow, Pulse, Strobe)
- [ ] Implement `SetLEDModeAction` menu action
- [ ] Create `LED::Color` sub-menu with color selection
- [ ] Implement LED color list (suggest: Red, Green, Blue, White, Cyan, Magenta, Yellow)
- [ ] Implement `SetLEDColorAction` menu action
- [ ] Add NVS persistence for all LED settings
- [ ] Add LED settings load/restore on boot
- [ ] Test live brightness preview while adjusting
- [ ] Test mode and color application immediately
- [ ] Test NVS persistence across power cycles
- [ ] Verify brightness range (0-100%) maps correctly to FastLED

---

## Technical Notes

- FastLED provides efficient LED control for WS2812B/NeoPixel strips
- Brightness control should be smooth (0-255 or 0-100% range)
- Live preview for brightness should update at ~10Hz for responsiveness
- Mode effects should be non-blocking (use task or timer)
- Consider caching current LED state to reduce NVS reads
- Color definition: Use CRGB structure from FastLED
- Mode cycling: Suggest using FreeRTOS task for effect animation

---

## Related Files

- `src/LED/LEDController.cpp/.h` - Create LED control class
- `src/LED/LEDConfig.h` - Define LED pin, count, supported modes/colors
- `src/Menu/Model/MenuTree.h` - Add LED menu structure
- `src/Menu/Action/SetLEDPowerAction.cpp/.h`
- `src/Menu/Action/SetLEDBrightnessAction.cpp/.h`
- `src/Menu/Action/SetLEDModeAction.cpp/.h`
- `src/Menu/Action/SetLEDColorAction.cpp/.h`
- `src/Config/ConfigManager.h` - Add LED settings persistence
- `platformio.ini` - Add FastLED library

---

## Configuration Example

```cpp
// LED Configuration (suggested pin: GPIO4, 10 LEDs)
#define LED_PIN 4
#define NUM_LEDS 10
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

// Supported modes
enum LEDMode { MODE_SOLID, MODE_RAINBOW, MODE_PULSE, MODE_STROBE };

// Supported colors  
enum LEDColor { 
    COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_WHITE,
    COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW
};
```

---

## Optional Enhancements (Post-MVP)

- Add custom color picker (HSV/RGB input)
- Add pattern speed adjustment
- Add breathing/fade effects
- Add "reactive" mode (responds to wheel/button input)
- Add synchronized animations based on device state
