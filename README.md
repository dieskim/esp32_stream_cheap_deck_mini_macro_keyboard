# ESP32-C3 Stream Deck Mini Macro Keyboard - Quick Flash Guide

![ESP32 Stream Deck](https://raw.githubusercontent.com/dieskim/esp32_stream_cheap_deck_mini_macro_keyboard/main/ESP32%20C3%20Supermini%20-%20Stream%20Cheap%20Deck%20-%20Mini%20Macro%20Keyboard.png)

**3D Printed Case:** [Download from MakerWorld](https://makerworld.com/en/models/1899311-esp32-stream-cheap-deck-bluetooth-macro-keyboard)

## What You Need

- ESP32-C3 development board
- USB-C cable (data-capable)
- Arduino IDE 2.0 or later

## Installation Steps

### 1. Install Arduino IDE
Download from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Add ESP32 Board Support
1. Open Arduino IDE → **File → Preferences** (Windows/Linux) or **Arduino IDE → Settings** (Mac)
2. Add this URL to "Additional Board Manager URLs":
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. Go to **Tools → Board → Boards Manager**
4. Search "esp32" and install **ESP32 by Espressif Systems > Version 3.3.2**

### 3. Install Required Libraries
1. **ESP32-BLE-Keyboard:**
   - **Important:** If you already have ESP32-BLE-Keyboard installed, remove it first:
     - Go to Arduino's libraries folder (**File → Preferences** on Windows/Linux or **Arduino IDE → Settings** on Mac → show "Sketchbook location")
     - Navigate to the `libraries` folder
     - Delete the `ESP32-BLE-Keyboard` folder
     - Restart Arduino IDE
   - Go to **Sketch → Include Library → Add .ZIP Library**
   - Download and add: [ESP32-BLE-Keyboard-0.0.6.zip](https://github.com/dieskim/ESP32-BLE-Keyboard/releases/download/v0.0.6/ESP32-BLE-Keyboard-0.0.6.zip)

2. **NimBLE-Arduino:**
   - Go to **Sketch → Include Library → Manage Libraries**
   - Search "NimBLE-Arduino" and click **Install**

### 4. Download & Open Project
1. Download this repository as ZIP from GitHub
2. Extract and open the `.ino` file in Arduino IDE

### 5. Configure Board
- **Tools → Board → ESP32 Arduino → ESP32C3 Dev Module**
- **Tools → USB CDC On Boot → Enabled**
- **Tools → Port → Select your ESP32-C3 port** (COM3, /dev/ttyUSB0, etc.)

### 6. Flash
1. Hold the **BOOT button** on your ESP32-C3
2. Click **Upload** in Arduino IDE
3. Release BOOT when upload starts
4. Press **RESET** when done

## Troubleshooting

**Device not detected?** Install CH340/CP2102 USB drivers and try a different cable

**Upload fails?** Hold BOOT button during entire upload or reduce upload speed to 115200

**Need help?** [Open an issue](https://github.com/dieskim/esp32_stream_cheap_deck_mini_macro_keyboard/issues)
