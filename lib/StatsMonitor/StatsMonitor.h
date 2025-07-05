#pragma once

#include "esp_heap_caps.h"
#include "Arduino.h"

void printMemoryStats() {
  Serial.println("=== Memory Info ===");
  Serial.printf("Free heap: %u bytes\n", esp_get_free_heap_size());
  Serial.printf("Minimum free heap ever: %u bytes\n", esp_get_minimum_free_heap_size());
  Serial.printf("Largest free block: %u bytes\n", heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT));
  Serial.println();
}

void printFlashStats() {
  Serial.println("=== Flash Info ===");
  Serial.printf("Sketch size: %u bytes\n", ESP.getSketchSize());
  Serial.printf("Free sketch space: %u bytes\n", ESP.getFreeSketchSpace());
  Serial.printf("Flash chip size: %u bytes\n", ESP.getFlashChipSize());
  Serial.println();
}

void printStackUsage() {
  Serial.println("=== Stack Usage ===");
  Serial.printf("Remaining stack (main loop): %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
  Serial.println();
}