# User Journeys

## Journey 1: Daily Use - The Invisible Tool

**User:** Feroj (Creator/Primary User)

Feroj sits down at his desk and starts working. The KnobKoKy is there, already paired to his PC, configured exactly how he left it yesterday. He's scrolling through code in VS Code, reviewing documentation, adjusting Spotify volume during a quick break - the wheel just does what he expects without conscious thought.

He doesn't "use" the device. He just works. The only reminder it exists is when he instinctively reaches for it and it's exactly where his hand expects it to be. Hours pass. The device never demands attention, never needs "managing." It's an extension of his workflow, not an interruption to it.

**Requirements Revealed:**
- Configuration persistence across power cycles
- Instant-on operation (no setup on boot)
- Seamless BLE reconnection

---

## Journey 2: Mode Switch Mid-Workflow

**User:** Feroj (Creator/Primary User)

Feroj is deep in a coding session, using Scroll mode to navigate through files. A Slack notification pops up - his team wants to do a quick call. He reaches for the knob, long-presses to enter the menu, rotates to "Wheel Behavior," short-presses, rotates to "Volume," and short-presses to confirm.

A brief notification confirms "Volume Mode Active" - then the menu disappears and he's back to his desktop. He joins the call, adjusts volume naturally with the wheel throughout the conversation. After the call, he long-presses again, switches back to Scroll, gets confirmation, and continues coding.

The whole interaction takes a few seconds. He never lost focus on his actual work.

**Requirements Revealed:**
- Quick menu access via long-press
- Visual/audio confirmation of mode change
- Auto-exit menu after selection (return to normal operation)
- Intuitive navigation (wheel to browse, short-press to select, long-press to back/exit)

---

## Journey 3: First-Time Setup - The New Owner

**User:** Alex (Future Maker)

Alex discovers the UtilityButtonsWithKnobUSB project on GitHub while looking for a DIY volume knob. He's got an ESP32-C3 Super Mini and a rotary encoder from an old project. He clones the repo, opens the config file, and sees clear documentation for adding buttons.

He wires three buttons to GPIO 3, 4, and 5, updates the config file to define these pins, and builds the firmware using PlatformIO. The build succeeds, he flashes it, and the device boots.

The encoder works immediately as a scroll wheel. Alex pairs it with his PC and it just works. He enters the Button Configurator through the menu and assigns behaviors: Mute to button 1, Play to button 2, Next to button 3. Each button works exactly as configured.

Alex now has a personalized 3-button + knob controller. The config file edit and rebuild took 5 minutes, and he has complete control over his hardware setup.

**Requirements Revealed:**
- Clear documentation for hardware configuration
- Config file defines button GPIO pins (compile-time)
- Straightforward build process via PlatformIO
- Works out-of-box with just encoder (minimal hardware)
- Button behavior assignment through menu (runtime)
- Sensible defaults for new installations

---

## Journey 4: Hardware Modification - Adding a Button Later

**User:** Feroj (Creator/Primary User)

Feroj has been using his 4-button setup for months. He decides to add a fifth button for "Play/Pause" because he's tired of reaching for the keyboard during music sessions.

He wires a new button to GPIO 8, opens the config file, adds the new pin definition, and rebuilds the firmware. The rebuild takes under a minute. He flashes the new firmware, and the device boots with all five buttons recognized.

He enters the Button Configurator and assigns "Play" to the new button. His existing button behavior assignments are preserved - only the new button needs configuration. The whole process takes about 5 minutes.

**Requirements Revealed:**
- Config file clearly documents how to add buttons
- Firmware rebuild is fast and straightforward
- Existing behavior assignments persist across firmware updates
- New buttons appear in Button Configurator automatically after rebuild

---

## Journey 5: Troubleshooting - Something's Wrong

**User:** Feroj (Creator/Primary User)

Feroj notices one of his buttons stopped working. He enters the menu, goes to "Device Status" to check current button configurations. He sees the button is still configured for GPIO 4 with "Pause" behavior.

He checks the hardware - loose wire. He fixes it, and the button works again. The Device Status screen helped him confirm the firmware was fine, narrowing the problem to hardware.

Another time, he notices Bluetooth seems laggy. He checks Device Status and sees connection strength is weak. He moves the device closer to his PC's Bluetooth adapter and the issue resolves.

**Requirements Revealed:**
- Device Status shows current configuration state
- Connection strength visibility
- Button configuration visibility for troubleshooting
- Read-only status (no accidental changes while troubleshooting)

---

## Journey Requirements Summary

| Journey | Key Capabilities Required |
|---------|---------------------------|
| Daily Use | Config persistence, instant-on, seamless BLE |
| Mode Switch | Menu system, confirmation feedback, auto-exit |
| First-Time Setup | Clear config docs, easy build process, behavior assignment |
| Hardware Mod | Config file edit + rebuild, behavior persistence across updates |
| Troubleshooting | Device status display, config visibility |
