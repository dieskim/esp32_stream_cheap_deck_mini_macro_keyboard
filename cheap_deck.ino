#include <BleKeyboard.h>
#include <Adafruit_NeoPixel.h>

BleKeyboard bleKeyboard("Super Keys", "ESP32", 100); // Device name

const uint8_t button1Pin = 10;
const uint8_t button2Pin = 9;
const uint8_t button3Pin = 3;
const uint8_t button4Pin = 2;
const uint8_t button5Pin = 20;
const uint8_t button6Pin = 21;
const uint8_t button7Pin = 1;
const uint8_t button8Pin = 0;

const uint8_t buttonPins[] = {button1Pin, button2Pin, button3Pin, button4Pin, button5Pin, button6Pin, button7Pin, button8Pin};
const uint8_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

// Array to store button names for logging
const char* buttonNames[] = {"Button 1", "Button 2", "Button 3", "Button 4", "Button 5", "Button 6", "Button 7", "Button 8"};

// Function keys F13 to F20
const uint8_t functionKeys[] = {KEY_F13, KEY_F14, KEY_F15, KEY_F16, KEY_F17, KEY_F18, KEY_F19};

// Array to track previous button states (HIGH = unpressed, LOW = pressed)
bool lastButtonStates[numButtons];

void setup() {
  Serial.begin(115200);
  bleKeyboard.begin();
  
  // Set all button pins as inputs with pull-up resistors
  for (uint8_t i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonStates[i] = HIGH; // Initialize as unpressed
  }
  Serial.println("Setup complete, waiting for connection...");
}

void loop() {
  if (bleKeyboard.isConnected()) {
    for (uint8_t i = 0; i < numButtons; i++) {
      bool currentState = digitalRead(buttonPins[i]);
      
      // Button pressed (HIGH to LOW)
      if (currentState == LOW && lastButtonStates[i] == HIGH) {
        Serial.print(buttonNames[i]);
        Serial.print(" (pin ");
        Serial.print(buttonPins[i]);
        Serial.print(") pressed, sending ");
        if (i == 7) { // Button 8
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
      }
      // Button released (LOW to HIGH)
      else if (currentState == HIGH && lastButtonStates[i] == LOW) {
        Serial.print(buttonNames[i]);
        Serial.print(" (pin ");
        Serial.print(buttonPins[i]);
        Serial.print(") released, sending ");
        if (i == 7) { // Button 8
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
  } else {
    Serial.println("Waiting for Bluetooth connection...");
    delay(500);
  }
}