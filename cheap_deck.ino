#include <BleKeyboard.h>

BleKeyboard bleKeyboard("Super Keys", "ESP32", 100);

// BUTTONS WHEN LOOKING FROM FRONT 
// TOP LEFT     1 2 3 4     TOP RIGHT
// BOTTOM LEFT  5 6 7 8     BOTTOM RIGHT
const uint8_t buttonPins[] = {
  20, // Button 1
  9,  // Button 2
  2,  // Button 3
  1,  // Button 4
  21, // Button 5
  10, // Button 6
  3,  // Button 7
  0   // Button 8
};
const uint8_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

const char* buttonNames[] = {"Button 1", "Button 2", "Button 3", "Button 4", "Button 5", "Button 6", "Button 7", "Button 8"};
const uint8_t functionKeys[] = {KEY_F13, KEY_F14, KEY_F15, KEY_F16, KEY_F17, KEY_F18, KEY_F19};

bool lastButtonStates[numButtons];

// Blue LED (GPIO8)
#define BLUE_LED 8

// Track connection state for logging + LED behavior
bool lastConnected = false;

void flashBlue(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BLUE_LED, LOW);
    delay(delayMs);
    digitalWrite(BLUE_LED, HIGH);
    delay(delayMs);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Startup...");

  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH); // start off

  bleKeyboard.begin();

  for (uint8_t i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonStates[i] = HIGH;
  }

  Serial.println("Setup complete, waiting for Bluetooth connection...");
}

void loop() {
  if (!bleKeyboard.isConnected()) {
    // Flash slowly while waiting
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

  // Just connected
  if (!lastConnected) {
    Serial.println("Bluetooth connected!");
    flashBlue(5, 100);  // flash quickly 5 times
    digitalWrite(BLUE_LED, HIGH); // turn off after connect
    lastConnected = true;
  }

  // --- Button handling ---
  for (uint8_t i = 0; i < numButtons; i++) {
    bool currentState = digitalRead(buttonPins[i]);

    if (currentState == LOW && lastButtonStates[i] == HIGH) {
      Serial.print(buttonNames[i]);
      Serial.print(" pressed, sending ");
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
    } else if (currentState == HIGH && lastButtonStates[i] == LOW) {
      Serial.print(buttonNames[i]);
      Serial.println(" released");
      if (i == 7) {
        bleKeyboard.release(KEY_F19);
        bleKeyboard.release(KEY_LEFT_CTRL);
        bleKeyboard.release(KEY_LEFT_ALT);
        bleKeyboard.release(KEY_LEFT_GUI);
      } else {
        bleKeyboard.release(functionKeys[i]);
      }
    }

    lastButtonStates[i] = currentState;
  }
}
