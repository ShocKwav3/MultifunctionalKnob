# Non-Functional Requirements

## Performance

- NFR1: Menu navigation shall feel responsive with no perceptible lag between input and display update

## Reliability

- NFR2: Saved configuration shall persist across power cycles with zero data loss
- NFR3: System shall handle invalid/corrupted configuration gracefully by falling back to defaults
- NFR4: Input events shall not be dropped under normal operation (queue-based processing)

## Maintainability

- NFR5: Code shall follow consistent naming conventions throughout the codebase
- NFR6: Architecture shall follow established patterns (event-driven, handler pattern, singleton where appropriate)
- NFR7: Display abstraction shall allow new display implementations without modifying menu logic
- NFR8: New wheel modes shall be addable by implementing a defined handler interface
- NFR9: New button behaviors shall be addable by extending the behavior registry
- NFR10: Code shall include header documentation for public interfaces
- NFR11: Build process shall be documented and reproducible via PlatformIO

## Compatibility

- NFR12: Device shall work with ESP32-C3 Super Mini board as reference hardware
