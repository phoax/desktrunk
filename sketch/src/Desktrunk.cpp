#include "Desktrunk.h"

#include "Arduino.h"
#include <CheapStepper.h>

#include <Regexp.h>

#include <EEPROM.h>
#define EEPROM_SIZE 64
#define EEPROM_ADDRESS 0

int step = 2800;
int stepOpen;
int stepClose;

/**
 * Init desktrunk
 */
void Desktrunk::init () {
	if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }

  int trunkStatus = EEPROM.read(EEPROM_ADDRESS);
  if (trunkStatus == 1) {
    Serial.println("Trunk close");
    trunk.opened = false;
    trunk.closed = true;
    stepClose = 0;
    stepOpen = step;
  } else {
    Serial.println("Trunk open");
    trunk.opened = true;
    trunk.closed = false;
    stepClose = step - 1200;
    stepOpen = 0;
  }
}

/**
 * Set secret password
 * @param secret Secret password
 */
void Desktrunk::setSecret(const char * secret) {
  _secret = secret;
}

/**
 * Get secret password
 * @returns _secret Secret password
 */
const char * Desktrunk::getSecret() {
  return _secret;
}

/**
 * Set stepper
 * @param IN1, IN2, IN3, IN4, Pins of the stepper
 */
void Desktrunk::setStepper(int IN1, int IN2, int IN3, int IN4) {
  _stepperIN1 = IN1;
  _stepperIN2 = IN2;
  _stepperIN3 = IN3;
  _stepperIN4 = IN4;
  CheapStepper stepper(IN1, IN2, IN3, IN4);
  _stepper = stepper;
}

/**
 * Set led
 * @param red, green, blue, Pins of the stepper
 */
void Desktrunk::setLed(int red, int green, int blue) {
  _ledRed = red;
  _ledGreen = green;
  _ledBlue = blue;
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
}

/**
 * Check trunk status
 */
void Desktrunk::check() {
  if (trunk.closing) {
    Serial.println("check is closing");
    closeTrunk();
  }
  if (trunk.opening) {
    Serial.println("check is opening");
    openTrunk();
  }
}

/**
 * Check access from data
 * @param data Data from the NFC
 * @returns if access granted or not
 */
bool Desktrunk::checkAccess(char * data) {
  MatchState ms;
  ms.Target (data);
  char result = ms.Match (_secret, 0);
  if (result == REGEXP_MATCHED)
  {
    return true;
  } else {
    Serial.println("Nfc secret not allowed");
    ledError();
    return false;
  }
}

/**
 * Start closing trunk
 */
void Desktrunk::closingTrunk() {
  digitalWrite(_ledGreen, HIGH);
  if (!trunk.closing) {
    Serial.print("Closing trunk");
    trunk.closing = true;
  }
}

/**
 * Close trunk
 */
void Desktrunk::closeTrunk() {
  if (trunk.closing) {
    Serial.println("Close trunk");
    Serial.print(stepClose);
    _stepper.moveTo(true, stepClose);
    EEPROM.write(EEPROM_ADDRESS, 1);
    EEPROM.commit();
    trunk.closing = false;
    trunk.opened = false;
    trunk.closed = true;
    Serial.print("Trunk closed");
    turnOffStepper();
    // Led off
    delay(1000);
    digitalWrite(_ledGreen, LOW);
  }
}

/**
 * Start opening trunk
 */
void Desktrunk::openingTrunk() {
  digitalWrite(_ledGreen, HIGH);
  if (!trunk.opening) {
    Serial.print("Opening trunk");
    trunk.opening = true;
  }
}

/**
 * Open trunk
 */
void Desktrunk::openTrunk() {
  if (trunk.opening) {
    Serial.println("Open trunk:");
    Serial.print(stepOpen);
    // _stepper.moveTo(true, stepClose + 100);
    _stepper.moveTo(false, stepOpen);
    EEPROM.write(EEPROM_ADDRESS, 0);
    EEPROM.commit();
    trunk.opening = false;
    trunk.opened = true;
    trunk.closed = false;
    Serial.print("Trunk opened");
    turnOffStepper();
    // Led off
    delay(1000);
    digitalWrite(_ledGreen, LOW);
  }
}

/**
 * Turn off stepper
 */
void Desktrunk::turnOffStepper() {
  Serial.println("Turn off stepper");
  digitalWrite(_stepperIN1, LOW);
  digitalWrite(_stepperIN2, LOW);
  digitalWrite(_stepperIN3, LOW);
  digitalWrite(_stepperIN4, LOW);
}

/**
 * Led turn on when start
 */
void Desktrunk::ledStart() {
  digitalWrite(_ledBlue, HIGH);
  delay(500);
  digitalWrite(_ledBlue, LOW);
  delay(500);

  digitalWrite(_ledBlue, HIGH);
  delay(500);
  digitalWrite(_ledBlue, LOW);
  delay(500);

  digitalWrite(_ledBlue, HIGH);
  delay(500);
  digitalWrite(_ledBlue, LOW);
  delay(500);

  digitalWrite(_ledBlue, HIGH);
  delay(500);
  digitalWrite(_ledBlue, LOW);
}

/**
 * Led turn on when error
 */
void Desktrunk::ledError() {
  digitalWrite(_ledRed, HIGH);
  delay(2000);
  digitalWrite(_ledRed, LOW);
}

/**
 * Led turn on when success
 */
void Desktrunk::ledSuccess() {
  digitalWrite(_ledGreen, HIGH);
  delay(2000);
  digitalWrite(_ledGreen, LOW);
}