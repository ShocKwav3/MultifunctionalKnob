#pragma once

/**
 * @brief Consolidated BLE state tracking
 *
 * Single source of truth for all BLE-related state:
 * - Connection status (connected/disconnected)
 * - Pairing mode status (used for display and pairing conflict detection)
 *
 * This replaces the previous duplication between BlePairingState.isInPairingMode
 * and the old hardwareState.bluetoothPairing fields.
 *
 * Note: No default initializers to allow use in unions (DisplayRequest).
 */
struct BleStateType {
    bool isConnected;     ///< True when BLE device is connected
    bool isPairingMode;   ///< True when actively advertising for pairing
};
