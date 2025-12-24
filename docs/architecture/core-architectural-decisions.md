# Core Architectural Decisions

## Decision Priority Analysis

**Critical Decisions (Block Implementation):**
- NVS persistence structure
- Menu state machine pattern
- Event routing for menu activation
- Display update mechanism

**Important Decisions (Shape Architecture):**
- Action execution pattern
- Button configuration structure
- Default values strategy

**Deferred Decisions (Post-MVP):**
- OTA update mechanism
- Battery power management
- Multi-profile support

## Data Architecture

| Decision | Choice | Rationale |
|----------|--------|-----------|
| NVS Namespace | Single namespace with prefixed keys | Organized without namespace-switching overhead |
| Default Values | Defaults struct with factory reset | Minimize NVS writes, explicit reset capability |
| Enum Storage | uint8_t with validation wrapper | Compact storage with graceful fallback on corruption |

**NVS Key Structure:**
```
Namespace: "knobkoky"
Keys: "wheel.mode", "btn0.action", "btn1.action", "btn2.action", "btn3.action"
```

**Validation Pattern:**
```cpp
template<typename EnumT>
EnumT readEnumConfig(const char* key, EnumT defaultVal, EnumT maxValid) {
    uint8_t stored = preferences.getUChar(key, static_cast<uint8_t>(defaultVal));
    return (stored <= static_cast<uint8_t>(maxValid))
           ? static_cast<EnumT>(stored)
           : defaultVal;
}
```

## Component Architecture (Menu System)

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Menu State Machine | Static tree structure with parent pointers | No dynamic allocation, compile-time menu definition |
| Mode Transition | Event interceptor pattern | Menu intercepts events at higher level, clean separation from mode system |
| Action Execution | Command pattern with action objects | Flexible, extensible, clean separation of concerns |

**Menu Tree Structure:**
```cpp
struct MenuItem {
    const char* label;
    MenuItem* parent;
    MenuItem* const* children;
    uint8_t childCount;
    MenuAction* action;
};
```

**Event Interceptor Pattern:**
```cpp
void handleEncoderEvent(EncoderInputEvent& event) {
    if (menuController.isActive()) {
        menuController.handleEvent(event);
        return;  // Menu consumes event
    }
    currentModeHandler->handleEvent(event);
}
```

**Command Pattern for Actions:**
```cpp
class MenuAction {
public:
    virtual ~MenuAction() = default;
    virtual void execute() = 0;
    virtual const char* getConfirmationMessage() const { return nullptr; }
};

class SelectWheelModeAction : public MenuAction {
    WheelMode mode;
public:
    SelectWheelModeAction(WheelMode m) : mode(m) {}
    void execute() override { /* save to NVS, update mode manager */ }
    const char* getConfirmationMessage() const override { return "Mode saved"; }
};
```

## Communication Patterns

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Event Architecture | Distributed Pipelines (Option B) | Decouples domains (Menu, System), adheres to SRP |
| Display Access | Arbitration via DisplayTask | Prevents race conditions on shared display resource |
| Menu Events | Dedicated MenuEventQueue | Separates UI logic from system state |
| Button Config | Array of pin definitions with metadata | Easy to iterate, adding button = one line change |

**Event Pipelines:**
1. **Menu Pipeline:** `MenuController` → `MenuEventDispatcher` → `MenuEventQueue` → `MenuEventHandler`
2. **System Pipeline:** `SystemComponents` → `SystemEventDispatcher` → `SystemEventQueue` → `SystemEventHandler`
3. **Display Pipeline:** `*Handler` → `DisplayRequestQueue` → `DisplayTask` (Arbitrator)

**Display Arbitration Pattern:**
```cpp
// All handlers must use this struct to request drawing
struct DisplayRequest {
    enum Type { DRAW_MENU, SHOW_STATUS, CLEAR } type;
    union {
        struct { const char* title; const char** items; uint8_t count; uint8_t selected; } menu;
        struct { const char* key; const char* value; } status;
    } data;
};
// Only DisplayTask consumes this queue and touches hardware
```

**Button Configuration:**
```cpp
// include/Config/button_config.h (✅ IMPLEMENTED)
struct ButtonConfig {
    uint8_t pin;
    const char* label;
    bool activeLow;
};

inline constexpr ButtonConfig BUTTONS[] = {
    {3, "Button 1", true},
    {4, "Button 2", true},
    {5, "Button 3", true},
    {9, "Button 4", true}  // GPIO 8 is strapping pin, using GPIO 9
};

inline constexpr size_t BUTTON_COUNT = sizeof(BUTTONS) / sizeof(BUTTONS[0]);
inline constexpr uint32_t DEBOUNCE_MS = 50;
```

## Decision Impact Analysis

**Implementation Sequence:**
1. Display abstraction (DisplayInterface + SerialDisplay)
2. Display Arbitration (DisplayRequestQueue + DisplayTask)
3. NVS persistence wrapper (ConfigManager)
4. Button configuration and GPIO setup
5. Menu data structures (MenuItem tree)
6. MenuAction classes
7. MenuController with event interception
8. MenuEventHandler and SystemEventHandler implementation
9. Integration with existing EncoderEventHandler

**Cross-Component Dependencies:**
- MenuController depends on MenuEventDispatcher
- MenuEventHandler depends on DisplayRequestQueue
- DisplayTask depends on DisplayInterface
