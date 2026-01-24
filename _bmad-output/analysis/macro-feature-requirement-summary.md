# Macro Activation System - Requirement Summary

**Date:** 2026-01-22
**Status:** Ready for PRD/Architecture
**Complexity:** Low

---

## Overview

Add a macro activation system that allows users to assign key combinations to existing inputs, triggered by holding a dedicated macro button.

## Core Requirements

### Hardware
- **New macro button** on available GPIO pin
- Press & hold to enter macro mode (modifier behavior)

### Functionality
- **Macro mode**: While macro button held, all inputs execute their assigned macro instead of normal action
- **Macro type**: Key combinations only (e.g., Ctrl+C, Cmd+Shift+4, Alt+Tab)
- **Storage**: One macro per input, persisted to NVS

### Configurable Inputs (7 total)
| Input | Macro Slot |
|-------|------------|
| Wheel Button | 1 macro |
| Wheel Left | 1 macro |
| Wheel Right | 1 macro |
| Button 1 | 1 macro |
| Button 2 | 1 macro |
| Button 3 | 1 macro |
| Button 4 | 1 macro |

### Menu Integration
New menu section under Settings:
```
Settings > Macros >
  ├── Wheel Button: [Set Combo]
  ├── Wheel Left: [Set Combo]
  ├── Wheel Right: [Set Combo]
  ├── Button 1: [Set Combo]
  ├── Button 2: [Set Combo]
  ├── Button 3: [Set Combo]
  └── Button 4: [Set Combo]
```

### User Flow for Setting a Macro
1. Navigate to Settings > Macros > [Input]
2. Select input to configure
3. Enter key combo (UI TBD - likely modifier selection + key)
4. Save to NVS

## Out of Scope
- Text string macros
- Multi-step sequences with delays
- Multiple macros per input
- External configuration (web/app)

## Dependencies
- Existing menu system
- Existing NVS/ConfigManager infrastructure
- BLE HID keyboard functionality

---

*Generated via Express Analysis - Business Analyst (Mary)*
