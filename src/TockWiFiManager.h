#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define TRIGGER_PIN D3
unsigned int timeout = 120; // seconds to run for
unsigned int startTime = millis();
bool portalRunning = false;
bool startAP = false; // start AP and webserver if true, else start only webserver

WiFiManager wm;
WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "192.168.0.32", 40);
WiFiManagerParameter custom_pub_topic("server", "pub topic", "tock-commands", 40);
WiFiManagerParameter custom_sub_topic("server", "sub topic", "tock-commands", 40);

// funcions declarations
void saveParamsCallback();
void doWiFiManager();

void setupWM()
{
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    pinMode(TRIGGER_PIN, INPUT_PULLUP);

    // reset settings - wipe credentials for testing
    // wm.resetSettings();
    wm.addParameter(&custom_mqtt_server);
    wm.addParameter(&custom_pub_topic);
    wm.addParameter(&custom_sub_topic);

    wm.setConfigPortalBlocking(false);
    wm.setSaveParamsCallback(saveParamsCallback);

    // automatically connect using saved credentials if they exist
    // If connection fails it starts an access point with the specified name
    if (wm.autoConnect("AutoConnectAP", "pass"))
    {
        Serial.println("connected...yeey :)");
        flipper.attach(1, flip); // pisca lento se conectado ao wifi
    }
    else
    {
        Serial.println("Configportal running");
    }
}

void loopWM()
{
    wm.process();
    // put your main code here, to run repeatedly:
    doWiFiManager();
    Serial.println(digitalRead(TRIGGER_PIN));
}

void saveParamsCallback()
{
    Serial.println("Get Params:");
    Serial.print(String(custom_mqtt_server.getID()) + String(":") + custom_mqtt_server.getValue());
    Serial.print(String(custom_pub_topic.getID()) + String(":") + custom_pub_topic.getValue());
    Serial.print(String(custom_sub_topic.getID()) + String(":") + custom_sub_topic.getValue());
}

void doWiFiManager()
{
    // is auto timeout portal running
    if (portalRunning)
    {
        wm.process(); // do processing

        // check for timeout
        if ((millis() - startTime) > (timeout * 1000))
        {
            Serial.println("portaltimeout");
            portalRunning = false;
            if (startAP)
            {
                wm.stopConfigPortal();
            }
            else
            {
                wm.stopWebPortal();
            }
        }
    }

    // is configuration portal requested?
    if (digitalRead(TRIGGER_PIN) == LOW && (!portalRunning))
    {
        if (startAP)
        {
            Serial.println("Button Pressed, Starting Config Portal");
            wm.setConfigPortalBlocking(false);
            wm.startConfigPortal();
        }
        else
        {
            Serial.println("Button Pressed, Starting Web Portal");
            wm.startWebPortal();
        }
        portalRunning = true;
        startTime = millis();
    }
}