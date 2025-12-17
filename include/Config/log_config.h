#pragma once

#include <Arduino.h>

#ifndef LOG_LEVEL
#define LOG_LEVEL 2  // Default to INFO level
#endif

#if LOG_LEVEL >= 1
#define LOG_ERROR(tag, format, ...) Serial.printf("[ERROR][%s] " format "\n", tag, ##__VA_ARGS__)
#else
#define LOG_ERROR(tag, format, ...)
#endif

#if LOG_LEVEL >= 2
#define LOG_INFO(tag, format, ...) Serial.printf("[INFO][%s] " format "\n", tag, ##__VA_ARGS__)
#else
#define LOG_INFO(tag, format, ...)
#endif

#if LOG_LEVEL >= 3
#define LOG_DEBUG(tag, format, ...) Serial.printf("[DEBUG][%s] " format "\n", tag, ##__VA_ARGS__)
#else
#define LOG_DEBUG(tag, format, ...)
#endif
