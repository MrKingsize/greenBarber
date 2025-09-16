// Project Name: MCUModulesEsp32
// Board: Espressif ESP32 Dev Module
// Framework: Arduino
// Language Standard: C++11
// USB controller: cp2102 https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads

#include <Arduino.h>

#define LED_PIN 2   // On most ESP32 DevKit boards the onboard LED is on GPIO2

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  static bool ledState = false;

  // Toggle LED state
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);

  // Print LED state to Serial Monitor
  Serial.print("LED is now: ");
  Serial.println(ledState ? "ON" : "OFF");

  delay(1000);  // wait 1 second
}
