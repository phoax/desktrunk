#include <Arduino.h>
#include <Desktrunk.h>
#include <NFC_Factorim.h>

#include <Thread.h>
#include <ThreadController.h>

const char * secret = "sesame";

#define INTERRUPT_PIN (1)

#define STEPPER_IN1 (15)
#define STEPPER_IN2 (2)
#define STEPPER_IN3 (0)
#define STEPPER_IN4 (4)

#define PN532_SCK  (18)
#define PN532_MOSI (23)
#define PN532_MISO (19)
#define PN532_SS   (5)

#define LED_RED (27)
#define LED_GREEN (26)
#define LED_BLUE (25)

Desktrunk desktrunk;

NFC_Factorim nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

/**
 * Callback for nfc
 */
void nfcCallback(){
  char * data = nfc.readPassive();

  if (data) {
    if (desktrunk.checkAccess(data)) {
      if (desktrunk.trunk.closed == true) {
        Serial.println("Access granted");
        desktrunk.openingTrunk();
      } else {
        Serial.println("Already opened");
        desktrunk.ledSuccess();
      }
    }
  }
}

/**
 * Closing front
 */
void closingFront() {
  if (desktrunk.trunk.closed) {
    desktrunk.openingTrunk();
  } else {
    desktrunk.closingTrunk();
  }
}

void setup(void) {
  Serial.begin(9600);
  Serial.println("Desktrunk starting");

  nfc.begin();
  nfc.nfcInfo();
  nfc.SAMConfig();

  desktrunk.init();
  desktrunk.setSecret(secret);
  desktrunk.setStepper(STEPPER_IN1, STEPPER_IN2, STEPPER_IN3, STEPPER_IN4);
  desktrunk.setLed(LED_RED, LED_GREEN, LED_BLUE);

  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), closingFront, CHANGE);

  desktrunk.ledStart();
}

void loop(void) {
  nfcCallback();
  desktrunk.check();
}