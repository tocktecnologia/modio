#define TRIGGER_PORTAL 4 // GPIO 4
#define LED_BUILTIN 2    // GPIO 2

#include <Arduino.h>
#include "TockWiFiManager.h"
#include "TockMQTT.h"
// #include "OtaUpdate.h"

void setup()
{
  Serial.begin(9600);
  tockDebug = true;
  pinMode(LED_BUILTIN, OUTPUT);
  flipper.attach(0.05, flip); // pisca rapido enquanto tenta se conectar ao wifi

  setupWM();
  setupMQTT();
}

void loop()
{

  loopWM();
  loopMQTT();
}