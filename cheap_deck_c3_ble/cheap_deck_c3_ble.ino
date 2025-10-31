// Super Keys Stream Deck - ESP32-C3 Bluetooth HID
// Configurable 8-button macro pad with Preferences storage and Serial config.

#include <BleKeyboard.h>
#include <Preferences.h>
#include <NimBLEDevice.h>
#include "common.h"

// ─────────────────────────────────────────────────
// ESP32-C3 SPECIFIC HARDWARE
// ─────────────────────────────────────────────────

#define BLUE_LED 8

// BUTTONS WHEN LOOKING FROM FRONT
// TOP LEFT     1  2  3  4  TOP RIGHT
// BOTTOM LEFT  5  6  7  8  BOTTOM RIGHT
const uint8_t buttonPins[] = {20, 9, 2, 1, 21, 10, 3, 0};

BleKeyboard bleKeyboard("Super Keys C3", "ESP32", 100);
Preferences preferences;  // Required by common.h

bool lastConnected = false;

// ─────────────────────────────────────────────────
// HID IMPLEMENTATION (BLUETOOTH SPECIFIC)
// ─────────────────────────────────────────────────

void bleKeyPressImpl(uint8_t keyCode) {
  bleKeyboard.press(keyCode);
}

void bleKeyReleaseImpl(uint8_t keyCode) {
  bleKeyboard.release(keyCode);
}

// Set function pointers for common.h
KeyPressFn hidKeyPress = bleKeyPressImpl;
KeyReleaseFn hidKeyRelease = bleKeyReleaseImpl;

// ─────────────────────────────────────────────────
// LED CONTROL (C3 SIMPLE LED)
// ─────────────────────────────────────────────────

void setLED(uint8_t r, uint8_t g, uint8_t b) {
  // C3 only has blue LED - turn on if any color requested
  digitalWrite(BLUE_LED, (r || g || b) ? LOW : HIGH);
}

// ─────────────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);

  Serial.println("Starting (ESP32-C3)");
  
  // Initialize blue LED
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH); // OFF (active low)

  Serial.println("Startup...");

  // Load key mappings (from common.h)
  loadKeyMappings();
  
  // Start Bluetooth
  bleKeyboard.begin();

  // Initialize button pins
  for (uint8_t i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonStates[i] = HIGH;
  }

  Serial.println("Setup complete, waiting for Bluetooth connection...");
}

// ─────────────────────────────────────────────────
// MAIN LOOP
// ─────────────────────────────────────────────────

void loop() {
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

  bool connected = bleKeyboard.isConnected();
  static uint32_t lastAdvKick = 0;
  static uint32_t lastBlink = 0;

  // Handle disconnected state
  if (!connected) {
    if (lastConnected) {
      Serial.println("Bluetooth disconnected.");
      bleKeyboard.releaseAll();
      lastConnected = false;
    }
    
    // Kick advertising periodically for better reconnection
    uint32_t now = millis();
    if (now - lastAdvKick > 2000) {
      lastAdvKick = now;
      NimBLEDevice::startAdvertising();
    }
    
    // Blink LED while disconnected
    if (now - lastBlink > 500) {
      lastBlink = now;
      static bool ledState = false;
      ledState = !ledState;
      setLED(0, 0, ledState ? 255 : 0);
    }
    
    delay(100);
    return;
  }

  // Handle connection event
  if (!lastConnected) {
    Serial.println("Bluetooth connected!");
    flashLED(5, 100, 0, 0, 255, setLED);; // Blue flash
    setLED(0, 0, 0);
    lastConnected = true;
  }

  // Handle all buttons (from common.h)
  handleButtons();
}