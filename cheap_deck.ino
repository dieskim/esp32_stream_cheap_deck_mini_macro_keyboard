#include <BleKeyboard.h>
#include <Adafruit_NeoPixel.h>

BleKeyboard bleKeyboard("Super Keys", "ESP32", 100);

const uint8_t buttonPins[] = {10, 9, 3, 2, 20, 21, 1, 0};
const uint8_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

const char* buttonNames[] = {"Button 1", "Button 2", "Button 3", "Button 4", "Button 5", "Button 6", "Button 7", "Button 8"};
const uint8_t functionKeys[] = {KEY_F13, KEY_F14, KEY_F15, KEY_F16, KEY_F17, KEY_F18, KEY_F19};

bool lastButtonStates[numButtons];

#define PIN        8
#define NUMPIXELS  1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

enum LedState {
  LED_BOOT,
  LED_WAITING,
  LED_CONNECTED_OFF,
  LED_CONNECTED_BUTTON
};

LedState currentLedState = LED_BOOT;

void setLedColor(LedState state) {
  if (state == currentLedState) return; // No change needed

  currentLedState = state;
  switch(state) {
    case LED_BOOT:
      pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Red
      break;
    case LED_WAITING:
      pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Blue
      break;
    case LED_CONNECTED_OFF:
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));   // Off
      break;
    case LED_CONNECTED_BUTTON:
      pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Green
      break;
  }
  pixels.show();
}

void setup() {
  Serial.begin(115200);
  pixels.begin();

  setLedColor(LED_BOOT);

  bleKeyboard.begin();

  for (uint8_t i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonStates[i] = HIGH;
  }

  Serial.println("Setup complete, waiting for connection...");
}

void loop() {
  if (!bleKeyboard.isConnected()) {
    setLedColor(LED_WAITING);
    Serial.println("Waiting for Bluetooth connection...");
    delay(500);
    return;
  }

  // Connected
  bool anyButtonDown = false;

  for (uint8_t i = 0; i < numButtons; i++) {
    bool currentState = digitalRead(buttonPins[i]);

    if (currentState == LOW) {
      anyButtonDown = true;
    }

    if (currentState == LOW && lastButtonStates[i] == HIGH) {
      Serial.print(buttonNames[i]);
      Serial.print(" (pin ");
      Serial.print(buttonPins[i]);
      Serial.print(") pressed, sending ");
      if (i == 7) {
        Serial.println("Command+Option+Control+F19 DOWN");
        bleKeyboard.press(KEY_LEFT_CTRL);
        bleKeyboard.press(KEY_LEFT_ALT);
        bleKeyboard.press(KEY_LEFT_GUI);
        bleKeyboard.press(KEY_F19);
      } else {
        Serial.print("F");
        Serial.print(13 + i);
        Serial.println(" DOWN");
        bleKeyboard.press(functionKeys[i]);
      }
      delay(50);
    } else if (currentState == HIGH && lastButtonStates[i] == LOW) {
      Serial.print(buttonNames[i]);
      Serial.print(" (pin ");
      Serial.print(buttonPins[i]);
      Serial.print(") released, sending ");
      if (i == 7) {
        Serial.println("Command+Option+Control+F19 UP");
        bleKeyboard.release(KEY_F19);
        bleKeyboard.release(KEY_LEFT_CTRL);
        bleKeyboard.release(KEY_LEFT_ALT);
        bleKeyboard.release(KEY_LEFT_GUI);
      } else {
        Serial.print("F");
        Serial.print(13 + i);
        Serial.println(" UP");
        bleKeyboard.release(functionKeys[i]);
      }
      delay(50);
    }

    lastButtonStates[i] = currentState;
  }

  if (anyButtonDown) {
    setLedColor(LED_CONNECTED_BUTTON);
  } else {
    setLedColor(LED_CONNECTED_OFF);
  }
}
