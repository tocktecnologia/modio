#include <FS.h>        
#include <LittleFS.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "utils.h"


WiFiManager wm;

WiFiManagerParameter custom_io("IO", "In/Out", "o", 1);
WiFiManagerParameter custom_server("server", "server", "192.168.0.5", 15);
WiFiManagerParameter custom_pin1("Pin1", "Pin1", "1", 3);
WiFiManagerParameter custom_pin2("Pin2", "Pin2", "2", 3);
WiFiManagerParameter custom_pin3("Pin3", "Pin3", "3", 3);
WiFiManagerParameter custom_pin4("Pin4", "Pin4", "4", 3);
WiFiManagerParameter custom_pin5("Pin5", "Pin5", "5", 3);
WiFiManagerParameter custom_pin6("Pin6", "Pin6", "6", 3);
WiFiManagerParameter custom_pin7("Pin7", "Pin7", "7", 3);
WiFiManagerParameter custom_pin8("Pin8", "Pin8", "8", 3);
WiFiManagerParameter custom_pin9("Pin9", "Pin9", "9", 3);
WiFiManagerParameter custom_pin10("Pin10", "Pin10", "10", 3);
WiFiManagerParameter custom_pin11("Pin11", "Pin11", "11", 3);
WiFiManagerParameter custom_pin12("Pin12", "Pin12", "12", 3);
WiFiManagerParameter custom_pin13("Pin13", "Pin13", "13", 3);

std::vector<String> pins;

const char * filepath = "/p_conf.json";
const char * wiFiPassword = "tock1234";
void checkConfigButton();
void saveParamsCallback();
void configurePins(ArduinoJson::JsonObject fileJsonObj);

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
    String filedata = readFile(LittleFS, filepath);
    DynamicJsonDocument fileJson(512);
    JsonObject fileJsonObj;
    if (filedata != String()){
        deserializeJson(fileJson, filedata);
        fileJsonObj = fileJson.as<JsonObject>();
        serializeJsonPretty(fileJson,Serial);
        
        custom_io.setValue(fileJsonObj["io"].as<String>().c_str(),15); 
        custom_server.setValue(fileJsonObj["server"].as<String>().c_str(),15); 
        custom_pin1.setValue(fileJsonObj["Pin1"].as<String>().c_str(),3); 
        custom_pin2.setValue(fileJsonObj["Pin2"].as<String>().c_str(),3);
        custom_pin3.setValue(fileJsonObj["Pin3"].as<String>().c_str(),3);        
        custom_pin4.setValue(fileJsonObj["Pin4"].as<String>().c_str(),3);
        custom_pin5.setValue(fileJsonObj["Pin5"].as<String>().c_str(),3);
        custom_pin6.setValue(fileJsonObj["Pin6"].as<String>().c_str(),3);
        custom_pin7.setValue(fileJsonObj["Pin7"].as<String>().c_str(),3);
        custom_pin8.setValue(fileJsonObj["Pin8"].as<String>().c_str(),3);
        custom_pin9.setValue(fileJsonObj["Pin9"].as<String>().c_str(),3);
        custom_pin10.setValue(fileJsonObj["Pin10"].as<String>().c_str(),3);
        custom_pin11.setValue(fileJsonObj["Pin11"].as<String>().c_str(),3);
        custom_pin12.setValue(fileJsonObj["Pin12"].as<String>().c_str(),3);
        custom_pin13.setValue(fileJsonObj["Pin13"].as<String>().c_str(),3);

        configurePins(fileJsonObj);

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
    wm.addParameter(&custom_pin9);
    wm.addParameter(&custom_pin10);
    wm.addParameter(&custom_pin11);
    wm.addParameter(&custom_pin12);
    wm.addParameter(&custom_pin13);
    
    wm.setSaveParamsCallback(saveParamsCallback);
    wm.setConnectTimeout(30);
    String thingName = esp8266ID();
    String clientId = "TOCK-" + thingName;
    if(wm.autoConnect(clientId.c_str(),wiFiPassword)){
        Serial.println("connected...yeey :)");
        flipper.attach(0.2, flip);
    }
    else {
        Serial.println("Configportal running");
    }

}

void loopWM() {
    wm.process();
    checkConfigButton();
}

void saveParamsCallback () {
    DynamicJsonDocument jsonToFile(512);
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
    jsonToFile["Pin9"] = custom_pin9.getValue();
    jsonToFile["Pin10"] = custom_pin10.getValue();
    jsonToFile["Pin11"] = custom_pin11.getValue();
    jsonToFile["Pin12"] = custom_pin12.getValue();
    jsonToFile["Pin13"] = custom_pin13.getValue();
    writeFile(LittleFS, filepath, jsonToFile.as<String>().c_str());

   
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


void configurePins(ArduinoJson::JsonObject fileJsonObj){
        
        pins.push_back(fileJsonObj["Pin1"].as<String>());
        pins.push_back(fileJsonObj["Pin2"].as<String>());
        pins.push_back(fileJsonObj["Pin3"].as<String>());
        pins.push_back(fileJsonObj["Pin4"].as<String>());
        pins.push_back(fileJsonObj["Pin5"].as<String>());
        pins.push_back(fileJsonObj["Pin6"].as<String>());
        pins.push_back(fileJsonObj["Pin7"].as<String>());
        pins.push_back(fileJsonObj["Pin3"].as<String>());
        pins.push_back(fileJsonObj["Pin9"].as<String>());
        pins.push_back(fileJsonObj["Pin10"].as<String>());
        pins.push_back(fileJsonObj["Pin11"].as<String>());
        pins.push_back(fileJsonObj["Pin12"].as<String>());
        pins.push_back(fileJsonObj["Pin3"].as<String>());

        for(int i=0;i<pins.size();i++){
            if(fileJsonObj["io"].as<String>()=="i"){
                pinMode(pins[i].toInt(), INPUT);
            }
            else {
                pinMode(pins[i].toInt(), OUTPUT);
            } 
        }

}