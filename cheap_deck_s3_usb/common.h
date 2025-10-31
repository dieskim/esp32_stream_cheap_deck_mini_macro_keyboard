#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include <Preferences.h>

// ─────────────────────────────────────────────────
// CONFIGURATION CONSTANTS
// ─────────────────────────────────────────────────

const uint8_t numButtons = 8;

const char* buttonNames[] = {
  "Button 1", "Button 2", "Button 3", "Button 4",
  "Button 5", "Button 6", "Button 7", "Button 8"
};

// ─────────────────────────────────────────────────
// LED UTILS (SHARED)
// ─────────────────────────────────────────────────
void flashLED(int times, int delayMs,
              uint8_t r = 0, uint8_t g = 0, uint8_t b = 255,
              void (*setLEDFn)(uint8_t, uint8_t, uint8_t) = nullptr) {
  if (!setLEDFn) return; // requires platform LED handler
  for (int i = 0; i < times; i++) {
    setLEDFn(r, g, b);
    delay(delayMs);
    setLEDFn(0, 0, 0);
    delay(delayMs);
  }
}

// ─────────────────────────────────────────────────
// DATA STRUCTURES
// ─────────────────────────────────────────────────

struct KeyCombo {
  uint8_t keyCode;
  bool ctrl;
  bool alt;
  bool shift;
  bool gui;
};

struct KeyConfig {
  uint8_t keyCode;
  bool ctrl;
  bool alt;
  bool shift;
  bool gui;
};

// ─────────────────────────────────────────────────
// GLOBAL STATE
// ─────────────────────────────────────────────────

KeyConfig keyMappings[numButtons];
bool lastButtonStates[numButtons];
bool configMode = false;
String configBuffer = "";

// External preferences instance (must be created in main file)
extern Preferences preferences;

// ─────────────────────────────────────────────────
// HID CALLBACK FUNCTION POINTERS
// ─────────────────────────────────────────────────

// Function pointer types for HID operations
typedef void (*KeyPressFn)(uint8_t keyCode);
typedef void (*KeyReleaseFn)(uint8_t keyCode);

// External function pointers (must be set in main file)
extern KeyPressFn hidKeyPress;
extern KeyReleaseFn hidKeyRelease;

// ─────────────────────────────────────────────────
// FUNCTION DECLARATIONS
// ─────────────────────────────────────────────────

void loadKeyMappings();
void saveKeyMappings();
void resetToDefaults();
void handleConfigCommand(String cmd);
void handleButtons();

// ─────────────────────────────────────────────────
// PARSING FUNCTIONS
// ─────────────────────────────────────────────────

KeyCombo parseKeyCombo(String combo) {
  KeyCombo kc = {0, false, false, false, false};
  combo.trim();
  combo.toUpperCase();

  kc.ctrl = combo.indexOf("CTRL") >= 0;
  kc.alt = combo.indexOf("ALT") >= 0;
  kc.shift = combo.indexOf("SHIFT") >= 0;
  kc.gui = (combo.indexOf("GUI") >= 0 || combo.indexOf("CMD") >= 0 || combo.indexOf("WIN") >= 0);

  int pos = combo.lastIndexOf("KEY_F");
  if (pos < 0) pos = combo.lastIndexOf("F");
  if (pos >= 0) {
    String numStr = combo.substring(pos + (combo.startsWith("KEY_F", pos) ? 5 : 1));
    int fnum = numStr.toInt();
    if (fnum >= 1 && fnum <= 12)
      kc.keyCode = 193 + fnum; // 194–205
    else if (fnum >= 13 && fnum <= 24)
      kc.keyCode = 240 + (fnum - 13); // 240–251
  }

  return kc;
}

// ─────────────────────────────────────────────────
// PREFERENCES STORAGE (ESP32 GENERIC)
// ─────────────────────────────────────────────────

void loadKeyMappings() {
  preferences.begin("streamdeck", false);
  for (int i = 0; i < numButtons; i++) {
    char key[16];
    sprintf(key, "key%d_code", i);
    keyMappings[i].keyCode = preferences.getUChar(key, 240 + i); // defaults F13–F20

    sprintf(key, "key%d_ctrl", i);
    keyMappings[i].ctrl = preferences.getBool(key, false);

    sprintf(key, "key%d_alt", i);
    keyMappings[i].alt = preferences.getBool(key, false);

    sprintf(key, "key%d_shift", i);
    keyMappings[i].shift = preferences.getBool(key, false);

    sprintf(key, "key%d_gui", i);
    keyMappings[i].gui = preferences.getBool(key, false);
  }
  preferences.end();

  Serial.println("Key mappings loaded:");
  for (int i = 0; i < numButtons; i++) {
    Serial.printf("Button %d: code %d (CTRL:%d ALT:%d SHIFT:%d GUI:%d)\n",
                  i + 1, keyMappings[i].keyCode,
                  keyMappings[i].ctrl, keyMappings[i].alt,
                  keyMappings[i].shift, keyMappings[i].gui);
  }
}

void saveKeyMappings() {
  preferences.begin("streamdeck", false);
  for (int i = 0; i < numButtons; i++) {
    char key[16];
    sprintf(key, "key%d_code", i);
    preferences.putUChar(key, keyMappings[i].keyCode);

    sprintf(key, "key%d_ctrl", i);
    preferences.putBool(key, keyMappings[i].ctrl);

    sprintf(key, "key%d_alt", i);
    preferences.putBool(key, keyMappings[i].alt);

    sprintf(key, "key%d_shift", i);
    preferences.putBool(key, keyMappings[i].shift);

    sprintf(key, "key%d_gui", i);
    preferences.putBool(key, keyMappings[i].gui);
  }
  preferences.end();
  Serial.println("Key mappings saved!");
}

void resetToDefaults() {
  for (int i = 0; i < numButtons; i++) {
    keyMappings[i].keyCode = 240 + i; // F13–F20 default
    keyMappings[i].ctrl = false;
    keyMappings[i].alt = false;
    keyMappings[i].shift = false;
    keyMappings[i].gui = false;
  }
  saveKeyMappings();
  Serial.println("Reset to default key mappings");
}

// ─────────────────────────────────────────────────
// CONFIGURATION PROTOCOL (PLATFORM INDEPENDENT)
// ─────────────────────────────────────────────────

void handleConfigCommand(String cmd) {
  cmd.trim();
  if (cmd == "CONFIG_START") {
    configMode = true;
    Serial.println("CONFIG_MODE_READY");
  } else if (cmd == "CONFIG_END") {
    configMode = false;
    saveKeyMappings();
    Serial.println("CONFIG_SAVED");
  } else if (cmd == "CONFIG_RESET") {
    resetToDefaults();
    Serial.println("CONFIG_RESET_OK");
  } else if (cmd.startsWith("SET_KEY:")) {
    int firstColon = cmd.indexOf(':', 8);
    if (firstColon > 0) {
      int buttonIndex = cmd.substring(8, firstColon).toInt();
      String keyName = cmd.substring(firstColon + 1);

      if (buttonIndex >= 0 && buttonIndex < numButtons) {
        KeyCombo kc = parseKeyCombo(keyName);
        if (kc.keyCode != 0) {
          keyMappings[buttonIndex].keyCode = kc.keyCode;
          keyMappings[buttonIndex].ctrl = kc.ctrl;
          keyMappings[buttonIndex].alt = kc.alt;
          keyMappings[buttonIndex].shift = kc.shift;
          keyMappings[buttonIndex].gui = kc.gui;

          Serial.printf("SET_KEY_OK:%d:%d:%s%s%s%s\n",
                        buttonIndex, kc.keyCode,
                        kc.ctrl ? "CTRL+" : "",
                        kc.alt ? "ALT+" : "",
                        kc.shift ? "SHIFT+" : "",
                        kc.gui ? "GUI" : "");
        } else {
          Serial.printf("SET_KEY_ERR:%d:%s\n", buttonIndex, keyName.c_str());
        }
      }
    }
  } else if (cmd == "GET_CONFIG") {
    Serial.println("CONFIG_START");
    for (int i = 0; i < numButtons; i++) {
      Serial.print("KEY:");
      Serial.print(i);
      Serial.print(":");
      Serial.print(keyMappings[i].keyCode);
      Serial.print(":");

      bool any = false;
      if (keyMappings[i].ctrl) {
        Serial.print("CTRL+");
        any = true;
      }
      if (keyMappings[i].alt) {
        Serial.print("ALT+");
        any = true;
      }
      if (keyMappings[i].shift) {
        Serial.print("SHIFT+");
        any = true;
      }
      if (keyMappings[i].gui) {
        Serial.print("GUI+");
        any = true;
      }
      if (!any) Serial.print("-");
      Serial.println();
    }
    Serial.println("CONFIG_END");
  }
}

// ─────────────────────────────────────────────────
// BUTTON HANDLING (PLATFORM INDEPENDENT)
// ─────────────────────────────────────────────────

// External button pins array (must be defined in main file)
extern const uint8_t buttonPins[];

void handleButtons() {
  for (uint8_t i = 0; i < numButtons; i++) {
    bool currentState = digitalRead(buttonPins[i]);

    if (currentState == LOW && lastButtonStates[i] == HIGH) {
      // Button pressed
      KeyConfig kc = keyMappings[i];
      Serial.printf("%s pressed (code %d)\n", buttonNames[i], kc.keyCode);

      // Press modifier keys
      if (kc.ctrl) hidKeyPress(KEY_LEFT_CTRL);
      if (kc.alt) hidKeyPress(KEY_LEFT_ALT);
      if (kc.shift) hidKeyPress(KEY_LEFT_SHIFT);
      if (kc.gui) hidKeyPress(KEY_LEFT_GUI);
      
      // Press main key
      hidKeyPress(kc.keyCode);
      delay(50); // Brief hold

    } else if (currentState == HIGH && lastButtonStates[i] == LOW) {
      // Button released
      KeyConfig kc = keyMappings[i];

      // Release main key
      hidKeyRelease(kc.keyCode);
      
      // Release modifier keys
      if (kc.ctrl) hidKeyRelease(KEY_LEFT_CTRL);
      if (kc.alt) hidKeyRelease(KEY_LEFT_ALT);
      if (kc.shift) hidKeyRelease(KEY_LEFT_SHIFT);
      if (kc.gui) hidKeyRelease(KEY_LEFT_GUI);

      Serial.printf("%s released\n", buttonNames[i]);
    }

    lastButtonStates[i] = currentState;
  }
}

#endif // COMMON_H