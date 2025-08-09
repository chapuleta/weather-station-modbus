# Polycarbon wind speed transmitter (Model 485) SN-3000-FSJT-N01 Ver 2.0

## directory
- Chapter 1 Product Introduction
  - 1.1 Product Overview
  - 1.2 Functional features
  - 1.3 Main parameters
- Chapter 2 Hardware Connections
  - 2.1 Inspection before installation of the equipment
  - 2.2 Interface Description
  - 2.2.1 Sensor wiring
  - 2.3 Installation
  - 2.4 Precautions
- Chapter 3 Configuring Software Installation and Use
  - 3.1 The sensor is connected to the computer
  - 3.2 Use of sensor monitoring software
- Chapter 4 Communication Protocols
  - 4.1 Basic communication parameters
  - 4.2 Data Frame Format Definition
  - 4.3 Register address
  - 4.4 Examples and Explanations of Communication Protocols
- clause 5 chapter Common problems and solutions

---

## Chapter 1 Product Introduction

### 1.1 Product Overview
The wind speed transmitter adopts the traditional three-cup wind speed sensor structure, and the wind cup is made of carbon fiber material, which has high strength and good start-up; The built-in signal processing unit of the cup can output the corresponding wind speed signal according to the user's needs, which can be widely used in meteorology, ocean, environment, airport, port, laboratory, industry and agriculture and transportation.

### 1.2 Functional features
- Range: 0-70m/s, resolution 0.1m/s
- Anti-electromagnetic interference treatment
- The bottom outlet method is adopted to completely eliminate the aging problem of aviation plug rubber pad, and it is still waterproof for long-term use
- The high-performance imported bearings are adopted, the rotational resistance is small, and the measurement is accurate
- Polycarbonate shell, high mechanical strength, high hardness, corrosion resistance, no rust can be used outdoors for a long time
- The structure and weight of the equipment are carefully designed and distributed, with small moment of inertia and sensitive response
- Standard ModBus-RTU communication protocol, easy access

### 1.3 Main parameters
| Parameter | Value |
|---|---|
| DC Power Supply (Default) | 10~30V DC |
| power consumption | ≤0.3W |
| The operating temperature of the transmitter circuit | -40°C~+60°C, 0%RH~80%RH |
| Communication interfaces | 485 communication (ModBus) protocol, Baud rate: 2400, 4800 (default), 9600, Data bit length: 8 bits, Parity mode: None, Stop bit length: 1 bit, Default ModBus communication address: 1 |
| Parameter settings | Configuration is performed via the 485 interface using the provided configuration software |
| resolution | 0.1m/s |
| precision | ±(0.2+0.03V)m/s,@(0~30m/s,25°C) V indicates the wind speed |
| Measuring range | 0~70m/s |
| Dynamic response time | <1s |
| Start the wind speed | ≤0.2m/s |

---

## Chapter 2 Hardware Connections

### 2.1 Inspection before installation of the equipment
- 1 transmitter equipment
- 4 mounting screws
- Certificate of conformity, warranty card

### 2.2 Interface Description
The power interface is a wide voltage power supply input of 10-30V. When wiring the 485 signal line, pay attention to the two lines of A and B cannot be reversed, and the addresses of multiple devices on the bus cannot conflict.

### 2.2.1 Sensor wiring
| Wire | Line color | illustrate |
|---|---|---|
| electricity source | brown | Power supply (10~30V DC) |
| | black | Power supply negative |
| open letter | Yellow (green) color | 485-A |
| | blue | 485-B |

### 2.3 Installation
Flange installation is adopted, the threaded flange connection makes the lower pipe fittings of the wind speed sensor firmly fixed on the flange, the chassis is Ø80mm, and four mounting holes of Ø4.5mm are opened on the circumference of Ø68mm, and the bolts are used to tightly fix it on the bracket, so that the whole set of instruments is kept at the best level, to ensure the accuracy of wind speed data, and the flange connection is easy to use and can withstand greater pressure.

### 2.4 Precautions
1. The user shall not disassemble it by himself, let alone touch the sensor core, so as not to cause damage to the product.
2. Try to stay away from high-power interference equipment, so as not to cause inaccurate measurement, such as inverter, motor, etc., when installing and disassembling the transmitter, the power supply must be disconnected first, and water entering the transmitter can lead to irreversible changes.
3. Prevent chemical reagents, oil, dust, etc. from directly infringing on the sensor, do not use it for a long time in condensation and extreme temperature environment, and strictly prevent cold and heat shock.

---

## Chapter 3 Configuring Software Installation and Use
Our company provides a supporting "485 parameter configuration software", which can easily use the computer to read the parameters of the sensor, and flexibly modify the device ID and address of the sensor. Note that when using software auto-acquisition, you need to ensure that there is only one sensor on the 485 bus.

### 3.1 The sensor is connected to the computer
After the sensor is properly connected to the computer via USB to 485 and provided with power, you can see the correct COM port in the computer ("My Computer - Properties - Device Manager - Ports" to see the COM port). Open the package, select "Debugging Software" --- "485 Parameter Configuration Software", and find and RS485ControlV21.exe open it. If you don't see the COM port in Device Manager, it means that you don't have the USB to 485 driver installed (in the package) or you haven't installed the driver correctly, contact a technician for assistance.

### 3.2 Use of sensor monitoring software
1. The configuration interface is as shown in the figure, first obtain the serial slogan according to the method in chapter 3.1 and select the correct serial port.
2. Click on the test baud rate of the software, the software will test the baud rate and address of the current device, the default baud rate is 4800bit/s, and the default address is 0x01.
3. Modify the address and baud rate according to the needs of use, and query the current functional status of the equipment at the same time.
4. If the test is not successful, please re-check the wiring of the equipment and the installation of the 485 driver.

---

## Chapter 4 Communication Protocols

### 4.1 Basic communication parameters
| Parameter | Value |
|---|---|
| Encoding | 8-bit binary |
| Data bits | 8 bits |
| Parity bits | not |
| Stop bits | 1 bit |
| Error checks | CRC (Redundant Cycle Code). |
| baud rate | 1200bit/s, 2400bit/s, 4800bit/s, 9600bit/s, 19200bit/s, 38400bit/s, 57600bit/s, 115200bit/sbit/s can be set, and the factory default is 4800bit/s |

### 4.2 Data Frame Format Definition
The ModBus-RTU communication protocol is used in the following format:
- The initial structure >= 4 bytes of time
- Address code = 1 byte
- Feature code = 1 byte
- Data area = N bytes
- Error check = 16-digit CRC code
- The time at which the end structure >= 4 bytes

Address code: The address of the transmitter, which is unique in the communication network (factory default 0x01).
Function code: The function indication of the instruction sent by the host computer, this transmitter only uses the function code 0x03 (read register data).
Data area: The data area is the specific communication data, pay attention to the 16bits data high byte first!
CRC code: a two-byte check code.

**Host query frame structure:**
| Address code | Feature codes | Register start address | Register length | The check sum is low | The check digit is high |
|---|---|---|---|---|---|
| 1 byte | 1 byte | 2 bytes | 2 bytes | 1 byte | 1 byte |

**Slave Response Frame Structure:**
| Address code | Feature codes | Number of valid bytes | Data Zone 1 | Second data area | N data area | Checksum |
|---|---|---|---|---|---|---|
| 1 byte | 1 byte | 1 byte | 2 bytes | 2 bytes | 2 bytes | 2 bytes |

### 4.3 Register address
| Register address | PLC or configuration address | content | Feature codes are supported |
|---|---|---|---|
| 0000 H | 40001 | Instantaneous wind speed The uploaded data is 10 times the true value | 03/04 |
| 07D0 H | 42001 | Device address 1~254 (factory default 1) | 0x03/0x04/0x06/0x10 |
| 07D1H | 42002 | Device baud rate 0 represents 2400 bit/s and 1 represents 4800 bit/s 2 for 9600bit/s 3 for 19200bit/s 4 for 38400bit/s 5 for 57600bit/s 6 represents 115200bit/s and 7 represents 1200bit/s | 0x03/0x04/0x06/0x10 |

### 4.4 Examples and Explanations of Communication Protocols
**For example, read the wind speed value of the device address 0x01**

**Query frame:**
| Address code | Feature codes | Start address | The length of the data | The check sum is low | The check digit is high |
|---|---|---|---|---|---|
| 0x01 | 0x03 | 0x00 0x00 | 0x00 0x01 | 0x84 | 0x0A |

**Response frame: (e.g. read that the current wind speed is 8.6m/s)**
| Address code | Feature codes | Returns the number of valid bytes | The current wind speed value | The check sum is low | The check digit is high |
|---|---|---|---|---|---|
| 0x01 | 0x03 | 0x02 | 0x00 0x56 | 0x38 | 0x7A |

**Wind speed calculation:**
Current wind speed: 0056H (hexadecimal) = 86=> Wind speed = 8.6m/s

---

## Chapter 5 Common Problems and Solutions
**No output or output errors**
Possible causes:
1. The computer has a COM port, and the selected port is incorrect.
2. The baud rate is wrong.
3. The 485 bus is disconnected, or the A and B lines are reversed.
4. If the number of devices is too large or the wiring is too long, the nearest power supply should be added, 485 booster should be added, and 120Ω terminal resistance should be increased at the same time.
5. The USB to 485 driver is not installed or damaged.
6. The equipment is damaged.
