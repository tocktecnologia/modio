#include "Arduino.h"
#include "ESP8266httpUpdate.h"
String version = "1.0.0.0";
void doUpdate();

void setupOta()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin("Júnior e Camila", "DamkpKCk");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(150);
  }

  Serial.print("Wifi IP:");
  Serial.println(WiFi.localIP());

  // Setup LED GPIO
  pinMode(2,OUTPUT);
}

int updateCheckTimer = 0;
void loopOta()
{
  // Lets blink
  digitalWrite(2, 1);
  delay(100);
  digitalWrite(2, 0);
  delay(100);

  updateCheckTimer++;
  if(updateCheckTimer > 30)
  {
    updateCheckTimer = 0;
    doUpdate();
  }
}

void doUpdate()
{
  String url = "http://otadrive.com/DeviceApi/GetEsp8266Update?k=9a03f4fb-98c5-4963-9d39-cc9181f65edd";
  url += "&s=" + String(CHIPID);
  url += "&v=" + version;


  t_httpUpdate_return ret = ESPhttpUpdate.update(espClient,url, version);
  switch (ret)
  {
  case HTTP_UPDATE_FAILED:
    Serial.println("Update faild!");
    break;
  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("No new update available");
    break;
  // We can't see this, because of reset chip after update OK
  case HTTP_UPDATE_OK:
    Serial.println("Update OK");
    break;

  default:
    break;
  }
}
