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
If you prefer flashing via Arduino IDE, you can still download the Arduino Sketch and Manually Flash it.

## 🛠️ How to Rebuild the Firmware (Arduino IDE)

If you need to rebuild the merged firmware binary used by the web flasher, follow this checklist.

### How to Rebuild

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
6. Update the manigest.json

The merged binary includes the bootloader, partition table, and application firmware in a single file and is suitable for use with the web flasher.


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
- On macOS, F14 and F15 are reserved at the system level and are not exposed to applications by default. If these keys appear non-functional, this is a macOS limitation — not a hardware or firmware issue. Remapping is required to use them.

---

✨ Created by [@dieskim](https://github.com/dieskim)
