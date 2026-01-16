#include "MenuController.h"
#include "Menu/Action/MenuAction.h"
#include "Event/Dispatcher/MenuEventDispatcher.h"
#include "Menu/Model/MenuTree.h"
#include "Config/log_config.h"

static constexpr const char* TAG = "MenuController";

MenuController::MenuController()
    : active(false)
    , viewing(false)
    , currentItem(nullptr)
    , selectedIndex(0) {
}

bool MenuController::isActive() const {
    return active;
}

bool MenuController::canNavigate() const {
    return active && !viewing && currentItem != nullptr && currentItem->childCount > 0;
}

bool MenuController::canSelect() const {
    return active && !viewing && currentItem != nullptr && currentItem->childCount > 0;
}

void MenuController::activate() {
    active = true;
    viewing = false;
    currentItem = MenuTree::getRoot();
    selectedIndex = 0;

    LOG_INFO(TAG, "Activated");
    emitActivated();
}

void MenuController::deactivate() {
    active = false;
    viewing = false;
    currentItem = nullptr;
    selectedIndex = 0;

    LOG_INFO(TAG, "Deactivated");
    MenuEventDispatcher::dispatchDeactivated();
}

void MenuController::handleRotation(int8_t delta) {
    if (!canNavigate()) {
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
    if (!canSelect()) {
        return;
    }

    const MenuItem* selected = &currentItem->children[selectedIndex];

    if (selected->childCount > 0) {
        // Branch node: enter submenu
        
        // DESIGN PATTERN: Shared Menu Items with Dynamic Parent Pointers
        // ============================================================
        // All 4 buttons share the same buttonBehaviorItems array (DRY principle).
        // Static initialization sets parent = Button 1 (default), which would cause
        // navigation bugs (Button 3 → Mute → Back would return to Button 1).
        // 
        // Solution: Update parent pointers dynamically when entering a button submenu.
        // This ensures Back button correctly returns to the button the user came from.
        // 
        // Why not fix in initMenuTree()? Because there's only ONE shared array but
        // FOUR different button contexts - parent pointer must be context-specific.
        // This is the correct embedded-friendly solution (no duplication, minimal RAM).
        if (selected->children == MenuTree::buttonBehaviorItems) {
            for (uint8_t i = 0; i < MenuTree::BUTTON_BEHAVIOR_COUNT; i++) {
                // Cast away const to update parent pointer (safe: navigation state is mutable)
                const_cast<MenuItem*>(&MenuTree::buttonBehaviorItems[i])->parent = selected;
            }
            LOG_DEBUG(TAG, "Updated shared button behavior parent pointers to: %s", selected->label);
        }
        
        currentItem = selected;
        selectedIndex = 0;
        LOG_DEBUG(TAG, "Entered submenu: %s", selected->label);
        emitNavigationChanged();
    } else if (selected->action != nullptr) {
        // Leaf node with action: execute
        LOG_INFO(TAG, "Executing action: %s", selected->label);
        MenuEventDispatcher::dispatchItemSelected(selected);
        // Pass currentItem (parent branch) as context for context-aware actions
        selected->action->execute(currentItem);

        // If action returns nullptr for confirmation (Device Status, About),
        // enter viewing mode to prevent rotation from overwriting the display
        const char* confirmation = selected->action->getConfirmationMessage();
        if (confirmation == nullptr) {
            viewing = true;
            LOG_DEBUG(TAG, "Entered viewing mode");
        } else {
            // Action completed with confirmation: redraw menu to refresh status bar
            // This ensures any state changes (like wheel mode selection) are immediately visible
            emitNavigationChanged();
        }
    }
}

void MenuController::handleBack() {
    if (!active) {
        return;
    }

    // If in viewing mode, exit viewing mode and redisplay menu
    if (viewing) {
        viewing = false;
        LOG_DEBUG(TAG, "Exited viewing mode");
        emitNavigationChanged();
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
