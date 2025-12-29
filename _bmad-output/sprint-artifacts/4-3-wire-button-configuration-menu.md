# Story 4.3: Wire Button Configuration Menu

Status: ready-for-dev

## Story

As a **user**,
I want **the Button Config menu to show all my buttons and let me assign behaviors**,
so that **I can see and change button configurations easily**.

## Acceptance Criteria

1. **Menu Tree Update:**
   - Update `src/Menu/Model/MenuTree.h`
   - "Button Config" submenu children:
     - "Button 1" -> Submenu with behaviors
     - "Button 2" -> Submenu with behaviors
     - ... (for all `BUTTON_COUNT`)

2. **Behavior Submenus:**
   - Each button submenu has:
     - "None" -> `SetButtonBehaviorAction(i, NONE)`
     - "Mute" -> `SetButtonBehaviorAction(i, MUTE)`
     - "Play" -> `SetButtonBehaviorAction(i, PLAY)`
     - "Pause" -> `SetButtonBehaviorAction(i, PAUSE)`
     - "Next" -> `SetButtonBehaviorAction(i, NEXT)`
     - "Previous" -> `SetButtonBehaviorAction(i, PREVIOUS)`

3. **Dynamic/Static Generation:**
   - Since `MenuTree` is static/constexpr, we must define these structures explicitly or use macros/templates if possible. Given `BUTTON_COUNT` is small (4), explicit definition is acceptable and preferred for simplicity.

## Tasks / Subtasks

- [ ] Update `src/Menu/Model/MenuTree.h` to include button configuration submenus

## Dev Notes

### Architecture Compliance

- **Static Structure:** Keep it static. Define `BUTTON_1_ITEMS`, `BUTTON_2_ITEMS`, etc.

### References

- [Architecture: Component Architecture](_bmad-output/architecture/index.md#component-architecture-menu-system)
- [Epics: Story 4.3](_bmad-output/epics/index.md#story-43-wire-button-configuration-menu)

## Dev Agent Record

### Context Reference

- `_bmad-output/architecture/index.md`
- `_bmad-output/epics/index.md`

### Agent Model Used

- google/gemini-3-pro-preview
