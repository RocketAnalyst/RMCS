# RMCS — ROCI Motor Characterization System

RMCS (ROCI Motor Characterization System) is an open-source rocket motor thrust measurement and data logging system developed by the Rocketeers of Central Indiana (ROCI).

The system is designed to record load-cell measurements during rocket motor static testing and produce timestamped CSV data for subsequent analysis.

> **Project status: V0.01 — Experimental / Development**

V0.01 is a functional proof-of-concept measurement system. It has been tested using controlled mechanical loads and has successfully demonstrated continuous data acquisition, state detection, SD-card logging, and post-event recording.

This project is experimental instrumentation and is **not a certified safety-critical measurement system**. Anyone using or modifying the system is responsible for determining whether it is appropriate and safe for their particular application.

---

## Features

V0.01 currently provides:

- HX711 load-cell data acquisition
- 1-ton load-cell support
- Approximately 85 samples/second acquisition rate
- User-controlled tare
- Debounced START and TARE buttons
- Immediate recording when START is pressed
- Automatic ignition-event detection
- Automatic burnout detection
- Five seconds of post-burn recording
- Timestamped measurements
- Raw HX711 values preserved in the data
- Signed thrust measurements
- Automatic sequential test-file numbering
- Built-in Teensy 4.1 microSD logging
- CSV output
- Multiple tests per SD card/session
- No RTC dependency
- Modular C++ architecture using PlatformIO

---

## V0.01 Hardware

### Microcontroller

- Teensy 4.1

### Load Measurement

- BENGBUZZYQ 1-ton button/disc load cell
- SparkFun HX711 Load Cell Amplifier

### Storage

- Teensy 4.1 built-in microSD card socket
- microSD card

### User Controls

- TARE pushbutton
- START pushbutton

### Planned Hardware

The following are intentionally not part of V0.01:

- STOP/ABORT button
- Status LED
- Buzzer
- OLED display
- RTC
- Additional sensors
- Advanced telemetry
- PC visualization software

---

## Wiring

### HX711 → Load Cell

| Load Cell Wire | HX711 Terminal |
|---|---|
| Yellow | YLW / SD |
| Green | GRN / A+ |
| White | WHT / A- |
| Black | BLK / E- |
| Red | RED / E+ |

### HX711 → Teensy 4.1

| HX711 | Teensy |
|---|---|
| VCC | 5V / VIN |
| GND | GND |
| DAT | Pin 3 |
| CLK | Pin 2 |
| VDD | 3.3V |

The HX711 is configured for its higher-rate operating mode.

### Buttons

Both buttons use the Teensy's internal pull-up resistors.

| Function | Teensy Pin |
|---|---:|
| TARE | 4 |
| START | 5 |

Each button connects between its assigned GPIO pin and GND.

---

## Software Architecture

RMCS is intentionally divided into small modules so that the system can be expanded without placing all functionality in `main.cpp`.

```text
RMCS/
├── include/
│   ├── Config.h
│   ├── Buttons.h
│   ├── LoadCell.h
│   ├── DataLogger.h
│   └── StateMachine.h
│
├── src/
│   ├── main.cpp
│   ├── Buttons.cpp
│   ├── LoadCell.cpp
│   ├── DataLogger.cpp
│   └── StateMachine.cpp
│
├── platformio.ini
├── README.md
├── LICENSE
└── .gitignore