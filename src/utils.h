#include <ESP8266WiFi.h>
#include <Ticker.h>

Ticker flipper;
void flip()
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // set pin to the opposite state
}

String esp8266ID()
{
    byte mac[6];
    WiFi.macAddress(mac);
    return String(mac[2], HEX) + String(mac[3], HEX) + String(mac[4], HEX) + String(mac[5], HEX);
}
