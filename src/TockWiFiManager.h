#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#define TRIGGER_PORTAL D2

WiFiManager wm;
WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "192.168.0.32", 40);
WiFiManagerParameter custom_pub_topic("pub_topic", "pub topic", "tock-commands", 40);
WiFiManagerParameter custom_sub_topic("sub_topic", "sub topic", "tock-commands", 40);

// funcions declarations
void saveParamsCallback();
void checkConfigButton();

void setupWM()
{
    flipper.attach(0.05, flip); // pisca rapido enquanto tenta se conectar ao wifi
    WiFi.mode(WIFI_STA);        // explicitly set mode, esp defaults to STA+AP
    pinMode(TRIGGER_PORTAL, OUTPUT);

        // reset settings - wipe credentials for testing
    // wm.resetSettings();
    wm.setDebugOutput(false);
    wm.addParameter(&custom_mqtt_server);
    wm.addParameter(&custom_pub_topic);
    wm.addParameter(&custom_sub_topic);

    wm.setConfigPortalBlocking(false);
    wm.setSaveParamsCallback(saveParamsCallback);

    // automatically connect using saved credentials if they exist
    // If connection fails it starts an access point with the specified name
    String ssid = String("TOCK-") + esp8266ID();
    if (wm.autoConnect(ssid.c_str(), "tock1234"))
    {
        Serial.println("connected...yeey :)");
        flipper.attach(0.3, flip); // pisca rapido enquanto tenta se conectar ao wifi
        saveParamsCallback();
    }
    else
    {
        Serial.println("Configportal running");
        flipper.attach(0.05, flip); // pisca rapido enquanto tenta se conectar ao wifi
    }
}

void loopWM()
{
    wm.process();
    // put your main code here, to run repeatedly:

    checkConfigButton();
}

void checkConfigButton()
{
    if (digitalRead(TRIGGER_PORTAL))
    {
        delay(1000);
        if (digitalRead(TRIGGER_PORTAL))
        {
            flipper.detach(); // pisca lento se conectado ao wifi
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

void saveParamsCallback()
{
    Serial.println("Get Params:");
    Serial.println(String(custom_mqtt_server.getID()) + String(":") + custom_mqtt_server.getValue());
    Serial.println(String(custom_pub_topic.getID()) + String(":") + custom_pub_topic.getValue());
    Serial.println(String(custom_sub_topic.getID()) + String(":") + custom_sub_topic.getValue());
}