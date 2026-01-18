#include "PowerManager.h"
#include "Config/log_config.h"
#include "Config/system_config.h"

PowerManager::PowerManager()
    : lastActivityTime(millis()), currentState(PowerState::ACTIVE) {
    LOG_DEBUG("PowerManager", "Initialized");
}

void PowerManager::resetActivity() {
    // Critical section: atomically update both lastActivityTime and currentState
    // This prevents race condition where updateActivityState() reads stale lastActivityTime
    taskENTER_CRITICAL(&stateMux);
    lastActivityTime = millis();
    currentState = PowerState::ACTIVE;
    taskEXIT_CRITICAL(&stateMux);

    LOG_DEBUG("PowerManager", "Activity reset");
}

PowerState PowerManager::getState() const {
    // Critical section for thread-safe read
    taskENTER_CRITICAL(&stateMux);
    PowerState state = currentState;
    taskEXIT_CRITICAL(&stateMux);
    return state;
}

void PowerManager::start() {
    xTaskCreate(
        taskEntry,
        "PowerMgr",     // Task name
        2048,           // Stack size (bytes)
        this,           // Task parameter (this pointer)
        1,              // Priority (low priority, not time-critical)
        nullptr         // Task handle (not needed)
    );
    LOG_INFO("PowerManager", "Task started");
}

void PowerManager::taskEntry(void* param) {
    PowerManager* instance = static_cast<PowerManager*>(param);
    instance->taskLoop();
}

void PowerManager::taskLoop() {
    while (true) {
        updateActivityState();
        vTaskDelay(pdMS_TO_TICKS(POWER_CHECK_INTERVAL_MS));
    }
}

void PowerManager::updateActivityState() {
    // Critical section: atomically read lastActivityTime and currentState together
    // This prevents race where resetActivity() updates lastActivityTime after we read it
    taskENTER_CRITICAL(&stateMux);
    uint32_t activityTimeSnapshot1 = lastActivityTime;
    PowerState previousState = currentState;
    taskEXIT_CRITICAL(&stateMux);

    // Calculate elapsed time from atomic snapshot
    uint32_t elapsed = millis() - activityTimeSnapshot1;

    // Calculate new state outside critical section to minimize lock time
    PowerState newState;
    if (elapsed >= POWER_SLEEP_THRESHOLD_MS) {
        newState = PowerState::SLEEP;
    } else if (elapsed >= POWER_WARNING_THRESHOLD_MS) {
        newState = PowerState::WARNING;
    } else {
        newState = PowerState::ACTIVE;
    }

    // Critical section: re-check lastActivityTime and conditionally update state
    // If lastActivityTime changed during calculation, resetActivity() was called
    // mid-calculation and we must discard our stale result (AC5: no race conditions)
    taskENTER_CRITICAL(&stateMux);
    uint32_t activityTimeSnapshot2 = lastActivityTime;
    if (activityTimeSnapshot1 == activityTimeSnapshot2) {
        // No reset occurred during calculation - safe to apply newState
        currentState = newState;
    } else {
        // Reset occurred during calculation - discard stale calculation
        // currentState already set to ACTIVE by resetActivity()
        newState = currentState;  // Update newState for logging consistency
    }
    taskEXIT_CRITICAL(&stateMux);

    // Log only on state transitions (outside critical section)
    if (newState != previousState) {
        switch (newState) {
            case PowerState::WARNING:
                LOG_INFO("PowerManager", "State transition: ACTIVE → WARNING (elapsed: %lu ms)", elapsed);
                // TODO Story 10.2: Dispatch warning event to display handler
                break;
            case PowerState::SLEEP:
                LOG_INFO("PowerManager", "State transition: WARNING → SLEEP (elapsed: %lu ms)", elapsed);
                // TODO Story 10.3: Dispatch sleep event to enter deep sleep
                break;
            case PowerState::ACTIVE:
                LOG_DEBUG("PowerManager", "State transition: %d → ACTIVE", static_cast<int>(previousState));
                break;
        }
    }
}
