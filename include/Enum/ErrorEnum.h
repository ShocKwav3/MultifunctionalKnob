#pragma once

#include <cstdint>

enum class Error : uint8_t {
    OK,
    INVALID_PARAM,
    NVS_READ_FAIL,
    NVS_WRITE_FAIL,
    INVALID_STATE
};

constexpr uint8_t Error_MAX = static_cast<uint8_t>(Error::INVALID_STATE);

inline const char* errorToString(Error e) {
    switch (e) {
        case Error::OK:             return "OK";
        case Error::INVALID_PARAM:  return "INVALID_PARAM";
        case Error::NVS_READ_FAIL:  return "NVS_READ_FAIL";
        case Error::NVS_WRITE_FAIL: return "NVS_WRITE_FAIL";
        case Error::INVALID_STATE:  return "INVALID_STATE";
        default:                    return "UNKNOWN";
    }
}
