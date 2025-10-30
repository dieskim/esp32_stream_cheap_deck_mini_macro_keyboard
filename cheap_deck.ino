#include <BleKeyboard.h>
#include <Preferences.h>
#include <NimBLEDevice.h>

// Detect board type at compile time
#if defined(CONFIG_IDF_TARGET_ESP32S3)
  #define IS_ESP32_S3 true
  #include <Adafruit_NeoPixel.h>
  #define RGB_LED_PIN 48
  Adafruit_NeoPixel rgbLed(1, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
  #define IS_ESP32_S3 false
  #define BLUE_LED 8
#else
  #error "Unsupported board type"
#endif

BleKeyboard bleKeyboard("Super Keys", "ESP32", 100);
Preferences preferences;

// BUTTONS WHEN LOOKING FROM FRONT
// TOP LEFT     1 2 3 4     TOP RIGHT
// BOTTOM LEFT  5 6 7 8     BOTTOM RIGHT
#if IS_ESP32_S3
  const uint8_t buttonPins[] = {1, 2, 13, 4, 5, 6, 7, 8};
#else
  const uint8_t buttonPins[] = {20, 9, 2, 1, 21, 10, 3, 0};
#endif

const uint8_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
const char* buttonNames[] = {
  "Button 1", "Button 2", "Button 3", "Button 4",
  "Button 5", "Button 6", "Button 7", "Button 8"
};

// ───────────────────────────────────────────────
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

bool lastButtonStates[numButtons];
bool lastConnected = false;
bool configMode = false;
String configBuffer = "";

// ───────────────────────────────────────────────
// LED Control Functions
void setLED(uint8_t r, uint8_t g, uint8_t b) {
#if IS_ESP32_S3
  rgbLed.setPixelColor(0, rgbLed.Color(r, g, b));
  rgbLed.show();
#else
  // C3 only has blue LED - turn on if any color requested
  digitalWrite(BLUE_LED, (r || g || b) ? LOW : HIGH);
#endif
}

void flashLED(int times, int delayMs, uint8_t r = 0, uint8_t g = 0, uint8_t b = 255) {
  for (int i = 0; i < times; i++) {
    setLED(r, g, b);
    delay(delayMs);
    setLED(0, 0, 0);
    delay(delayMs);
  }
}

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
  
  // USB CDC stability improvement for both boards
  Serial.setTxTimeoutMs(0);
  while (!Serial && millis() < 1500) delay(10);

#if IS_ESP32_S3
  Serial.println("USB CDC ready (ESP32-S3)");
  
  // Initialize RGB LED
  rgbLed.begin();
  rgbLed.setBrightness(50);
  setLED(0, 0, 0);
#else
  Serial.println("Starting (ESP32-C3)");
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);
#endif

  Serial.println("Startup...");

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

  bool connected = bleKeyboard.isConnected();
  static uint32_t lastAdvKick = 0;
  static uint32_t lastBlink = 0;

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
      setLED(0, 0, ledState ? 50 : 0);
    }
    
    delay(100);
    return;
  }

  if (!lastConnected) {
    Serial.println("Bluetooth connected!");
    flashLED(5, 100, 0, 255, 0); // Green flash on S3, blue on C3
    setLED(0, 0, 0);
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