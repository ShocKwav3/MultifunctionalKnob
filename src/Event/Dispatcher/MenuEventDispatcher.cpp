#include "MenuEventDispatcher.h"
#include "Config/log_config.h"

static constexpr const char* TAG = "MenuEventDispatcher";

QueueHandle_t MenuEventDispatcher::eventQueue = nullptr;

void MenuEventDispatcher::init(QueueHandle_t queue) {
    eventQueue = queue;
}

void MenuEventDispatcher::dispatchActivated(const MenuItem* currentItem, uint8_t selectedIndex, uint8_t itemCount) {
    MenuEvent event{};
    event.type = MenuEventType::MENU_ACTIVATED;
    event.currentItem = currentItem;
    event.selectedIndex = selectedIndex;
    event.itemCount = itemCount;
    dispatch(event);
}

void MenuEventDispatcher::dispatchDeactivated() {
    MenuEvent event{};
    event.type = MenuEventType::MENU_DEACTIVATED;
    event.currentItem = nullptr;
    event.selectedIndex = 0;
    event.itemCount = 0;
    dispatch(event);
}

void MenuEventDispatcher::dispatchNavigationChanged(const MenuItem* currentItem, uint8_t selectedIndex, uint8_t itemCount) {
    MenuEvent event{};
    event.type = MenuEventType::MENU_NAVIGATION_CHANGED;
    event.currentItem = currentItem;
    event.selectedIndex = selectedIndex;
    event.itemCount = itemCount;
    dispatch(event);
}

void MenuEventDispatcher::dispatchItemSelected(const MenuItem* selectedItem) {
    MenuEvent event{};
    event.type = MenuEventType::MENU_ITEM_SELECTED;
    event.currentItem = selectedItem;
    event.selectedIndex = 0;
    event.itemCount = 0;
    dispatch(event);
}

void MenuEventDispatcher::dispatch(const MenuEvent& event) {
    if (eventQueue) {
        if (xQueueSend(eventQueue, &event, 0) != pdTRUE) {
            LOG_INFO(TAG, "Event queue full, event dropped");
        }
    }
}
