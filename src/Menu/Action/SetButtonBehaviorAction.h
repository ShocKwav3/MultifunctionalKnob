#pragma once

#include "MenuAction.h"
#include "Enum/ButtonActionEnum.h"

// Forward declarations
class ConfigManager;
class ButtonEventHandler;

/**
 * @brief Menu action to assign a behavior to a button
 *
 * Context-aware action that determines which button to configure by analyzing
 * the menu navigation path. All buttons share the same behavior menu items
 * (None, Mute, Play, Pause, Next, Previous), and the button index is extracted
 * from the parent menu item label at runtime.
 *
 * Encapsulates button action assignment:
 * 1. Extract button index from menu context (parent label "Button 1" → index 0)
 * 2. Persist the button action to NVS via ConfigManager
 * 3. Invalidate ButtonEventHandler cache to force reload on next button press
 *
 * Follows the Command Pattern with context-awareness for menu actions.
 */
class SetButtonBehaviorAction : public MenuAction {
public:
    /**
     * @brief Construct a SetButtonBehaviorAction
     *
     * @param action The ButtonAction to assign (MUTE, PLAY, PAUSE, etc.)
     * @param config ConfigManager instance for NVS persistence
     * @param buttonHandler ButtonEventHandler instance to invalidate cache (optional)
     */
    explicit SetButtonBehaviorAction(ButtonAction action, ConfigManager* config, ButtonEventHandler* buttonHandler = nullptr);

    /**
     * @brief Execute the button action assignment
     *
     * Determines button index from menu navigation context, then saves the action
     * to NVS. Action assignment takes effect without restart.
     *
     * @param context The selected MenuItem (used to determine which button submenu we're in)
     */
    void execute(const MenuItem* context) override;

    /**
     * @brief Get confirmation message for the action assignment
     *
     * @return Action-specific confirmation message:
     *         - "Mute Assigned" for MUTE
     *         - "Play Assigned" for PLAY
     *         - "Pause Assigned" for PAUSE
     *         - "Next Assigned" for NEXT
     *         - "Previous Assigned" for PREVIOUS
     *         - "Button Cleared" for NONE
     */
    const char* getConfirmationMessage() override;

private:
    ButtonAction action;
    ConfigManager* configManager;
    ButtonEventHandler* buttonEventHandler;  // Optional: for cache invalidation

    /**
     * @brief Extract button index from menu navigation context
     *
     * Walks up the parent chain to find the button submenu parent (e.g., "Button 1"),
     * then extracts the numeric index from the label.
     *
     * @param context The selected MenuItem
     * @return Button index (0-3), or 0 if unable to determine
     */
    uint8_t extractButtonIndex(const MenuItem* context) const;
};
