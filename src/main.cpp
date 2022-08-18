#define N_PINS 13
#define TRIGGER_PORTAL 4

#include <Arduino.h>
#include "TockWiFiManager.h"
#include "TockMQTT.h"

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