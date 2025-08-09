# Polycarbon Wind Direction Transmitter (485type) SN-3000-FXJT-N01 Ver 2.0

## Table of contents
- Chapter 1 Product Introduction
  - 1.1 Product Overview
  - 1.2 Features
  - 1.3 Main parameters
  - 1.4 System Framework Diagram
- Chapter 2 Hardware Connection
  - 2.1 Equipment inspection before installation
  - 2.2 Interface Description
  - 2.2.1 Sensor Wiring
  - 2.3 Installation
  - 2.4 Notes
- Chapter 3 Configuration Software Installation and Usage
  - 3.1 Sensor connected to computer
  - 3.2 Use of sensor monitoring software
- Chapter 4 Communication Protocol
  - 4.1 Basic communication parameters
  - 4.2 Data frame format definition
  - 4.3 Register Address
  - 4.4 Conversion relationship between numerical values
  - 4.5 Communication protocol examples and explanations
- Chapter 5 Common Problems and Solutions

---

## No. 1: Product Introduction

### 1.1 Product Overview
The wind direction sensor is small and light, easy to carry and assemble. The three-cup design concept can effectively obtain external environmental information. Polycarbonate composites the external part is electroplated and sprayed with plastic, which has good anti-corrosion and anti-erosion characteristics, ensuring that the instrument will not rust for a long time. At the same time, the smooth internal bearing system ensures the accuracy of information collection. It is widely used in wind direction measurement in greenhouses, environmental protection, meteorological stations, ships, docks, aquaculture and other environments.

### 1.2 Features
- Range: 8 Directions
- Anti-electromagnetic interference processing
- Adopt high-performance imported bearings, small rotation resistance and accurate measurement
- Polycarbonate shell, strong mechanical strength, high hardness, corrosion resistance, no rust, can be used outdoors for a long time
- The structure and weight of the equipment are carefully designed and distributed, with small moment of inertia and sensitive response.
- standard ModBus-RTU Communication protocol, easy access

### 1.3 Main parameters
| Parameter | Value |
|---|---|
| DC power supply (default) | 10~30V DC |
| Power consumption | ≤0.15W |
| Transmitter circuit operating temperature | -40°C~+60℃, 0%RH~80%RH |
| Communication interface | 485 Communication (Modbus) protocol, Baud Rate: 2400, 4800(default), 9600, Data bit length: 8Bit, Parity check mode: None, Stop bit length: 1Bit, default ModBus Correspondence address: 1 |
| Supported function codes | 03 |
| Parameter settings | Use the provided configuration software to 485 Interface configuration |
| Measuring range | 8 Directions |
| Dynamic response speed | <0.5s |

---

## No. 2: Hardware Hookup

### 2.1 Equipment pre-installation inspection
- Transmitter equipment 1 tower
- Mounting Screws 4 indivual
- Certificate of conformity, warranty card

### 2.2 Interface Description
Wide voltage power input 10~30V Both are acceptable. 485 Pay attention when wiring the signal line A\B The two wires cannot be connected reversely, and the addresses of multiple devices on the bus cannot conflict.

### 2.2.1 Sensor Wiring
| Wire | Line Color | Illustrate |
|---|---|---|
| electricity source | brown | Power supply positive (10~30V DC) |
| | black | Negative power supply |
| Pass letter | Yellow (green) | 485-A |
| | blue | 485-B |

### 2.3 Installation
Flange installation and threaded flange connection make the lower pipe of the wind direction sensor firmly fixed on the flange plate and chassis. Ø80mm, exist Ø68mm Open four equal Ø4.5mm The mounting holes are used to fix it tightly on the bracket with bolts to keep the whole set of instruments at the best level and ensure the accuracy of wind direction data. The flange connection is easy to use and can withstand greater pressure.

**Note:** When installing, make sure the arrow on the sensor points to the north to avoid measurement errors.

### 2.4 Notes
1. Users are not allowed to disassemble or touch the sensor core to avoid damage to the product.
2. Try to stay away from high-power interference equipment to avoid inaccurate measurements, such as frequency converters, motors, etc. When installing or removing the transmitter, the power supply must be disconnected first. Water entering the transmitter may cause irreversible changes.
3. Prevent chemical reagents, oil, dust, etc. from directly damaging the sensor. Do not use it for a long time in an environment with condensation or extreme temperature. Prevent cold and hot shocks.

---

## No. 3: Configuration software installation and use
Our company provides supporting "485 Parameter Configuration Software" can easily use the computer to read the sensor parameters and flexibly modify the sensor equipment ID and address. Note that when using software to automatically obtain 485 There is only one sensor on the bus.

### 3.1 Sensor connected to computer
Pass the sensor through USBchange 485 After correctly connecting to the computer and providing power, You can see the correct COM mouth(" My Computer-Check in "Properties - Device Manager - Ports" Comport). Open the data package and select "Debug Software"--- “485Parameter Configuration Software", find RS485ControlV21.exe Just open it. If you don't find it in the device manager COMport, it means you have not installed USBchange 485If the driver (included in the data package) is not installed correctly, please contact a technician for help.

### 3.2 Use of sensor monitoring software
1. The configuration interface is shown in the figure, first according to 3.1 The method in the chapter obtains the serial port number and selects the correct serial port.
2. click the software's test baud rate, the software will test the baud rate and address of the current device. The default baud rate is 4800bit/s, The default address is 0x01.
3. modify the address and baud rate according to usage needs, and query the current functional status of the device.
4. If the test is unsuccessful, please recheck the device wiring and 485 Driver installation status.

---

## No. 4: Communication Protocol

### 4.1 Basic communication parameters
| Parameter | Value |
|---|---|
| Edit code | 8bit binary |
| Data bits | 8Bit |
| Parity bit | none |
| Stop bits | 1Bit |
| Error checking | CRC(Redundant Cyclic Code) |
| Baud rate | 2400bit/s, 4800bit/s, 9600 bit/s Can be set, factory default is 4800bit/s |

### 4.2 Data frame format definition
use ModBus-RTU Communication protocol, the format is as follows:
- Initial structure ≥4Bytes of time
- Address code = 1byte
- Function code = 1byte
- Data Area = Nbyte
- Error checking = 16Bit CRC code
- End Structure ≥4Bytes of time

Address code: The address of the transmitter, which is unique in the communication network (factory default 0x01).
Function code: The function indication of the command sent by the host. This transmitter only uses the function code 0x03 (Read register data).
Data area: The data area is the specific communication data. 16 bits The data high byte comes first!
CRC Code: Two-byte check code.

**Host inquiry frame structure:**
| Address code | Function code | Register start address | Register length | Check code low | Check code high digit |
|---|---|---|---|---|---|
| 1byte | 1byte | 2byte | 2byte | 1byte | 1byte |

**Slave response frame structure:**
| Address code | Function code | Number of valid bytes | Data Zone 1 | Second data area | Nondate Area | Check code |
|---|---|---|---|---|---|---|
| 1byte | 1byte | 1byte | 2byte | 2byte | 2byte | 2byte |

### 4.3 Register Address
| Register Address | PLC Or configure the address | content | operate |
|---|---|---|---|
| 0000 H | 40001 | wind direction(0-7files) The uploaded data is the real value | Read-only |
| 0001 Η | 40002 | wind direction(0-360°) The uploaded data is the real value | Read-only |

### 4.4 Conversion relationship between numerical values
| Collected value (0-7files) | Collected value (0-360°) | Corresponding direction |
|---|---|---|
| 0 | 0° | North Wind |
| 1 | 45° | Northeast Wind |
| 2 | 90° | Dongfeng |
| 3 | 135° | Southeast Wind |
| 4 | 180° | south wind |
| 5 | 225° | Southwest Wind |
| 6 | 270° | West Wind |
| 7 | 315° | Northwest Wind |

### 4.5 Communication protocol examples and explanations
**Example: Reading device address 0x01 Wind direction**

**Inquiry frame:**
| Address code | Function code | Starting address | Data length | Check code low | Check code high digit |
|---|---|---|---|---|---|
| 0x01 | 0x03 | 0x00 0x00 | 0x00 0x02 | 0xC4 | 0x0B |

**Response frame:** (For example, wind direction value is read (0-7File) is 2, (0-360°) for 90°)
| Address code | Function code | Returns the number of valid bytes | wind direction (0-7files) | wind direction (0-360°) | Check code low | Check code high digit |
|---|---|---|---|---|---|---|
| 0x01 | 0x03 | 0x04 | 0x00 0x02 | 0x00 0x5A | 0xDB | 0xC8 |

**Wind direction calculation:**
- (0-7files): 0002H (hexadecimal) = 2 => wind direction = Dongfeng
- (0-360°): 005AH (hexadecimal) = 90 => wind direction = Dongfeng

---

## No. 5: Common Problems and Solutions
**No output or output error**
Possible causes:
1. Computer has COM The selected port is incorrect.
2. baud rate error.
3. 485 The bus is disconnected, or A,B The wires are connected reversely.
4. If there are too many devices or the wiring is too long, power supply should be provided nearby. 485 Enhancer, while increasing 120Ω Terminal resistance.
5. USBchange485 The driver is not installed or is damaged.
6. equipment damage.
