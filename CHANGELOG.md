# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial project setup with PlatformIO
- ESP32 MIDI input/output handling
- SysEx protocol implementation
- OSC network communication
- TFT display interface
- LED control system
- Input handling (buttons, encoders, expression pedals)
- JSON configuration management

### Changed
- Centralized all SysEx/MIDI protocol constants in midi.h
- Refactored midi_in.cpp and midi_out.cpp for improved maintainability
- Optimized onSysEx function with proper error handling and validation

### Fixed
- Buffer overflow protection in SysEx handling
- Memory safety improvements across all modules
- Proper error handling for network operations

## [1.0.0] - 2025-06-30

### Added
- Initial release of ESP32 ShowControl MIDI system
- Complete MIDI protocol implementation
- OSC communication support
- Multi-page display system
- Hardware abstraction layer for inputs/outputs
