#pragma once

#include <stdint.h>

class MenuAction;

/**
 * @brief Menu item structure for hierarchical menu navigation
 *
 * Represents a node in the menu tree. Branch nodes have children but no action.
 * Leaf nodes have an action but no children. Parent pointer enables upward
 * navigation.
 *
 * Designed for constexpr initialization to reside in flash/RO data.
 */
struct MenuItem {
    const char* label;              ///< Display text for this menu item
    const MenuItem* parent;         ///< Parent menu item (nullptr for root)
    const MenuItem* children;       ///< Array of child menu items (nullptr for leaf)
    uint8_t childCount;             ///< Number of children (0 for leaf nodes)
    MenuAction* action;             ///< Action to execute (nullptr for branch nodes)
};
