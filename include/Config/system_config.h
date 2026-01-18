#pragma once

#include <stdint.h>

// Power Management Configuration
constexpr uint32_t POWER_WARNING_THRESHOLD_MS = 240000;  // 4 minutes
constexpr uint32_t POWER_SLEEP_THRESHOLD_MS = 300000;    // 5 minutes
constexpr uint32_t POWER_CHECK_INTERVAL_MS = 1000;       // Check every 1 second
