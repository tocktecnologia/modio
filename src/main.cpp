#include <Arduino.h>
#include "utils.h"

#include "TockWiFiManager.h"
#include "TockMQTT.h"

void setup()
{
  Serial.begin(9600);
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
