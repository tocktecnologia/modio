#include <Arduino.h>
#include "utils.h"

#include "TockWiFiManager.h"
#include "TockMQTT.h"

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  setupWM();

  setupMQTT();
}

void loop()
{
  loopWM();
  loopMQTT();
}
