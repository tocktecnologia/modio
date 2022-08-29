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

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String payloadString = "";
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
        payloadString = payloadString + (char)payload[i];
    }
    Serial.println();

     // if is a output module
    if(String(custom_io.getValue()) == "o"){
        StaticJsonDocument<256> fileJson;
        JsonObject fileJsonObj;
        //debug
        deserializeJson(fileJson, payloadString);
        fileJsonObj = fileJson.as<JsonObject>()["state"]["desired"];
        //debug
        serializeJsonPretty(fileJson,Serial);

        if(!fileJson.as<JsonObject>()["state"].containsKey("desired")) return;


        // StaticJsonDocument<256> jsonToFile;
        for (JsonPair jp : fileJsonObj) {
            int pinId = String(jp.key().c_str()).substring(3).toInt();
            String state = jp.value();
            if(pinId>0){
                int pinOut = String(wifiParamsPins[pinId-1].getID()).toInt();
                if(state == "x" || state == "X"){
                    state = (String)!digitalRead(pinOut);
                }
                Serial.print("digitalWrite -> pinOut: ");
                Serial.print(pinOut);
                Serial.print(" state: ");
                Serial.println(state.toInt());
                
                digitalWrite(pinOut,state.toInt());
                
                switch (pinId)
                {
                case 1:custom_pin1.setLabelPlacement(state.toInt());break;
                case 2:custom_pin2.setLabelPlacement(state.toInt());break;
                case 3:custom_pin3.setLabelPlacement(state.toInt());break;
                case 4:custom_pin4.setLabelPlacement(state.toInt());break;
                case 5:custom_pin5.setLabelPlacement(state.toInt());break;
                case 6:custom_pin6.setLabelPlacement(state.toInt());break;
                case 7:custom_pin7.setLabelPlacement(state.toInt());break;
                case 8:custom_pin8.setLabelPlacement(state.toInt());break;
                default:
                    break;
                }
                // jsonToFile["server"] = custom_server.getValue();
                // jsonToFile["Pin1"] = custom_pin1.getValue();
                // jsonToFile["Pin2"] = custom_pin2.getValue();
                // jsonToFile["Pin3"] = custom_pin3.getValue();
                // jsonToFile["Pin4"] = custom_pin4.getValue();
                // jsonToFile["Pin5"] = custom_pin5.getValue();
                // jsonToFile["Pin6"] = custom_pin6.getValue();
                // jsonToFile["Pin7"] = custom_pin7.getValue();
                // jsonToFile["Pin8"] = custom_pin8.getValue();
                // jsonToFile["Pin9"] = custom_pin9.getValue();
                             
            }
        }
        // writeFile(LittleFS, filepath, jsonToFile.as<String>().c_str());
        // jsonToFile.clear();  

        String reportedMessage = "{\"state\": {\"reported\": {"+
            String("\"pin") + String(custom_pin1.getValue())+"\": " + String(digitalRead(atoi(custom_pin1.getID()))) +
            ",\"pin" + String(custom_pin3.getValue())+"\": " + String(digitalRead(atoi(custom_pin3.getID()))) + 
            ",\"pin" + String(custom_pin2.getValue())+"\": " + String(digitalRead(atoi(custom_pin2.getID()))) + 
            ",\"pin" + String(custom_pin4.getValue())+"\": " + String(digitalRead(atoi(custom_pin4.getID()))) + 
            ",\"pin" + String(custom_pin5.getValue())+"\": " + String(digitalRead(atoi(custom_pin5.getID()))) + 
            ",\"pin" + String(custom_pin6.getValue())+"\": " + String(digitalRead(atoi(custom_pin6.getID()))) + 
            ",\"pin" + String(custom_pin7.getValue())+"\": " + String(digitalRead(atoi(custom_pin7.getID()))) + 
            ",\"pin" + String(custom_pin8.getValue())+"\": " + String(digitalRead(atoi(custom_pin8.getID()))) + 
            "}}}";
        
        client.publish(pub_topic, reportedMessage.c_str());

    } else Serial.print("pyload should be json");
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
        Serial.println(thingName + " connected!");
        client.publish(pub_topic, "connected");
        client.subscribe(sub_topic);
        flipper.attach(1, flip);
       
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
