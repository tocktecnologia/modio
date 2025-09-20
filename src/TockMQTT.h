#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// const char *mqtt_server = "192.168.0.5"; // "192.168.0.5";
const char *pub_topic = "tock-commands";
const char *sub_topic = "tock-commands";
const char *broker_user = "tocktec.com.br";
const char *broker_pass = "tock30130tecnologia";
String sub_topic_reset_wifi = "tock-reset-wifi";

// WiFiUDP Udp;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
long lastReconnectAttempt = 0;

void callback(char *topic, byte *payload, unsigned int length)
{

    String payloadString = "";
    for (int i = 0; i < length; i++)
    {
        payloadString = payloadString + (char)payload[i];
    }

    // String topicStr = String(topic);
    // if (to'picStr.equals(sub_topic_reset_wifi))
    // {
    //     Serial.println("Reseting esp wifi settings ...");
    //     delay(2000);
    //     wm.resetSettings();
    //     ESP.restart();
    // }

    // build json desired with message arrived
    if (String(custom_io.getValue()) != "o")
        return;
    StaticJsonDocument<256> fileJson;
    JsonObject fileJsonObj;
    deserializeJson(fileJson, payloadString);

    if (!fileJson.as<JsonObject>()["state"].containsKey("desired"))
    {
        return;
    }

    Serial.println("Message desired arrived: " + payloadString);

    fileJsonObj = fileJson.as<JsonObject>()["state"]["desired"];
    // serializeJsonPretty(fileJson,Serial); //debug
    fileJson.clear();

    // iterate over the messsage desired
    bool canReport = false;
    for (JsonPair jsonPair : fileJsonObj)
    {
        int pinIdReceivedMqtt = String(jsonPair.key().c_str()).substring(3).toInt(); // jsonPair.key = "pin3"

        int pinIdOutput = getPinIdOutput(pinIdReceivedMqtt, wifiParamsPins);
        // if some pin received from mqtt is configured, update output and enable report states
        if (pinIdOutput > 0)
        {
            // update pin output
            String state = jsonPair.value();

            if (!(state.equals("0") || state.equals("1")))
            {
                state = (String)digitalRead(pinIdOutput);
            }

            pinMode(pinIdOutput, OUTPUT);
            digitalWrite(pinIdOutput, !state.toInt());
            canReport = true;
        }
    }
    fileJsonObj.clear();

    // if any one of the pins received from message mqtt is configured, not report state of all pins
    if (!canReport)
    {
        Serial.println("Pin not configured");
        return;
    }

    // report pins states
    String reportedMessage = String("{\"state\": {\"reported\": {") +
                             "\"pin" + String(custom_pin1.getValue()) + "\": " + String(!digitalRead(atoi(custom_pin1.getID()))) +
                             ",\"pin" + String(custom_pin2.getValue()) + "\": " + String(!digitalRead(atoi(custom_pin2.getID()))) +
                             ",\"pin" + String(custom_pin3.getValue()) + "\": " + String(!digitalRead(atoi(custom_pin3.getID()))) +
                             ",\"pin" + String(custom_pin4.getValue()) + "\": " + String(!digitalRead(atoi(custom_pin4.getID()))) +
                             ",\"pin" + String(custom_pin5.getValue()) + "\": " + String(!digitalRead(atoi(custom_pin5.getID()))) +
                             ",\"pin" + String(custom_pin6.getValue()) + "\": " + String(!digitalRead(atoi(custom_pin6.getID()))) +
                             ",\"pin" + String(custom_pin7.getValue()) + "\": " + String(!digitalRead(atoi(custom_pin7.getID()))) +
                             ",\"pin" + String(custom_pin8.getValue()) + "\": " + String(!digitalRead(atoi(custom_pin8.getID()))) +
                             "}}}";

    Serial.println("reporting states: " + reportedMessage);
    client.publish(pub_topic, reportedMessage.c_str());
    writeFile(LittleFS, filepathStates, reportedMessage.c_str());
}

boolean reconnect()
{
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
        client.subscribe(sub_topic_reset_wifi.c_str());
        flipper.attach(1, flip);
    }
    // else {
    //     // check if wifi is disconnected
    //     if (WiFi.status() != WL_CONNECTED){
    //         digitalWrite(LED_BUILTIN, 1);
    //         wm.resetSettings(); // reset settings?
    //         //  wm.setConfigPortalBlocking(false);
    //         //  wm.startConfigPortal();
    //         wm.startWebPortal();
    //         delay(1000);
    //         ESP.reset();
    //     }
    // }

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
