
#include <Arduino.h>
#include <FS.h>          //this needs to be first, or it all crashes and burns...
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <LittleFS.h>
#include "utils.h"

#define TRIGGER_PORTAL D2

char mqtt_server[40];
char mqtt_port[6] = "8080";
char api_token[34] = "YOUR_APITOKEN";

WiFiManager wifiManager;


void saveParamsCallback(){

}
void checkConfigButton();
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    
    // Sometimes when stronger things happens, its needs format to clean memory
    // SPIFFS.format();
    // LittleFS.format();

    DynamicJsonDocument doc(256);
    doc["mqtt_server"] = "192.168.0.5";
    doc["mqtt_port"] = "8080";
    doc["api_token"] = "token";

    // Open file saved tp retrieve json
    if (!LittleFS.begin()){
        Serial.println("LittleFS mount failed");return;
    }
    String filedata = readFile(LittleFS, "/config.json");
    DynamicJsonDocument fileJson(256);
    JsonObject fileJsonObj;
    if (filedata != String()){
        deserializeJson(fileJson, filedata);
        fileJsonObj = fileJson.as<JsonObject>();
        serializeJsonPretty(fileJson,Serial);
    }

    // add params to server and update values from file
    WiFiManagerParameter *custom_param;
    JsonObject jsonDocObj = doc.as<JsonObject>();
    for (JsonPair json : jsonDocObj){
        auto jsonKey = json.key().c_str();
        auto jsonValue = json.value().as<String>();
        auto id = String(jsonKey) + String(random(0xffff), HEX);
      
        custom_param = new WiFiManagerParameter(id.c_str(), jsonKey, jsonValue.c_str(), 15);
        wifiManager.addParameter(custom_param);

        // update param with value from memory (fileJsonObj)
        if (fileJsonObj.containsKey(jsonKey)){
            Serial.print("fileJsonObj[jsonKey]: ");Serial.print(fileJsonObj[jsonKey].as<String>());
            custom_param->setValue(fileJsonObj[jsonKey], 15);
        }
    }

    // READ JSON FROM FILE
    // if (!LittleFS.begin())
    // {
    //     Serial.println("LittleFS mount failed");
    //     return;
    // }
    // String filedata = readFile(LittleFS, "/config.json");
    // DynamicJsonDocument paramsJsonInput(1024);
    // Serial.println(filedata);
    // if (filedata != String())
    // {
    //     deserializeJson(paramsJsonInput, filedata);
    //     JsonObject json = paramsJsonInput.as<JsonObject>();
    //     strcpy(mqtt_server, json["mqtt_server"]);
    //     strcpy(mqtt_port, json["mqtt_port"]);
    //     strcpy(api_token, json["api_token"]);
    // }

    // WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    // WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 5);
    // WiFiManagerParameter custom_api_token("apikey", "API token", api_token, 34);
    // wifiManager.addParameter(&custom_mqtt_server);
    // wifiManager.addParameter(&custom_mqtt_port);
    // wifiManager.addParameter(&custom_api_token);

    wifiManager.setSaveConfigCallback(saveParamsCallback);
    wifiManager.setMinimumSignalQuality();
    if (!wifiManager.autoConnect("AutoConnectAP", "password"))
    {
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        // reset and try again, or maybe put it to deep sleep
        ESP.restart();
        delay(5000);
    }
    Serial.print("connected with local ip: ");
    Serial.println(WiFi.localIP());

    // // Salving Configs
    // strcpy(mqtt_server, custom_mqtt_server.getValue());
    // strcpy(mqtt_port, custom_mqtt_port.getValue());
    // strcpy(api_token, custom_api_token.getValue());

    // // const size_t CAPACITY = JSON_ARRAY_SIZE(3);
    // // StaticJsonDocument<CAPACITY> doc;
    // // JsonArray array = doc.to<JsonArray>();

    // // DynamicJsonDocument docJson(1024);
    // // JsonObject json = docJson.as<JsonObject>();
    // // json["mqtt_server"] = mqtt_server;
    // // array.add(json);

    // // JsonObject json2 = docJson.as<JsonObject>();
    // // json["mqtt_port"] = mqtt_port;
    // // array.add(json2);
    // // writeFile(LittleFS, "/config.json", array.as<String>().c_str());
    // // Serial.println(doc.as<String>().c_str());
    // DynamicJsonDocument paramsJsonOutput(256);
    // paramsJsonOutput["mqtt_server"] = "server";
    // paramsJsonOutput["mqtt_port"] = "port";
    // paramsJsonOutput["api_token"] = "token";
    // // writeFile(LittleFS, "/config.json", paramsJsonOutput.as<String>().c_str());
    // Serial.print("write files: ");
    // Serial.println(res);

    // free(custom_param);
    // doc.clear();
    // fileJson.clear();

    DynamicJsonDocument jsonToFile(256);
    WiFiManagerParameter ** params = wifiManager.getParameters();
    
    
    Serial.println("wifiManager.getParametersCount()") ;
    Serial.println(wifiManager.getParametersCount());
    for (int i=0; i <wifiManager.getParametersCount() ;i++ ){
        jsonToFile[params[i]->getLabel()] =  params[i]->getValue();
        Serial.print("Label: ");Serial.println(params[i]->getLabel());
        Serial.print("Value: ");Serial.println( params[i]->getValue());
    }
    writeFile(LittleFS, "/config.json", jsonToFile.as<String>().c_str());

    // Serial.println(writeFile(LittleFS, "/config.json", jsonToFile.as<String>().c_str()));
    // Serial.println(paramsJsonOutput.as<String>().c_str());
    // free(params);   
    // jsonToFile.clear();
}

void loop()
{
    wifiManager.process();
    checkConfigButton();
}

void checkConfigButton()
{
    if (digitalRead(TRIGGER_PORTAL))
    {
        delay(50);
        if (digitalRead(TRIGGER_PORTAL))
        {
            // flipper.detach(); // pisca lento se conectado ao wifi
            digitalWrite(LED_BUILTIN, 1);
            wifiManager.resetSettings(); // reset settings?
            //  wm.setConfigPortalBlocking(false);
            //  wm.startConfigPortal();
            wifiManager.startWebPortal();
            delay(1000);
            ESP.reset();
        }
    }
}
