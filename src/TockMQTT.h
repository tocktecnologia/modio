#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// const char *mqtt_server = "192.168.0.5"; // "192.168.0.5";
const char *pub_topic = "tock-commands";
const char *sub_topic = "tock-commands";
const char *broker_user = "tocktec.com.br";
const char *broker_pass = "tock30130tecnologia";

// WiFiUDP Udp;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
long lastReconnectAttempt = 0;


bool pinIsConfigured(int pinIdReceivedMqtt){
    for(unsigned int i=0;i<wifiParamsPins.size();i++){
        int pinIdConfigured = String(wifiParamsPins[i].getID()).toInt();
        if(pinIdReceivedMqtt==pinIdConfigured) return true;
    }
    return false;
}

void callback(char *topic, byte *payload, unsigned int length)
{               
    // Reading message 
    // Serial.print("Message arrived [");
    // Serial.print(topic);
    // Serial.print("] ");
    String payloadString = "";
    for (int i = 0; i < length; i++)
    {
        // Serial.print((char)payload[i]);
        payloadString = payloadString + (char)payload[i];
    }

    // build json desired with message arrived
    if(String(custom_io.getValue()) != "o") return;
    StaticJsonDocument<256> fileJson;
    JsonObject fileJsonObj;
    deserializeJson(fileJson, payloadString);
    
    if(!fileJson.as<JsonObject>()["state"].containsKey("desired")) {return;}

    Serial.println("Message desired arrived: " + payloadString);

    fileJsonObj = fileJson.as<JsonObject>()["state"]["desired"];
    // serializeJsonPretty(fileJson,Serial); //debug   
    fileJson.clear();   
    
    // iterate over the messsage desired 
    for (JsonPair jsonPair : fileJsonObj) {
        int pinId = String(jsonPair.key().c_str()).substring(3).toInt(); // jsonPair.key = "pin3"
       
        // checking if pin is confiured 
        Serial.println("pinId: " + String(pinId));
        if(!pinIsConfigured(pinId)) return;

        String state = jsonPair.value();
        if(pinId>0){
            int pinOut = String(wifiParamsPins[pinId-1].getID()).toInt();
            if(state == "x" || state == "X"){
                state = (String)!digitalRead(pinOut);
            }
            digitalWrite(pinOut,state.toInt()); 
        }
    }
    fileJsonObj.clear();   
    
    // report pins states
    String reportedMessage =  String("{\"state\": {\"reported\": {") +
        "\"pin"  + String(custom_pin1.getValue())+"\": " + String(digitalRead(atoi(custom_pin1.getID()))) +
        ",\"pin" + String(custom_pin2.getValue())+"\": " + String(digitalRead(atoi(custom_pin2.getID()))) + 
        ",\"pin" + String(custom_pin3.getValue())+"\": " + String(digitalRead(atoi(custom_pin3.getID()))) + 
        ",\"pin" + String(custom_pin4.getValue())+"\": " + String(digitalRead(atoi(custom_pin4.getID()))) + 
        ",\"pin" + String(custom_pin5.getValue())+"\": " + String(digitalRead(atoi(custom_pin5.getID()))) + 
        ",\"pin" + String(custom_pin6.getValue())+"\": " + String(digitalRead(atoi(custom_pin6.getID()))) + 
        ",\"pin" + String(custom_pin7.getValue())+"\": " + String(digitalRead(atoi(custom_pin7.getID()))) + 
        ",\"pin" + String(custom_pin8.getValue())+"\": " + String(digitalRead(atoi(custom_pin8.getID()))) + 
        "}}}";
    
    Serial.println("reporting states: " + reportedMessage);
    client.publish(pub_topic, reportedMessage.c_str());
    writeFile(LittleFS, filepathStates, reportedMessage.c_str());


    
}

boolean reconnect(){
    String thingName = esp8266ID();
    String clientId = "TOCK-" + thingName + "-";
    clientId += String(random(0xffff), HEX);

    Serial.print("Try connecting to ");
    Serial.print(custom_server.getValue());
    Serial.println(" ...");
    flipper.attach(0.2, flip); // pisca lento se conectado ao wifi

    if (client.connect(clientId.c_str(), broker_user, broker_pass))
    {
        Serial.println(clientId + " connected to " + String(custom_server.getValue()) + "!");
        client.publish(pub_topic, "connected");
        client.subscribe(sub_topic);
        flipper.attach(1, flip);
       
    }else {
        // check if wifi is disconnected
        if (WiFi.status() != WL_CONNECTED){
            digitalWrite(LED_BUILTIN, 1);
            wm.resetSettings(); // reset settings?
            //  wm.setConfigPortalBlocking(false);
            //  wm.startConfigPortal();
            wm.startWebPortal();
            delay(1000);
            ESP.reset();
        }
    }

    return client.connected();
}

void setupMQTT()
{
    client.setServer(custom_server.getValue(), 1883);
    client.setCallback(callback);
    lastReconnectAttempt = 0;

    // for(int i=0; i<pins.size();i++){
    //     Serial.print(String("i[")+ String(i) + "]: ");Serial.println(pins.at(i));
    // }

}

void loopMQTT()
{

    if (!client.connected())
    {
        long now = millis();
        if (now - lastReconnectAttempt > 5000)
        {
            lastReconnectAttempt = now;
            // Attempt to reconnect
            if (reconnect())
            {
                lastReconnectAttempt = 0;
            }
        }
    }
    else
    {
        // Client connected
        client.loop();
    }
}

void sendResponse(int index_state, String command)
{
    // delay(1);
    // String pinSaida2;

    // // Troca pinoXXX da mensagem Serial pelo pino da saída correta.
    // DynamicJsonBuffer jsonBuffer2;
    // JsonObject &jsPinos = jsonBuffer2.parseObject((jsonPinos));
    // String comando = "pin" + String(command[index_state]) + String(command[index_state + 1]);

    // for (JsonPair keyState : jsPinos)
    // {
    //     String key = keyState.key;
    //     if (key == comando)
    //     {
    //         String pinSaida = jsPinos[key];
    //         pinSaida2 = pinSaida;
    //     }
    // } // for

    // String msg = "{\"" + pinSaida2 + "\": \"x\"}";

    // String desiredState = "{\"state\": {\"desired\": " + msg + "}}";
    // char desiredState_charBuf[100];
    // desiredState.toCharArray(desiredState_charBuf, 100);
    // client.publish(pub_topic, desiredState_charBuf);

    // Serial.println("Mensagem publicada : " + desiredState);
}
