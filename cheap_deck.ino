#include <BleKeyboard.h>
#include <Preferences.h>

BleKeyboard bleKeyboard("Super Keys", "ESP32", 100);
Preferences preferences;

// BUTTONS WHEN LOOKING FROM FRONT
// TOP LEFT     1 2 3 4     TOP RIGHT
// BOTTOM LEFT  5 6 7 8     BOTTOM RIGHT
const uint8_t buttonPins[] = {
  20,  // Button 1
  9,   // Button 2
  2,   // Button 3
  1,   // Button 4
  21,  // Button 5
  10,  // Button 6
  3,   // Button 7
  0    // Button 8
};

const uint8_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
const char* buttonNames[] = {
  "Button 1", "Button 2", "Button 3", "Button 4",
  "Button 5", "Button 6", "Button 7", "Button 8"
};

// ───────────────────────────────────────────────
// Structs for combo and config
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

KeyConfig keyMappings[numButtons];

// ───────────────────────────────────────────────
#define BLUE_LED 8

bool lastButtonStates[numButtons];
bool lastConnected = false;
bool configMode = false;
String configBuffer = "";

// ───────────────────────────────────────────────
KeyCombo parseKeyCombo(String combo) {
  KeyCombo kc = {0, false, false, false, false};
  combo.trim();
  combo.toUpperCase();

  kc.ctrl  = combo.indexOf("CTRL") >= 0;
  kc.alt   = combo.indexOf("ALT") >= 0;
  kc.shift = combo.indexOf("SHIFT") >= 0;
  kc.gui   = (combo.indexOf("GUI") >= 0 || combo.indexOf("CMD") >= 0 || combo.indexOf("WIN") >= 0);

  int pos = combo.lastIndexOf("KEY_F");
  if (pos < 0) pos = combo.lastIndexOf("F");
  if (pos >= 0) {
    String numStr = combo.substring(pos + (combo.startsWith("KEY_F", pos) ? 5 : 1));
    int fnum = numStr.toInt();
    if (fnum >= 1 && fnum <= 12)
      kc.keyCode = 193 + fnum;  // 194–205
    else if (fnum >= 13 && fnum <= 24)
      kc.keyCode = 240 + (fnum - 13); // 240–251
  }

  return kc;
}

// ───────────────────────────────────────────────
void flashBlue(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BLUE_LED, LOW);
    delay(delayMs);
    digitalWrite(BLUE_LED, HIGH);
    delay(delayMs);
  }
}

// ───────────────────────────────────────────────
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

// ───────────────────────────────────────────────
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

// ───────────────────────────────────────────────
void resetToDefaults() {
  for (int i = 0; i < numButtons; i++) {
    keyMappings[i].keyCode = 240 + i;  // F13–F20 default
    keyMappings[i].ctrl = false;
    keyMappings[i].alt = false;
    keyMappings[i].shift = false;
    keyMappings[i].gui = false;
  }
  saveKeyMappings();
  Serial.println("Reset to default key mappings");
}

// ───────────────────────────────────────────────
void handleConfigCommand(String cmd) {
  cmd.trim();

  if (cmd == "CONFIG_START") {
    configMode = true;
    Serial.println("CONFIG_MODE_READY");
  }
  else if (cmd == "CONFIG_END") {
    configMode = false;
    saveKeyMappings();
    Serial.println("CONFIG_SAVED");
  }
  else if (cmd == "CONFIG_RESET") {
    resetToDefaults();
    Serial.println("CONFIG_RESET_OK");
  }
  else if (cmd.startsWith("SET_KEY:")) {
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
  }
  else if (cmd == "GET_CONFIG") {
    Serial.println("CONFIG_START");
    for (int i = 0; i < numButtons; i++) {
      Serial.print("KEY:");
      Serial.print(i);
      Serial.print(":");
      Serial.print(keyMappings[i].keyCode);
      Serial.print(":");

      bool any = false;
      if (keyMappings[i].ctrl)  { Serial.print("CTRL+"); any = true; }
      if (keyMappings[i].alt)   { Serial.print("ALT+"); any = true; }
      if (keyMappings[i].shift) { Serial.print("SHIFT+"); any = true; }
      if (keyMappings[i].gui)   { Serial.print("GUI+"); any = true; }
      if (!any) Serial.print("-");
      Serial.println();
    }
    Serial.println("CONFIG_END");
  }
}

// ───────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("Startup...");

  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);

  loadKeyMappings();
  bleKeyboard.begin();

  for (uint8_t i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonStates[i] = HIGH;
  }

  Serial.println("Setup complete, waiting for Bluetooth connection...");
}

// ───────────────────────────────────────────────
void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      handleConfigCommand(configBuffer);
      configBuffer = "";
    } else {
      configBuffer += c;
    }
  }

  if (!bleKeyboard.isConnected()) {
    digitalWrite(BLUE_LED, LOW);
    delay(500);
    digitalWrite(BLUE_LED, HIGH);
    delay(500);
    if (lastConnected) {
      Serial.println("Bluetooth disconnected.");
      lastConnected = false;
    }
    return;
  }

  if (!lastConnected) {
    Serial.println("Bluetooth connected!");
    flashBlue(5, 100);
    digitalWrite(BLUE_LED, HIGH);
    lastConnected = true;
  }

  // ─── Button handling ───
  for (uint8_t i = 0; i < numButtons; i++) {
    bool currentState = digitalRead(buttonPins[i]);
    if (currentState == LOW && lastButtonStates[i] == HIGH) {
      KeyConfig kc = keyMappings[i];
      Serial.printf("%s pressed (code %d)\n", buttonNames[i], kc.keyCode);

      if (kc.ctrl)  bleKeyboard.press(KEY_LEFT_CTRL);
      if (kc.alt)   bleKeyboard.press(KEY_LEFT_ALT);
      if (kc.shift) bleKeyboard.press(KEY_LEFT_SHIFT);
      if (kc.gui)   bleKeyboard.press(KEY_LEFT_GUI);

      bleKeyboard.press(kc.keyCode);
    } 
    else if (currentState == HIGH && lastButtonStates[i] == LOW) {
      KeyConfig kc = keyMappings[i];
      bleKeyboard.release(kc.keyCode);

      if (kc.ctrl)  bleKeyboard.release(KEY_LEFT_CTRL);
      if (kc.alt)   bleKeyboard.release(KEY_LEFT_ALT);
      if (kc.shift) bleKeyboard.release(KEY_LEFT_SHIFT);
      if (kc.gui)   bleKeyboard.release(KEY_LEFT_GUI);

      Serial.printf("%s released\n", buttonNames[i]);
    }
    lastButtonStates[i] = currentState;
  }
}
