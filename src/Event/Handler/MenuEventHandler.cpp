#include "MenuEventHandler.h"
#include "Menu/Model/MenuItem.h"
#include "Config/log_config.h"
#include "state/HardwareState.h"

MenuEventHandler::MenuEventHandler(QueueHandle_t menuEventQueue, QueueHandle_t displayRequestQueue, HardwareState* hwState)
    : menuEventQueue(menuEventQueue)
    , displayRequestQueue(displayRequestQueue)
    , hardwareState(hwState) {
}

void MenuEventHandler::start(uint32_t stackSize, UBaseType_t priority) {
    xTaskCreate(taskEntry, "MenuEventHandler", stackSize, this, priority, nullptr);
    LOG_INFO(TAG, "Started");
}

void MenuEventHandler::taskEntry(void* param) {
    static_cast<MenuEventHandler*>(param)->taskLoop();
}

void MenuEventHandler::taskLoop() {
    MenuEvent event;

    while (true) {
        if (xQueueReceive(menuEventQueue, &event, portMAX_DELAY) == pdTRUE) {
            switch (event.type) {
                case MenuEventType::MENU_ACTIVATED:
                    handleMenuActivated(event);
                    break;

                case MenuEventType::MENU_DEACTIVATED:
                    handleMenuDeactivated();
                    break;

                case MenuEventType::MENU_NAVIGATION_CHANGED:
                    handleNavigationChanged(event);
                    break;

                case MenuEventType::MENU_ITEM_SELECTED:
                    handleItemSelected(event);
                    break;
            }
        }
    }
}

void MenuEventHandler::handleMenuActivated(const MenuEvent& event) {
    LOG_DEBUG(TAG, "Menu activated");
    sendDrawMenuRequest(event);
}

void MenuEventHandler::handleMenuDeactivated() {
    LOG_DEBUG(TAG, "Menu deactivated");
    sendDrawNormalModeRequest();
}

void MenuEventHandler::handleNavigationChanged(const MenuEvent& event) {
    LOG_DEBUG(TAG, "Navigation changed: index=%d", event.selectedIndex);
    sendDrawMenuRequest(event);
}

void MenuEventHandler::handleItemSelected(const MenuEvent& event) {
    LOG_DEBUG(TAG, "Item selected: %s",
              event.currentItem ? event.currentItem->label : "null");
    // Action execution is handled by MenuController
    // Here we could show confirmation if needed
}

void MenuEventHandler::sendDrawMenuRequest(const MenuEvent& event) {
    if (event.currentItem == nullptr) {
        return;
    }

    DisplayRequest request{};
    request.type = DisplayRequestType::DRAW_MENU;
    request.data.menu.title = event.currentItem->label;
    request.data.menu.count = event.itemCount;
    request.data.menu.selected = event.selectedIndex;
    request.data.menu.hwState = *hardwareState;  // Pass hardware state for menu status bar

    // Populate menu items from children
    const MenuItem* children = event.currentItem->children;
    uint8_t count = event.itemCount;

    if (count > DISPLAY_MAX_MENU_ITEMS) {
        count = DISPLAY_MAX_MENU_ITEMS;
    }

    for (uint8_t i = 0; i < count; i++) {
        request.data.menu.items[i] = children[i].label;
    }

    // Clear remaining slots
    for (uint8_t i = count; i < DISPLAY_MAX_MENU_ITEMS; i++) {
        request.data.menu.items[i] = nullptr;
    }

    request.data.menu.count = count;

    if (xQueueSend(displayRequestQueue, &request, 0) != pdTRUE) {
        LOG_INFO(TAG, "Display queue full, menu request dropped");
    }
}

void MenuEventHandler::sendClearRequest() {
    DisplayRequest request{};
    request.type = DisplayRequestType::CLEAR;

    if (xQueueSend(displayRequestQueue, &request, 0) != pdTRUE) {
        LOG_INFO(TAG, "Display queue full, clear request dropped");
    }
}

void MenuEventHandler::sendDrawNormalModeRequest() {
    DisplayRequest request{};
    request.type = DisplayRequestType::DRAW_NORMAL_MODE;
    request.data.normalMode.hwState = *hardwareState;

    if (xQueueSend(displayRequestQueue, &request, 0) != pdTRUE) {
        LOG_INFO(TAG, "Display queue full, normal mode request dropped");
    }
}
