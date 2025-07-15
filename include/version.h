#pragma once

// Semantic Versioning: MAJOR.MINOR.PATCH
#define FIRMWARE_VERSION_MAJOR 0
#define FIRMWARE_VERSION_MINOR 1
#define FIRMWARE_VERSION_PATCH 0
// Optional: Build metadata or pre-release tags
#define FIRMWARE_VERSION_BUILD "" // e.g., "-beta.1" or "git-commit-hash"

// Helper macro to combine version parts into a string literal
#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#ifdef FIRMWARE_VERSION_BUILD
#define FIRMWARE_VERSION \
    STRINGIFY(FIRMWARE_VERSION_MAJOR) "." \
    STRINGIFY(FIRMWARE_VERSION_MINOR) "." \
    STRINGIFY(FIRMWARE_VERSION_PATCH) \
    FIRMWARE_VERSION_BUILD
#else
#define FIRMWARE_VERSION \
    STRINGIFY(FIRMWARE_VERSION_MAJOR) "." \
    STRINGIFY(FIRMWARE_VERSION_MINOR) "." \
    STRINGIFY(FIRMWARE_VERSION_PATCH)
#endif
