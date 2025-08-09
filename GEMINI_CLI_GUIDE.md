# GEMINI CLI GUIDE - Weather Station Modbus Project

## PROJECT OVERVIEW

This project implements a complete weather monitoring system using Modbus RTU sensors connected via RS485. The system reads wind speed (anemometer) and wind direction (wind vane) data from industrial sensors and processes the information using both Python (for computer-based monitoring) and ESP32 (for embedded autonomous operation).

## PROJECT STRUCTURE

```
weather-station-modbus/
├── python/
│   ├── leitura.py              # Main Python script for sensor reading
│   └── requirements.txt        # Python dependencies
├── esp32/
│   ├── src/
│   │   └── main.cpp           # Main ESP32 code
│   ├── include/               # Header files
│   └── platformio.ini         # PlatformIO configuration
├── docs/
│   ├── wind_direction_transmitter.md  # Wind vane manual (OCR converted)
│   ├── wind_speed_transmitter.md      # Anemometer manual (OCR converted)
│   └── wiring_diagram.md             # Hardware connection guide
├── README.md                   # Project documentation
├── requirements.txt           # Python dependencies
├── wiring_diagram.md         # Hardware connections
└── GEMINI_CLI_GUIDE.md       # This file
```

## HARDWARE SPECIFICATIONS

### Wind Speed Transmitter (Anemometer)
- Model: SN-3000-FSJT-N01
- Protocol: Modbus RTU over RS485
- Range: 0-70 m/s
- Resolution: 0.1 m/s
- Supported baud rates: 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 bps
- Default Modbus address: 1
- Register 0x0000: Instantaneous wind speed (value * 10)
- Register 0x07D0: Device address (1-254)
- Register 0x07D1: Device baud rate configuration

### Wind Direction Transmitter (Wind Vane)
- Model: SN-3000-FXJT-N01
- Protocol: Modbus RTU over RS485
- Range: 8 directions (0-360 degrees)
- Supported baud rates: 2400, 4800, 9600 bps
- Default Modbus address: 1
- Register 0x0000: Wind direction (0-7 values)
- Register 0x0001: Wind direction (0-360 degrees)

### Baud Rate Configuration Codes (Anemometer)
- 0 = 2400 bit/s
- 1 = 4800 bit/s (default)
- 2 = 9600 bit/s
- 3 = 19200 bit/s
- 4 = 38400 bit/s
- 5 = 57600 bit/s
- 6 = 115200 bit/s
- 7 = 1200 bit/s

### Wind Direction Mapping
- 0 = 0° = North Wind
- 1 = 45° = Northeast Wind
- 2 = 90° = East Wind
- 3 = 135° = Southeast Wind
- 4 = 180° = South Wind
- 5 = 225° = Southwest Wind
- 6 = 270° = West Wind
- 7 = 315° = Northwest Wind

## COMMUNICATION PROTOCOL

### Modbus RTU Parameters
- Data bits: 8
- Parity: None
- Stop bits: 1
- Error checking: CRC (Cyclic Redundancy Check)
- Function codes supported:
  - Anemometer: 03, 04, 06, 10
  - Wind vane: 03

### Frame Structure
- Initial structure: >= 4 bytes of silence
- Address code: 1 byte
- Function code: 1 byte
- Data area: N bytes
- Error check: 16-bit CRC code
- End structure: >= 4 bytes of silence

## PYTHON IMPLEMENTATION

### Key Files
- `leitura.py`: Main script for reading sensor data
- `requirements.txt`: Dependencies (pymodbus>=3.0.0, serial)

### Python Code Structure
The Python implementation uses pymodbus 3.x with the following key parameters:
- `device_id` parameter instead of deprecated `unit` or `slave`
- `count` parameter for number of registers to read
- Error handling for communication failures
- Timestamp logging for data tracking

### Current Implementation Status
- Successfully reads both sensors at 4800 baud
- Proper error handling implemented
- Uses correct pymodbus 3.x syntax
- Device IDs: Anemometer=1, Wind vane=2

## ESP32 IMPLEMENTATION

### Hardware Connections
- Serial2 (GPIO16/17) for RS485 communication
- GPIO4 for DE/RE control (MAX485)
- MAX6675 on SPI for temperature
- Analog pin for UV sensor

### Key Features
- Autonomous sensor reading
- Error logging and handling
- Temperature and UV monitoring
- Serial output with timestamps
- Modbus RTU communication via ModbusMaster library

## DEVELOPMENT HISTORY AND FIXES

### Initial Problem Resolution
- FIXED: TypeError in pymodbus due to incorrect parameter usage
- SOLUTION: Updated from `unit=` to `device_id=` parameter
- FIXED: Incorrect register reading syntax
- SOLUTION: Added `count=` parameter for register quantity

### Communication Issues Resolved
- ISSUE: Connection timeouts at higher baud rates
- CURRENT STATUS: Working stable at 4800 bps
- RECOMMENDATION: Test 9600 bps (supported by both sensors)
- NOTE: 115200 bps only supported by anemometer

### Manual Documentation Integration
- Converted PDF manuals to markdown using OCR
- Extracted complete register maps
- Documented all supported baud rates
- Identified configuration parameters

## IMPLEMENTATION GUIDELINES

### Adding New Sensors
1. Check Modbus address conflicts
2. Verify baud rate compatibility
3. Update register mapping in code
4. Add error handling for new device
5. Test communication individually before integration

### Baud Rate Optimization
1. Test 9600 bps first (compatible with both sensors)
2. For anemometer-only setups, can use up to 115200 bps
3. Configure via software tool, not code changes
4. Verify stability over time before production use

### Error Handling Strategy
1. Implement timeouts for all Modbus requests
2. Log communication errors with timestamps
3. Retry failed requests with backoff
4. Differentiate between sensor and communication errors
5. Implement graceful degradation when sensors fail

### Performance Optimization
1. Use appropriate baud rates for application needs
2. Minimize register reads per cycle
3. Implement efficient data processing
4. Consider caching for frequently accessed data

## TROUBLESHOOTING GUIDE

### Common Issues
1. No sensor response: Check RS485 wiring (A/B polarity)
2. Intermittent readings: Add 120-ohm termination resistors
3. Wrong values: Verify Modbus address configuration
4. Timeout errors: Check baud rate settings and cable quality

### Debug Steps
1. Verify physical connections
2. Test with configuration software
3. Check Modbus address conflicts
4. Validate baud rate settings
5. Monitor error logs for patterns

## FUTURE ENHANCEMENTS

### Planned Features
- Web interface for ESP32
- SD card logging
- Wind speed alerts
- Automatic calibration
- Real-time dashboard

### Scalability Considerations
- Multiple sensor support
- Network communication (WiFi/Ethernet)
- Database integration
- Remote configuration
- OTA updates for ESP32

## PROJECT MAINTENANCE

### Code Updates
- Always test baud rate changes with both sensors
- Verify Modbus communication after library updates
- Maintain compatibility with both Python and ESP32 implementations
- Document any register mapping changes

### Documentation Updates
- Update this file when adding new features
- Keep sensor specifications current
- Document any hardware changes
- Maintain troubleshooting section

---

## RECENT UPDATES

### August 9, 2025
- Created complete project structure
- Integrated OCR-converted sensor manuals
- Established GitHub repository: https://github.com/chapuleta/weather-station-modbus
- Documented complete hardware specifications
- Identified optimal baud rate configurations (9600 bps recommended)
- Created comprehensive wiring diagrams
- Established development guidelines and troubleshooting procedures
