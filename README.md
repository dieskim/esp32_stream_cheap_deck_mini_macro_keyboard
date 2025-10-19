# ESP32-C3 Stream Deck Mini Macro Keyboard - Flashing Instructions

# 3D Printed Case and Instructions
[https://makerworld.com/en/models/1899311-esp32-stream-cheap-deck-bluetooth-macro-keyboard](https://makerworld.com/en/models/1899311-esp32-stream-cheap-deck-bluetooth-macro-keyboard)

![ESP32 Stream Deck](https://raw.githubusercontent.com/dieskim/esp32_stream_cheap_deck_mini_macro_keyboard/main/ESP32%20C3%20Supermini%20-%20Stream%20Cheap%20Deck%20-%20Mini%20Macro%20Keyboard.png)

This guide will help you flash the ESP32 Stream Deck Mini Macro Keyboard firmware to your ESP32-C3 device.

## Prerequisites

Before you begin, ensure you have the following:

1. **ESP-IDF or Arduino IDE** (the project supports both)
2. **USB drivers** for ESP32-C3 (CH340/CP2102 depending on your board)
3. **USB-C cable** to connect your ESP32-C3
4. **ESP32-C3 development board**

## Method 1: Using Arduino IDE (Recommended for Beginners)

### Step 1: Install Arduino IDE

Download and install Arduino IDE 2.0 or later from [arduino.cc](https://www.arduino.cc/en/software)

### Step 2: Add ESP32 Board Support

1. Open Arduino IDE
2. Navigate to **File → Preferences**
3. In the "Additional Board Manager URLs" field, add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click **OK**
5. Go to **Tools → Board → Boards Manager**
6. Search for "esp32"
7. Install **"esp32 by Espressif Systems"**

### Step 3: Install Required Library

The project requires the ESP32-BLE-Keyboard library. You have two options to install it:

#### Option A: Using Arduino Library Manager (Easiest)

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. In the Library Manager, search for **"ESP32 BLE Keyboard"**
4. Find the library by **T-vK**
5. Click **Install**

#### Option B: Manual Installation from GitHub

1. Download the library from: [https://github.com/T-vK/ESP32-BLE-Keyboard](https://github.com/T-vK/ESP32-BLE-Keyboard)
2. Click **Code → Download ZIP**
3. In Arduino IDE, go to **Sketch → Include Library → Add .ZIP Library**
4. Select the downloaded ZIP file
5. Restart Arduino IDE

### Step 4: Download the Project

Clone or download this repository:

```bash
git clone https://github.com/dieskim/esp32_stream_cheap_deck_mini_macro_keyboard.git
```

Or download as ZIP from GitHub and extract it.

### Step 5: Open the Project

1. Navigate to the project folder
2. Locate the `.ino` file
3. Open it in Arduino IDE

### Step 6: Configure Board Settings

Configure the following settings in Arduino IDE:

- **Tools → Board → ESP32 Arduino → ESP32C3 Dev Module**
- **Tools → USB CDC On Boot → Enabled**
- **Tools → Port → Select your ESP32-C3 port**
  - Windows: COM3, COM4, etc.
  - Linux: /dev/ttyUSB0, /dev/ttyACM0, etc.
  - macOS: /dev/cu.usbserial-*

### Step 7: Flash the Device

1. Press and hold the **BOOT button** on your ESP32-C3
2. Click the **Upload** button in Arduino IDE
3. Release the BOOT button when you see "Connecting..." or upload progress starts
4. Wait for the "Done uploading" message

### Step 8: Test the Device

1. Press the **RESET button** on your ESP32-C3
2. Open **Tools → Serial Monitor** (set baud rate to 115200)
3. You should see the device boot up and initialization messages

## Troubleshooting

### Device Not Detected

- **Install USB drivers**: Download and install CH340 or CP2102 drivers depending on your board
- **Try a different USB cable**: Some cables are charge-only and don't support data transfer
- **Check Device Manager/System**: Verify the device appears when plugged in
- **Press BOOT button**: Hold BOOT while plugging in the USB cable

### Upload Fails

- **Hold BOOT button**: Keep the BOOT button pressed during the entire upload process
- **Verify port selection**: Ensure the correct port is selected in Arduino IDE
- **Reduce upload speed**: Try setting **Tools → Upload Speed → 115200**
- **Check USB connection**: Try a different USB port or cable

### No Serial Output

- **Press RESET button**: After flashing, press RESET to start the program
- **Check baud rate**: Ensure Serial Monitor is set to 115200 baud
- **Verify USB CDC**: Make sure "USB CDC On Boot" is enabled in board settings

### Library Errors

- **"BleKeyboard.h not found"**: Make sure you've installed the ESP32-BLE-Keyboard library (see Step 3)
- **Compilation errors**: Verify you're using the latest version of the ESP32-BLE-Keyboard library
- **BLE initialization fails**: Ensure your ESP32-C3 board supports BLE (most do)

## Additional Resources

- [ESP32-C3 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/)
- [Arduino ESP32 GitHub](https://github.com/espressif/arduino-esp32)
- [ESP32-BLE-Keyboard Library](https://github.com/T-vK/ESP32-BLE-Keyboard)
- [Project Issues](https://github.com/dieskim/esp32_stream_cheap_deck_mini_macro_keyboard/issues)
