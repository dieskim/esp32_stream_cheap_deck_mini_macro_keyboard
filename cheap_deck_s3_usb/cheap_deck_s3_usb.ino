// Super Keys Stream Deck - ESP32-S3 USB HID Only
// Configurable 8-button macro pad with Preferences storage and Serial config.
// Tools: USB Mode: "CDC and HID", USB CDC On Boot: Enabled

#include <Preferences.h>
#include <Adafruit_NeoPixel.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "common.h"

// ─────────────────────────────────────────────────
// ESP32-S3 SPECIFIC HARDWARE
// ─────────────────────────────────────────────────

#define RGB_LED_PIN 48
Adafruit_NeoPixel rgbLed(1, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

USBHIDKeyboard usbKeyboard;
Preferences preferences;  // Required by common.h

// BUTTONS WHEN LOOKING FROM FRONT
// TOP LEFT     1  2  3  4  TOP RIGHT
// BOTTOM LEFT  5  6  7  8  BOTTOM RIGHT
const uint8_t buttonPins[] = {1, 2, 13, 4, 5, 6, 7, 8};

// ─────────────────────────────────────────────────
// HID IMPLEMENTATION (S3 USB SPECIFIC)
// ─────────────────────────────────────────────────

void usbKeyPressImpl(uint8_t keyCode) {
  usbKeyboard.press(keyCode);
}

void usbKeyReleaseImpl(uint8_t keyCode) {
  usbKeyboard.release(keyCode);
}

// Set function pointers for common.h
KeyPressFn hidKeyPress = usbKeyPressImpl;
KeyReleaseFn hidKeyRelease = usbKeyReleaseImpl;

// ─────────────────────────────────────────────────
// LED CONTROL (S3 NEOPIXEL SPECIFIC)
// ─────────────────────────────────────────────────

void setLED(uint8_t r, uint8_t g, uint8_t b) {
  rgbLed.setPixelColor(0, rgbLed.Color(r, g, b));
  rgbLed.show();
}

// ─────────────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);

  // Wait for CDC
  unsigned long start = millis();
  while (!Serial && (millis() - start < 5000))
    delay(10);

  Serial.println("\n[cdc] S3 CDC OK!");

  // Initialize RGB LED (S3 specific)
  rgbLed.begin();
  rgbLed.setBrightness(50);
  setLED(0, 0, 0);

  // Start USB HID (S3 specific)
  usbKeyboard.begin();
  Serial.println("[hid] Ready.");

  // Load key mappings (from common.h)
  loadKeyMappings();

  // Initialize button pins
  for (uint8_t i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonStates[i] = HIGH;
  }

  Serial.println("Setup complete.");
}

// ─────────────────────────────────────────────────
// MAIN LOOP
// ─────────────────────────────────────────────────

void loop() {
  yield(); // Give TinyUSB/CDC time slices

  // Handle Serial config commands (from common.h)
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      handleConfigCommand(configBuffer);
      configBuffer = "";
    } else {
      configBuffer += c;
    }
  }

  // Steady subtle blue LED (S3 specific)
  setLED(0, 0, 16);

  // Handle all buttons (from common.h)
  handleButtons();

  delay(10); // Light loop delay
}