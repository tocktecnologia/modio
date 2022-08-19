#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *mqtt_server = "192.168.0.5"; // "192.168.0.5";
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

        for (JsonPair jp : fileJsonObj) {
            int pinId = String(jp.key().c_str()).substring(3).toInt();
            int state = jp.value();
            if(pinId>0){     
                int pinOut = String(wifiParamsPins[pinId-1].getID()).toInt();
                digitalWrite(pinOut,state);
            }
        }

    } else Serial.print("pyload should be json");


}

boolean reconnect()
{
    String thingName = esp8266ID();
    String clientId = "TOCK-" + thingName + "-";
    clientId += String(random(0xffff), HEX);

    Serial.print("Try connecting to ");
    Serial.print(mqtt_server);
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
    client.setServer(mqtt_server, 1883);
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
