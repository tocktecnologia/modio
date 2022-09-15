#ifndef UTILS
#define UTILS

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

bool tockDebug = false;
void tockPrint(String str)
{
    if (tockDebug)
    {
        Serial.print("TOCK:");
        Serial.println(str);
    }
}

String readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);
    File file = fs.open(path, "r");
    if (!file || file.isDirectory())
    {
        return String();
    }
    String fileContent;
    while (file.available())
    {
        fileContent += String((char)file.read());
    }
    file.close();
    // Serial.println(fileContent);
    return fileContent;
}
String writeFile(fs::FS &fs, const char *path, const char *message)
{
    File file = fs.open(path, "w");
    if (!file)
    {
        return String();
    }
    if (file.print(message))
    {
        file.close();
        return String(message);
    }
    else
    {
        file.close();
        return String();
    }
}


int getPinIdOutput(int pinIdReceivedMqtt, std::vector<WiFiManagerParameter> &wifiParamsPins){
    for(unsigned int i=0;i<wifiParamsPins.size();i++){
        int pinIdConfigured = String(wifiParamsPins[i].getValue()).toInt();
        if(pinIdReceivedMqtt==pinIdConfigured) 
            return String(wifiParamsPins[i].getID()).toInt();
    }
    return -1;
}

#endif
