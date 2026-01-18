#pragma once

/**
 * @brief Interface that all event handlers must implement
 *
 * Ensures consistent behavior across all input handlers:
 * - User activity tracking for power management
 * - Compile-time enforcement via pure virtual methods
 *
 * Any class that processes user input events MUST implement this interface.
 */
class EventHandlerInterface {
public:
    virtual ~EventHandlerInterface() = default;

    /**
     * @brief Notify that user activity occurred
     *
     * Must be called whenever an input event is received.
     * Implementations should reset inactivity timers or perform
     * other cross-cutting activity tracking.
     */
    virtual void notifyUserActivity() = 0;
};
