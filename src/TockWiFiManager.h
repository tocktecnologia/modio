#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "utils.h"

WiFiManager wm;
WiFiManagerParameter custom_io("IO", "In/Out", "o", 1);
WiFiManagerParameter custom_server("server", "server", "192.168.0.32", 25);
WiFiManagerParameter custom_pin1("16", "GPIO 16", "1", 3); // D0
WiFiManagerParameter custom_pin2("5", "GPIO 05", "2", 3);  // D1
WiFiManagerParameter custom_pin3("4", "GPIO 04", "3", 3);  // D2
WiFiManagerParameter custom_pin4("14", "GPIO 14", "4", 3); // D5
WiFiManagerParameter custom_pin5("12", "GPIO 12", "5", 3); // D6
WiFiManagerParameter custom_pin6("13", "GPIO 13", "6", 3); // D7
WiFiManagerParameter custom_pin7("3", "GPIO 03", "7", 3);  // D8// nao funciona como saida
WiFiManagerParameter custom_pin8("15", "GPIO 15", "8", 3); // RX  precisaria de um pull down ou pullup

// WiFiManagerParameter custom_pin7("10", "GPIO 10", "7", 3); // D8// nao funciona como saida
// WiFiManagerParameter custom_pin8("3", "GPIO 03", "8", 3); // RX
// based in this image: https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/05/ESP8266-NodeMCU-kit-12-E-pinout-gpio-pin.png?resize=817%2C542&quality=100&strip=all&ssl=1

std::vector<WiFiManagerParameter> wifiParamsPins(8);

const char *filepathStates = "/states.json";
const char *filepath = "/conf.json";
const char *wiFiPassword = "tock1234";
void checkConfigButton();
void saveParamsCallback();
void configurePins();

void setupWM()
{
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    Serial.begin(9600);

    // reset settings - wipe credentials for testing
    // wm.resetSettings();
    // LittleFS.format();

    // Open file saved tp retrieve json
    if (!LittleFS.begin())
    {
        Serial.println("LittleFS mount failed");
        return;
    }

    // read pins configurations
    String filedata = readFile(LittleFS, filepath);
    DynamicJsonDocument fileJson(256);
    JsonObject fileJsonObj;
    if (filedata != String())
    {
        deserializeJson(fileJson, filedata);
        fileJsonObj = fileJson.as<JsonObject>();
        serializeJsonPretty(fileJson, Serial);

        custom_io.setValue(fileJsonObj["io"].as<String>().c_str(), 1);
        custom_server.setValue(fileJsonObj["server"].as<String>().c_str(), 25);
        custom_pin1.setValue(fileJsonObj["Pin1"].as<String>().c_str(), 3);
        custom_pin2.setValue(fileJsonObj["Pin2"].as<String>().c_str(), 3);
        custom_pin3.setValue(fileJsonObj["Pin3"].as<String>().c_str(), 3);
        custom_pin4.setValue(fileJsonObj["Pin4"].as<String>().c_str(), 3);
        custom_pin5.setValue(fileJsonObj["Pin5"].as<String>().c_str(), 3);
        custom_pin6.setValue(fileJsonObj["Pin6"].as<String>().c_str(), 3);
        custom_pin7.setValue(fileJsonObj["Pin7"].as<String>().c_str(), 3);
        custom_pin8.setValue(fileJsonObj["Pin8"].as<String>().c_str(), 3);
        // custom_pin9.setValue(fileJsonObj["Pin9"].as<String>().c_str(),3);

        fileJson.clear();
        fileJsonObj.clear();
    }
    else
        Serial.println("file empty!");

    wm.addParameter(&custom_io);
    wm.addParameter(&custom_server);
    wm.addParameter(&custom_pin1);
    wm.addParameter(&custom_pin2);
    wm.addParameter(&custom_pin3);
    wm.addParameter(&custom_pin4);
    wm.addParameter(&custom_pin5);
    wm.addParameter(&custom_pin6);
    wm.addParameter(&custom_pin7);
    wm.addParameter(&custom_pin8);
    // wm.addParameter(&custom_pin9);

    wm.setConfigPortalBlocking(true);
    wm.setWiFiAutoReconnect(true);
    wm.setConfigPortalTimeout(70);
    wm.setConnectTimeout(15);
    wm.setSaveParamsCallback(saveParamsCallback);

    String thingName = esp8266ID();
    String clientId = "TOCK-" + thingName;

    configurePins();

    if (wm.autoConnect(clientId.c_str(), wiFiPassword))
    {
        Serial.println("connected...yeey :)");
        flipper.attach(0.2, flip);
    }
    else
    {
        Serial.println("Configportal running");
    }
}

void loopWM()
{
    wm.process();
    // checkConfigButton();
}

void saveParamsCallback()
{
    // configurePins();

    StaticJsonDocument<256> jsonToFile;
    jsonToFile["io"] = custom_io.getValue();
    jsonToFile["server"] = custom_server.getValue();
    jsonToFile["Pin1"] = custom_pin1.getValue();
    jsonToFile["Pin2"] = custom_pin2.getValue();
    jsonToFile["Pin3"] = custom_pin3.getValue();
    jsonToFile["Pin4"] = custom_pin4.getValue();
    jsonToFile["Pin5"] = custom_pin5.getValue();
    jsonToFile["Pin6"] = custom_pin6.getValue();
    jsonToFile["Pin7"] = custom_pin7.getValue();
    jsonToFile["Pin8"] = custom_pin8.getValue();
    // jsonToFile["Pin9"] = custom_pin9.getValue();

    writeFile(LittleFS, filepath, jsonToFile.as<String>().c_str());
    jsonToFile.clear();
}

void checkConfigButton()
{
    if (digitalRead(TRIGGER_PORTAL))
    {
        delay(1000);
        if (digitalRead(TRIGGER_PORTAL))
        {
            // flipper.detach(); // pisca lento se conectado ao wifi
            digitalWrite(LED_BUILTIN, 1);
            wm.resetSettings(); // reset settings?
            //  wm.setConfigPortalBlocking(false);
            //  wm.startConfigPortal();
            wm.startWebPortal();
            delay(1000);
            ESP.reset();
        }
    }
}

void configurePins()
{
    wifiParamsPins.clear();
    wifiParamsPins.push_back(custom_pin1);
    wifiParamsPins.push_back(custom_pin2);
    wifiParamsPins.push_back(custom_pin3);
    wifiParamsPins.push_back(custom_pin4);
    wifiParamsPins.push_back(custom_pin5);
    wifiParamsPins.push_back(custom_pin6);
    wifiParamsPins.push_back(custom_pin7);
    wifiParamsPins.push_back(custom_pin8);
    // wifiParamsPins.push_back(custom_pin9);

    StaticJsonDocument<256> fileJson;
    String filedata = readFile(LittleFS, filepathStates);
    deserializeJson(fileJson, filedata);
    // serializeJsonPretty(fileJson,Serial); //debug

    // configs
    for (unsigned int i = 0; i < wifiParamsPins.size(); i++)
    {
        // ModIn
        int pinId = String(wifiParamsPins[i].getID()).toInt();
        if (String(custom_io.getValue()) == "i")
        {
            pinMode(pinId, INPUT);
        }
        // ModOut
        else
        {
            pinMode(pinId, OUTPUT);

            // if there're states in memory, update GPIOs
            if (filedata != String())
            {
                auto key = String("pin" + String(wifiParamsPins[i].getValue()));
                auto stateMemory = fileJson.as<JsonObject>()["state"]["reported"][key.c_str()].as<String>().toInt();

                digitalWrite(pinId, !stateMemory);

                String debug = key + ": (GPIO " + pinId + "):" + String(stateMemory);
                Serial.println(debug);
            }
        }
    }
    fileJson.clear();
    filedata.clear();
}