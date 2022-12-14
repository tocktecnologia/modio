#include <FS.h>        
#include <LittleFS.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "utils.h"



WiFiManager wm;
WiFiManagerParameter custom_io("IO", "In/Out", "o", 1);
WiFiManagerParameter custom_server("server", "server", "192.168.0.5", 25);
WiFiManagerParameter custom_pin1("16", "GPIO 16", "1", 3); //D0
WiFiManagerParameter custom_pin2("5", "GPIO 05", "2", 3); //D1
WiFiManagerParameter custom_pin3("0", "GPIO 0", "3", 3); //D3
WiFiManagerParameter custom_pin4("3", "GPIO 03", "4", 3); //RX
WiFiManagerParameter custom_pin5("14", "GPIO 14", "5", 3); //D5
WiFiManagerParameter custom_pin6("12", "GPIO 12", "6", 3); //D6
WiFiManagerParameter custom_pin7("13", "GPIO 13", "7", 3); //D7
WiFiManagerParameter custom_pin8("15", "GPIO 15", "8", 3); //D8
// WiFiManagerParameter custom_pin9("10", "GPIO 10", "9", 3); //S3

std::vector<WiFiManagerParameter> wifiParamsPins(8);

const char * filepathStates = "/config_states.json";
const char * filepath = "/config.json";
const char * wiFiPassword = "tock1234";
void checkConfigButton();
void saveParamsCallback();
void configurePins();

void setupWM() {
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    
    Serial.begin(9600);
    
    //reset settings - wipe credentials for testing
    // wm.resetSettings();
    // SPIFFS.format();
    // LittleFS.format();
    
    // Open file saved tp retrieve json
    if (!LittleFS.begin()){
        Serial.println("LittleFS mount failed");return;
    }   

    // read pins configurations
    String filedata = readFile(LittleFS, filepath);
    DynamicJsonDocument fileJson(256);
    JsonObject fileJsonObj;
    if (filedata != String()){
        deserializeJson(fileJson, filedata);
        fileJsonObj = fileJson.as<JsonObject>();
        serializeJsonPretty(fileJson,Serial);
        
        custom_io.setValue(fileJsonObj["io"].as<String>().c_str(),1); 
        custom_server.setValue(fileJsonObj["server"].as<String>().c_str(),25); 
        custom_pin1.setValue(fileJsonObj["Pin1"].as<String>().c_str(),3); 
        custom_pin2.setValue(fileJsonObj["Pin2"].as<String>().c_str(),3);
        custom_pin3.setValue(fileJsonObj["Pin3"].as<String>().c_str(),3);        
        custom_pin4.setValue(fileJsonObj["Pin4"].as<String>().c_str(),3);
        custom_pin5.setValue(fileJsonObj["Pin5"].as<String>().c_str(),3);
        custom_pin6.setValue(fileJsonObj["Pin6"].as<String>().c_str(),3);
        custom_pin7.setValue(fileJsonObj["Pin7"].as<String>().c_str(),3);
        custom_pin8.setValue(fileJsonObj["Pin8"].as<String>().c_str(),3);
        // custom_pin9.setValue(fileJsonObj["Pin9"].as<String>().c_str(),3);
    

        fileJson.clear();
        fileJsonObj.clear();

    } else Serial.println("file empty!");

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

    wm.setWiFiAutoReconnect(true);
    wm.setConfigPortalTimeout(90);
    wm.setConnectTimeout(30);
    wm.setSaveParamsCallback(saveParamsCallback);
    
    String thingName = esp8266ID();
    String clientId = "TOCK-" + thingName;
    if(wm.autoConnect(clientId.c_str(),wiFiPassword)){
        Serial.println("connected...yeey :)");
        flipper.attach(0.2, flip);
        configurePins();
    }
    else {
        Serial.println("Configportal running");
    }

}

void loopWM() {
    wm.process();
    // checkConfigButton();
    

}

void saveParamsCallback () {
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

void configurePins(){
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
        for(unsigned int i=0;i<wifiParamsPins.size();i++){
            // ModIn
            int pinId = String(wifiParamsPins[i].getID()).toInt();
            if(String(custom_io.getValue()) == "i"){
                pinMode(pinId, INPUT);
            }
            // ModOut
            else {
                pinMode(pinId, OUTPUT);

                // if there're states in memory, update GPIOs
                if(filedata!=String()){
                    auto key = String("pin" + String(wifiParamsPins[i].getValue()));
                    auto stateMemory = fileJson.as<JsonObject>()["state"]["reported"][key.c_str()].as<String>().toInt();
                  
                    digitalWrite(pinId,!stateMemory);
                
                    String debug = key + ": (GPIO " + pinId +  "):" + String(stateMemory);
                    Serial.println(debug);
                }
            } 
        }
        fileJson.clear();   
        filedata.clear();

}