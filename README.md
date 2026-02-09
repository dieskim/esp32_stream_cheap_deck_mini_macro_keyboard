# ⚡ ESP32-C3 Stream Deck Mini Macro Keyboard

[![ESP32 Stream Deck](https://raw.githubusercontent.com/dieskim/esp32_stream_cheap_deck_mini_macro_keyboard/refs/heads/main/ESP32-Supermini-Stream-Cheap-Deck-Mini-Macro-Keyboard.jpg)](https://dieskim.github.io/esp32_stream_cheap_deck_mini_macro_keyboard/)

### 🧰 3D Printed Case, Build Instructions & STLs

<a href="https://makerworld.com/en/models/1899311-esp32-stream-cheap-deck-bluetooth-macro-keyboard" target="_blank">
  <img src="https://cdn-forum.bambulab.com/original/3X/c/1/c1eda64c15dd27c7754e4f567d633b2a72cd387c.png" alt="Download & Build on MakerWorld" width="220">
</a>

### ☕ Support Me

<a href="https://www.buymeacoffee.com/dieskim" target="_blank">
  <img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" width="220">
</a>

## 🚀 Quick Flash Guide

Just open the web flasher below and follow the on-screen steps:

👉 **[Open the Web Flasher](https://dieskim.github.io/esp32_stream_cheap_deck_mini_macro_keyboard/)**

It works directly in your browser (Chrome or Edge required).  
You can:
- Flash the firmware for ESP32-C3 or ESP32-S3  
- Configure your key mappings (F13–F24, combos like `CTRL+F19`)  
- Reset or test your device instantly  

---

## 🧰 Requirements
- ESP32-C3 or ESP32-S3 development board  
- USB-C cable (data-capable)  
- Google Chrome or Microsoft Edge browser  

---

## 🧩 Troubleshooting
- **Device not detected?**  
  Hold the **BOOT** button while plugging in your board.  
- **Still no response?**  
  Try a different USB cable or port.  
- **Bluetooth pairing issues?**  
  Forget the old *Super Keys* device and re-pair after flashing.

---

## 🛠️ Advanced / Manual Setup

If you prefer flashing via Arduino IDE, you can download the Arduino Sketch and manually flash it.

### 📋 Prerequisites

Before you begin, ensure you have the following installed:

#### 1. Arduino IDE
- **Download:** [Arduino IDE](https://www.arduino.cc/en/software)
- **Recommended Version:** 2.0 or higher

#### 2. ESP32 Board Support
1. Open Arduino IDE
2. Go to `File` → `Preferences`
3. Add this URL to "Additional Boards Manager URLs":https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
4. Go to `Tools` → `Board` → `Boards Manager`
5. Search for "ESP32" by Espressif Systems
6. **Install version 3.3.2** (tested with 3.3.2)

#### 3. Required Libraries

Install the following libraries via `Sketch` → `Include Library` → `Manage Libraries`:

##### For Bluetooth (BLE) Mode:

| Library | Author | Version | Installation Method |
|---------|--------|---------|---------------------|
| **ESP32 BLE Keyboard** | T-vK | 0.3.2 | Library Manager or [GitHub Release](https://github.com/T-vK/ESP32-BLE-Keyboard/releases) |
| **NimBLE-Arduino** | h2zero | 2.3.6 | Library Manager |
| **Preferences** | - | Built-in | Included with ESP32 core |

**Installing ESP32 BLE Keyboard via ZIP (Alternative Method):**
1. Download 0.3.2 from the [GitHub Release page](https://github.com/T-vK/ESP32-BLE-Keyboard/releases)
2. In Arduino IDE go to `Sketch` → `Include Library` → `Add .ZIP Library...`
3. Select the downloaded ZIP file

##### For USB HID Mode (S3 Only):
- No additional libraries required (uses built-in TinyUSB)

### 🔧 Board Configuration

#### For ESP32-C3 (Bluetooth)
1. Select `Tools` → `Board` → `ESP32C3 Dev Module`
2. Set `Tools` → `USB CDC On Boot` → `Enabled`
3. Select your COM port under `Tools` → `Port`

#### For ESP32-S3 (Bluetooth)
1. Select `Tools` → `Board` → `ESP32S3 Dev Module`
2. Set `Tools` → `USB CDC On Boot` → `Enabled`
3. Select your COM port under `Tools` → `Port`

#### For ESP32-S3 (USB HID)
1. Select `Tools` → `Board` → `ESP32S3 Dev Module`
2. Set `Tools` → `USB CDC On Boot` → `Enabled`
3. Set `Tools` → `USB Mode` → `USB-OTG (TinyUSB)` ⚠️ **CRITICAL for USB HID**
4. Set `Tools` → `USB DFU On Boot` → `Disabled`
5. Select your COM port under `Tools` → `Port`

---

## 🛠️ How to Rebuild the Firmware (Arduino IDE)

If you need to rebuild the merged firmware binary used by the web flasher, follow this checklist.

### How to Rebuild

#### For ESP32-C3 (Bluetooth)

1. Open the project in **Arduino IDE**
2. Select the following options from the **Tools** menu:
   - **Board:** `ESP32C3 Dev Module`
   - **USB CDC On Boot:** `Enabled`
3. Build the firmware:
   - `Sketch → Export Compiled Binary`
4. Locate the generated merged binary in the sketch folder:
   - `*.ino.merged*.bin`
5. (Optional) Rename the file for your flasher or release:
   - `cheap_deck_c3_ble_v1.0.1.bin`
6. Update the manifest.json

#### For ESP32-S3 (Bluetooth)

1. Open the project in **Arduino IDE**
2. Select the following options from the **Tools** menu:
   - **Board:** `ESP32S3 Dev Module`
   - **USB CDC On Boot:** `Enabled`
3. Build the firmware:
   - `Sketch → Export Compiled Binary`
4. Locate the generated merged binary in the sketch folder:
   - `*.ino.merged*.bin`
5. (Optional) Rename the file for your flasher or release:
   - `cheap_deck_s3_ble_v1.0.1.bin`
6. Update the manifest.json

#### For ESP32-S3 (USB HID)

1. Open the project in **Arduino IDE**
2. Select the following options from the **Tools** menu:
   - **Board:** `ESP32S3 Dev Module`
   - **USB CDC On Boot:** `Enabled`
   - **USB Mode:** `USB-OTG (TinyUSB)` *(critical for USB HID functionality)*
   - **USB DFU On Boot:** `Disabled`
3. Build the firmware:
   - `Sketch → Export Compiled Binary`
4. Locate the generated merged binary in the sketch folder:
   - `*.ino.merged*.bin`
5. (Optional) Rename the file for your flasher or release:
   - `cheap_deck_s3_usb_v1.0.1.bin`
6. Update the manifest.json

**Important Notes:**
- The merged binary includes the bootloader, partition table, and application firmware in a single file
- This binary format is suitable for use with the web flasher
- **For S3 USB HID mode:** The `USB Mode: USB-OTG (TinyUSB)` setting is essential for proper USB HID keyboard functionality

---

✨ Created by [@dieskim](https://github.com/dieskim)
