#include "PowerManager.h"
#include "Config/log_config.h"
#include "Config/system_config.h"
#include "Display/Model/DisplayRequest.h"

PowerManager::PowerManager()
    : lastActivityTime(millis()), currentState(PowerState::ACTIVE), warningDisplayed(false), displayQueue(nullptr) {
    LOG_DEBUG("PowerManager", "Initialized");
}

void PowerManager::resetActivity() {
    // Capture time outside critical section to minimize lock duration
    uint32_t now = millis();

    // Critical section: atomically update lastActivityTime and currentState
    // This prevents race condition where updateActivityState() reads stale lastActivityTime
    taskENTER_CRITICAL(&stateMux);
    lastActivityTime = now;
    PowerState previousState = currentState;
    currentState = PowerState::ACTIVE;
    bool shouldClearWarning = (previousState == PowerState::WARNING) && warningDisplayed;
    taskEXIT_CRITICAL(&stateMux);

    // Clear warning outside critical section only if needed (optimization + race fix)
    if (shouldClearWarning) {
        clearWarning();
    }

    LOG_DEBUG("PowerManager", "Activity reset");
}

PowerState PowerManager::getState() const {
    // Critical section for thread-safe read
    taskENTER_CRITICAL(&stateMux);
    PowerState state = currentState;
    taskEXIT_CRITICAL(&stateMux);
    return state;
}

void PowerManager::setDisplayQueue(QueueHandle_t queue) {
    displayQueue = queue;
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
    // Critical section: atomically read state, calculate new state, and commit transition
    // Mutex prevents resetActivity() from racing with state transitions
    taskENTER_CRITICAL(&stateMux);

    uint32_t activityTime = lastActivityTime;
    PowerState previousState = currentState;
    uint32_t elapsed = millis() - activityTime;

    // Calculate and apply new state atomically
    PowerState newState;
    if (elapsed >= POWER_SLEEP_THRESHOLD_MS) {
        newState = PowerState::SLEEP;
    } else if (elapsed >= POWER_WARNING_THRESHOLD_MS) {
        newState = PowerState::WARNING;
    } else {
        newState = PowerState::ACTIVE;
    }

    currentState = newState;

    // Recovery mechanism: enforce visual state consistency
    // Check if display state is out of sync with power state and needs correction
    bool displayStateOutOfSync = (newState == PowerState::WARNING && !warningDisplayed) ||
                                   (newState != PowerState::WARNING && warningDisplayed);

    // Determine what actions are needed based on committed state and sync status
    bool isTransitionToWarning = (newState == PowerState::WARNING && newState != previousState);
    bool needShowWarning = (newState == PowerState::WARNING && !warningDisplayed);  // Show on transition OR retry if failed
    bool needClearWarning = (newState != PowerState::WARNING && warningDisplayed);  // Clear if not in WARNING state but flag is set

    taskEXIT_CRITICAL(&stateMux);

    // Execute transitions outside critical section based on atomic decision
    if (needShowWarning) {
        if (isTransitionToWarning) {
            LOG_INFO("PowerManager", "State transition: ACTIVE → WARNING (elapsed: %lu ms)", elapsed);
        }
        showWarning();  // Will retry on every check cycle until successful
    } else if (needClearWarning) {
        if (newState == PowerState::SLEEP) {
            LOG_INFO("PowerManager", "State transition: WARNING → SLEEP (elapsed: %lu ms)", elapsed);
            // TODO Story 10.3: Dispatch sleep event to enter deep sleep
        } else if (newState == PowerState::ACTIVE) {
            LOG_DEBUG("PowerManager", "State transition: WARNING → ACTIVE");
        }
        clearWarning();  // Will retry on every check cycle until successful
    }
}

void PowerManager::showWarning() {
    // Thread-safe check: protect warningDisplayed with mutex
    taskENTER_CRITICAL(&stateMux);
    bool alreadyDisplayed = warningDisplayed;
    taskEXIT_CRITICAL(&stateMux);

    if (alreadyDisplayed) {
        return;  // Already showing warning
    }

    if (displayQueue == nullptr) {
        LOG_ERROR("PowerManager", "Display queue not set, cannot show warning");
        return;
    }

    // Request display to show warning
    DisplayRequest req;
    req.type = DisplayRequestType::SHOW_MESSAGE;
    req.data.message.value = SLEEP_WARNING_MESSAGE;

    // Use timeout instead of portMAX_DELAY to prevent PowerManager task blocking
    BaseType_t result = xQueueSend(displayQueue, &req, pdMS_TO_TICKS(100));
    if (result != pdPASS) {
        LOG_ERROR("PowerManager", "Failed to send warning to display queue (timeout)");
        return;  // Keep warningDisplayed=false, recovery mechanism will retry
    }

    // Only set flag after successful queue send - thread-safe update
    taskENTER_CRITICAL(&stateMux);
    warningDisplayed = true;
    taskEXIT_CRITICAL(&stateMux);

    LOG_INFO("PowerManager", "Warning displayed");
}

void PowerManager::clearWarning() {
    // Thread-safe check: protect warningDisplayed with mutex
    taskENTER_CRITICAL(&stateMux);
    bool isDisplayed = warningDisplayed;
    taskEXIT_CRITICAL(&stateMux);

    if (!isDisplayed) {
        return;  // No warning to clear
    }

    if (displayQueue == nullptr) {
        return;  // No queue configured, nothing to clear
    }

    // Request display to clear warning - CLEAR_WARNING provides better encapsulation
    DisplayRequest req;
    req.type = DisplayRequestType::CLEAR_WARNING;

    BaseType_t result = xQueueSend(displayQueue, &req, pdMS_TO_TICKS(10));
    if (result != pdPASS) {
        LOG_ERROR("PowerManager", "Failed to send clear request (queue full)");
        return;  // Keep warningDisplayed=true, recovery mechanism will retry
    }

    // Only reset flag after successful queue send - thread-safe update
    taskENTER_CRITICAL(&stateMux);
    warningDisplayed = false;
    taskEXIT_CRITICAL(&stateMux);

    LOG_DEBUG("PowerManager", "Warning cleared");
}
