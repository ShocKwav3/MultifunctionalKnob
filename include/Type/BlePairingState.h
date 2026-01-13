#pragma once

/**
 * @brief BLE pairing state tracking
 *
 * Used to detect and handle pairing conflicts when a previously paired
 * host device attempts auto-reconnect during a new pairing attempt.
 *
 * When PairAction clears bonds and starts advertising, the host device
 * may auto-reconnect with encryption before a new pairing can occur.
 * This causes disconnect with reason 531 (encryption failure).
 */
struct BlePairingState {
    bool isInPairingMode = false;  // True when PairAction has started advertising
};
