#ifndef Desktrunk_h
#define Desktrunk_h

#include <CheapStepper.h>

class Desktrunk
{
  private:
    const char * _secret;
    const char * getSecret();
    int _stepperIN1;
    int _stepperIN2;
    int _stepperIN3;
    int _stepperIN4;
    int _ledRed;
    int _ledGreen;
    int _ledBlue;
  public:
    struct trunk {
      bool opened = false;
      bool opening = false;
      bool closed = true;
      bool closing = false;
    } trunk;
    CheapStepper _stepper;
    int _P2N2222;
    void init();
    void setSecret(const char * secret);
    void setStepper(int IN1, int IN2, int IN3, int IN4);
    void setLed(int red, int green, int blue);
    
    void check();
    bool checkAccess(char * data);
    void closingTrunk();
    void closeTrunk();
    void openingTrunk();
    void openTrunk();
    void turnOffStepper();
    void ledStart();
    void ledError();
    void ledSuccess();

};
 
#endif