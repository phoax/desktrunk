#include <Arduino.h>

#include "BluetoothSerial.h"

BluetoothSerial ESP_BT;

int incoming;

void setup(void) {
  Serial.begin(9600);

  ESP_BT.begin("ESP32_LED_Control");
  Serial.println("Bluetooth Device is Ready to Pair");
}

void loop(void) {
  if (ESP_BT.available()) //Check if we receive anything from Bluetooth
  {
    incoming = ESP_BT.read(); //Read what we recevive 
    Serial.print("Received:");
    
    Serial.println(incoming);

    if (incoming == 49) {
      ESP_BT.println("LED turned ON");
    }
        
    if (incoming == 48) {
      ESP_BT.println("LED turned OFF");
    }
  }
  ESP_BT.println("Bluuuuuuuuuuue");
  delay(1000);
}