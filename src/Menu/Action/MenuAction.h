#pragma once

/**
 * @brief Abstract interface for menu actions
 *
 * Menu actions are executed when a leaf menu item is selected.
 * Implementations handle specific behaviors like changing wheel mode,
 * configuring buttons, or displaying status information.
 */
class MenuAction {
public:
    virtual ~MenuAction() = default;

    /**
     * @brief Execute the menu action
     *
     * Called when the user selects a menu item with this action.
     * Implementations should perform the action's specific behavior.
     */
    virtual void execute() = 0;

    /**
     * @brief Get optional confirmation message before execution
     *
     * @return Confirmation message to display, or nullptr if no confirmation needed
     */
    virtual const char* getConfirmationMessage() { return nullptr; }
};
