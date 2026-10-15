# ESP32 Radar Scanner

A radar scanner built on an **ESP32** microcontroller. An **HC-SR04 ultrasonic sensor** is mounted on a **servo motor** that sweeps from 0° to 180° and back, detecting objects in front of it. Distances are displayed in real time on a **1.8" TFT screen** as a radar-style visualization, and a **buzzer** sounds when an object gets closer than 25 cm.

<!-- Add a photo or GIF of your finished project here, for example: -->
<!-- ![Radar scanner running](images/radar.jpg) -->

## Features

- Continuous 0–180° servo sweep
- Real-time distance measurement (up to 400 cm)
- Half-circle radar display with 4 range rings (1 m, 2 m, 3 m, 4 m) and grid lines every 30°
- Green sweep line that follows the servo angle
- Red dot marking the detected object's position
- Live angle and distance readout on screen
- Proximity alarm: the buzzer turns on when an object is closer than 25 cm
- Serial output (115200 baud) for debugging

## Components

| Part | Notes |
|------|-------|
| ESP32 development board | Main microcontroller |
| HC-SR04 ultrasonic sensor | Distance measurement |
| SG90 servo motor | Rotates the sensor |
| 1.8" ST7735 TFT display (128x160) | Radar visualization |
| Active buzzer | Proximity alarm |
| 220 Ω resistor | Buzzer current limiting |
| 1 kΩ + 2.2 kΩ resistors | Voltage divider for the ECHO pin |
| Breadboard and jumper wires | |
| Power bank (5 V USB) | Power supply |

## Wiring

| Component | Pin | ESP32 connection |
|-----------|-----|------------------|
| Servo | Signal | GPIO 13 |
| Servo | VCC | VIN (5 V) |
| HC-SR04 | TRIG | GPIO 33 |
| HC-SR04 | ECHO | GPIO 32 (through voltage divider) |
| HC-SR04 | VCC | VIN (5 V) |
| Buzzer | Positive | GPIO 12 (through 220 Ω resistor) |
| TFT | SCL | GPIO 19 |
| TFT | SDA | GPIO 23 |
| TFT | RES | GPIO 4 |
| TFT | DC | GPIO 2 |
| TFT | CS | GPIO 15 |
| TFT | VCC | 3.3 V |
| All | GND | GND |

### Voltage divider (ECHO pin protection)

The HC-SR04 outputs 5 V on its ECHO pin, which can damage the ESP32's 3.3 V GPIO. A voltage divider brings it down to about 3.4 V:

```
HC-SR04 ECHO ──[ 1 kΩ ]──┬── GPIO 32
                         │
                      [ 2.2 kΩ ]
                         │
                        GND
```

## Software setup

### Requirements

- [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32 board support (install via **Boards Manager**, search for "esp32" by Espressif Systems)
- Libraries (install via **Library Manager**):
  - `Adafruit GFX Library`
  - `Adafruit ST7735 and ST7789 Library`
  - `ESP32Servo`

### Upload the code

1. Clone this repository:
   ```bash
   git clone https://github.com/YOUR_USERNAME/esp32-radar-scanner.git
   ```
2. Open `radarPIA/radarPIA.ino` in the Arduino IDE.
3. Select your board (**Tools → Board → ESP32 Dev Module**) and the correct COM port.
4. Click **Upload**.
5. Power the board with a 5 V power bank or USB. The radar starts scanning automatically.

## How it works

The servo sweeps from 0° to 180° and back continuously. At each degree, the HC-SR04 fires an ultrasonic pulse and measures the time for the echo to return, then converts it to centimeters using the speed of sound. The TFT draws a half-circle radar with four range rings, a green sweep line following the servo angle, and a red dot at the detected object's position. Angle and distance are shown as text at the bottom of the screen.

When an object is detected closer than 25 cm, the buzzer turns on, and it turns off again once the object moves beyond that threshold.

### Code structure

| Function | Purpose |
|----------|---------|
| `measureDistance()` | Sends a 10 µs pulse on TRIG, times the echo on ECHO and returns the distance in cm. Returns 400 cm if no echo arrives within 30 ms, and clamps results to 400 cm. |
| `drawTitle()` | Draws the "RADAR SCANNER" title and green underline once at startup, so the radar redraw never makes it flicker. |
| `drawRadar()` | Called every loop. Clears the radar area, redraws rings and grid lines, draws the sweep line, the detection dot, and the angle/distance text. |
| `handleBuzzer()` | Turns the active buzzer on or off depending on the measured distance. |
| `setup()` | Initializes serial, SPI, the TFT, pins and the servo. |
| `loop()` | Moves the servo, measures, updates the buzzer and display, and advances the sweep angle. |

## Repository structure

```
esp32-radar-scanner/
├── README.md
├── radarPIA/
│   └── radarPIA.ino
└── docs/
    └── radar_scanner_documentation.pdf
```

Full project documentation is available in [`docs/radar_scanner_documentation.pdf`](docs/radar_scanner_documentation.pdf).

## Authors

- Parocescu Darius Andrei
- Untaru Raul Andrei
- Boltasu Iustin

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
