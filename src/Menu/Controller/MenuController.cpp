#include "MenuController.h"
#include "Menu/Action/MenuAction.h"
#include "Event/Dispatcher/MenuEventDispatcher.h"
#include "Menu/Model/MenuTree.h"
#include "Config/log_config.h"

static constexpr const char* TAG = "MenuController";

MenuController::MenuController()
    : active(false)
    , currentItem(nullptr)
    , selectedIndex(0) {
}

bool MenuController::isActive() const {
    return active;
}

void MenuController::activate() {
    active = true;
    currentItem = MenuTree::getRoot();
    selectedIndex = 0;

    LOG_INFO(TAG, "Activated");
    emitActivated();
}

void MenuController::deactivate() {
    active = false;
    currentItem = nullptr;
    selectedIndex = 0;

    LOG_INFO(TAG, "Deactivated");
    MenuEventDispatcher::dispatchDeactivated();
}

void MenuController::handleRotation(int8_t delta) {
    if (!active || currentItem == nullptr || currentItem->childCount == 0) {
        return;
    }

    // Calculate new index with wrapping
    int16_t newIndex = static_cast<int16_t>(selectedIndex) + delta;
    uint8_t count = currentItem->childCount;

    if (newIndex < 0) {
        newIndex = count - 1;  // Wrap to end
    } else if (newIndex >= count) {
        newIndex = 0;  // Wrap to start
    }

    if (newIndex != selectedIndex) {
        selectedIndex = static_cast<uint8_t>(newIndex);
        LOG_DEBUG(TAG, "Navigation: index=%d", selectedIndex);
        emitNavigationChanged();
    }
}

void MenuController::handleSelect() {
    if (!active || currentItem == nullptr || currentItem->childCount == 0) {
        return;
    }

    const MenuItem* selected = &currentItem->children[selectedIndex];

    if (selected->childCount > 0) {
        // Branch node: enter submenu
        currentItem = selected;
        selectedIndex = 0;
        LOG_DEBUG(TAG, "Entered submenu: %s", selected->label);
        emitNavigationChanged();
    } else if (selected->action != nullptr) {
        // Leaf node with action: execute
        LOG_INFO(TAG, "Executing action: %s", selected->label);
        MenuEventDispatcher::dispatchItemSelected(selected);
        selected->action->execute();
    }
}

void MenuController::handleBack() {
    if (!active) {
        return;
    }

    if (currentItem == nullptr || currentItem->parent == nullptr) {
        // At root or no parent: exit menu
        deactivate();
    } else {
        // Navigate to parent
        const MenuItem* parent = currentItem->parent;

        // Find index of current item in parent's children for cursor position
        selectedIndex = 0;
        if (parent->children != nullptr) {
            for (uint8_t i = 0; i < parent->childCount; i++) {
                if (&parent->children[i] == currentItem) {
                    selectedIndex = i;
                    break;
                }
            }
        }

        currentItem = parent;
        LOG_DEBUG(TAG, "Back to parent: %s", currentItem->label);
        emitNavigationChanged();
    }
}

const MenuItem* MenuController::getCurrentItem() const {
    return currentItem;
}

uint8_t MenuController::getSelectedIndex() const {
    return selectedIndex;
}

void MenuController::emitNavigationChanged() {
    if (currentItem != nullptr) {
        MenuEventDispatcher::dispatchNavigationChanged(
            currentItem,
            selectedIndex,
            currentItem->childCount
        );
    }
}

void MenuController::emitActivated() {
    if (currentItem != nullptr) {
        MenuEventDispatcher::dispatchActivated(
            currentItem,
            selectedIndex,
            currentItem->childCount
        );
    }
}
