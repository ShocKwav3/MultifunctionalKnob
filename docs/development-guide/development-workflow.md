# Development Workflow

## Typical Development Cycle

> **For AI Tools:** Use the pio-wrapper skill for all `pio run` commands. Load [`.claude/skills/pio-wrapper/SKILL.md`](../../.claude/skills/pio-wrapper/SKILL.md) for context.

1. **Edit Code**
   - Modify source files in `src/` or `lib/`
   - Update configuration in `include/Config/`

2. **Build**
   ```bash
   ./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run
   ```

3. **Upload**
   ```bash
   ./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run --target upload
   ```

4. **Test**
   ```bash
   pio device monitor --baud 460800
   ```

5. **Iterate**
   - Fix issues
   - Repeat from step 1

## Quick Development Commands

**Build + Upload + Monitor:**
```bash
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t upload && pio device monitor
```

**Clean + Build + Upload:**
```bash
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t clean && ./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t upload
```

## Code Organization Best Practices

### File Structure

**Headers:**
- Place in `include/` directory
- Use `#pragma once` for include guards
- Group by functionality (Config, Type, Enum, etc.)

**Implementation:**
- Place `.cpp` files in `src/` or `lib/<LibraryName>/`
- Match header file names

**Libraries:**
- Custom reusable code in `lib/`
- Each library has own directory

### Naming Conventions

**Current Conventions:**
- Classes: PascalCase (`EncoderDriver`, `AppEventHandler`)
- Files: Match class names (`EncoderDriver.h`, `EncoderDriver.cpp`)
- Constants: UPPER_SNAKE_CASE (`ENCODER_PIN_A`, `SCREEN_WIDTH`)
- Functions: camelCase (`handleRotate`, `setOnValueChange`)
- Enums: Scoped enums in namespace (`EventEnum::EncoderInputEventTypes`)

### Code Style

**Indentation:** 4 spaces (consistent throughout project)

**Braces:** Opening brace on same line for functions

**Comments:** Use descriptive variable/function names, add comments for complex logic

## Adding New Features

### Example: Add New Encoder Mode

1. **Create Mode Handler:**
   ```cpp
   // src/EncoderMode/Handler/EncoderModeHandlerCustom.h
   #pragma once
   #include "EncoderModeHandlerAbstract.h"
   #include "BleKeyboard.h" // Include for BleKeyboard type

   class EncoderModeHandlerCustom : public EncoderModeHandlerAbstract {
   public:
       EncoderModeHandlerCustom(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard); // Updated constructor
       void handleRotate(int delta) override;
       void handleShortClick() override;
       const char* getModeName() const override;
   };
   ```

2. **Implement Handler:**
   ```cpp
   // src/EncoderMode/Handler/EncoderModeHandlerCustom.cpp
   #include "EncoderModeHandlerCustom.h"

   EncoderModeHandlerCustom::EncoderModeHandlerCustom(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard) // Updated constructor
       : EncoderModeHandlerAbstract(dispatcher), _bleKeyboard(bleKeyboard) {} // Initialize BleKeyboard

   void EncoderModeHandlerCustom::handleRotate(int delta) {
       // Custom rotation logic, using _bleKeyboard for BLE operations
   }

   void EncoderModeHandlerCustom::handleShortClick() {
       // Custom click logic
   }

   const char* EncoderModeHandlerCustom::getModeName() const {
       return "Custom Mode";
   }
   ```
   *Note: Add `BleKeyboard* _bleKeyboard;` to `EncoderModeHandlerCustom.h` as a private member to store the injected instance.*

3. **Add Mode to Enum:**
   ```cpp
   // include/Enum/EventEnum.h
   enum class EncoderModeEventTypes {
       ENCODER_MODE_SCROLL,
       ENCODER_MODE_VOLUME,
       ENCODER_MODE_CUSTOM,  // Add here
       __ENCODER_MODE_SELECTION_LIMIT,
       // ...
   };
   ```

4. **Register in main.cpp:**
   ```cpp
   static EncoderModeHandlerCustom encoderModeHandlerCustom(&appDispatcher);
   encoderModeManager.registerHandler(
       EventEnum::EncoderModeEventTypes::ENCODER_MODE_CUSTOM,
       &encoderModeHandlerCustom
   );
   ```

5. **Build and Test:**
   ```bash
   ./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t upload && pio device monitor
   ```
