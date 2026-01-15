#pragma once

#include "MenuAction.h"
#include "Enum/WheelModeEnum.h"

// Forward declarations
class ConfigManager;
class EncoderModeManager;
struct HardwareState;

/**
 * @brief Menu action to select and activate a wheel mode
 *
 * Encapsulates the user's wheel mode selection:
 * 1. Persist the mode selection to NVS via ConfigManager
 * 2. Apply the mode immediately to EncoderModeManager
 * 3. Update HardwareState to reflect the mode change for display
 *
 * Follows the Command Pattern for menu actions.
 * Dependencies are injected via constructor (Dependency Inversion Principle).
 */
class SelectWheelModeAction : public MenuAction {
public:
    /**
     * @brief Construct a SelectWheelModeAction
     *
     * @param mode The target WheelMode to activate (SCROLL, VOLUME, or ZOOM)
     * @param config ConfigManager instance for NVS persistence
     * @param modeMgr EncoderModeManager instance for runtime mode switching
     * @param hwState HardwareState instance for updating display state
     */
    explicit SelectWheelModeAction(WheelMode mode, ConfigManager* config, EncoderModeManager* modeMgr, HardwareState* hwState);

    /**
     * @brief Execute the wheel mode change
     *
     * Saves the mode to NVS and applies it to the mode manager immediately.
     * Mode change takes effect without restart.
     *
     * @param context The MenuItem that was selected (unused for wheel mode - mode is fixed at construction)
     */
    void execute(const MenuItem* context) override;

    /**
     * @brief Get confirmation message for the mode change
     *
     * @return Mode-specific confirmation message:
     *         - "Scroll Mode Active" for SCROLL
     *         - "Volume Mode Active" for VOLUME
     *         - "Zoom Mode Active" for ZOOM
     */
    const char* getConfirmationMessage() override;

private:
    WheelMode targetMode;
    ConfigManager* configManager;
    EncoderModeManager* modeManager;
    HardwareState* hardwareState;  // Injected dependency for display state updates
};
