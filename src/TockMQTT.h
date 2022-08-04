#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *mqtt_server = "192.168.0.32";
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
}

boolean reconnect()
{
    String thingName = esp8266ID();
    String clientId = "TOCK-" + thingName + "-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), broker_user, broker_pass))
    {
        Serial.println(thingName + " connected!");
        flipper.attach(1, flip); // pisca a cada segundo

        // Once connected, publish an announcement...
        client.publish(pub_topic, "connected");
        // ... and resubscribe
        client.subscribe(sub_topic);
    }
    else
    {
        Serial.print("failed, rc=");
        Serial.print(client.state());
    }

    return client.connected();
}

void setupMQTT()
{
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    delay(1500);
    lastReconnectAttempt = 0;
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
