#include "AirQualityMonitor.h"

WiFiClient      espClient;
PubSubClient    client(espClient);
Config          config;

// defined in secret.h
const char* wifi_ssid = WIFI_SSID;
const char* wifi_pass = WIFI_PASS;
const char* mqtt_host = MQTT_HOST;

void setup() {
    Serial.begin(115200);

    Serial.println("Starting up ESP32 Air Quality Monitor");

    Serial << "Loading configuration...\n";
    SPIFFS.begin(true);
    StaticJsonDocument<4096> jsonDoc;
    File configFile = SPIFFS.open("/config.json", FILE_READ);
    deserializeJson(jsonDoc, configFile);
    configFile.close();

    JsonObject obj      = jsonDoc.as<JsonObject>();

    config.name         = jsonDoc["name"].as<String>() || "Unnamed Project";
    config.wifi_ssid    = jsonDoc["wifi_ssid"].as<String>();
    config.wifi_pass    = jsonDoc["wifi_pass"].as<String>();

    pinMode(LED_PIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_GPIO32_CHANNEL, ADC_ATTEN_MAX);

    Serial.print("Connecting to ");
    Serial.println(wifi_ssid);

    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected with IP: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqtt_host, 1883);
    client.setCallback(mqtt_callback);

    Serial.println("Starting Air Quality Monitor...");
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    int readings[NUM_READINGS];
    int total = 0;
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    for (int i=0; i<NUM_READINGS; i++) {
        int val = adc1_get_raw(ADC1_GPIO32_CHANNEL);
        readings[i] = val;
        total += val;
        // 2500 = 3.3v?
//        double aqi = 0.18 * (val / 4095) - 0.12;
        Serial.print(val);
        Serial.print(" ");
        delay(100);
    }
    double avg = total/NUM_READINGS;
    char aqiString[5];
    String jsonMessage;
    double voltage = (double)avg * (5.0 / 4095.0);
    double density = 0.25 * voltage - 0.1;
    Serial.print("Average: ");
    Serial.println(avg);
    Serial.print("Voltage: ");
    Serial.println(voltage);
    Serial.print("Density: ");
    Serial.println(density);
    dtostrf(avg, 1, 2, aqiString);
    StaticJsonDocument<4096> jsonDoc;
    JsonObject jsonOb = jsonDoc.as<JsonObject>();
    jsonOb["aqi"] = aqiString;
    jsonOb["pm2.5"] = density;
    jsonOb["voltage"] = voltage;
    JsonArray& data = jsonOb.createNestedArray("data");
    data.copyFrom(readings);
    serializeJson(jsonDoc, jsonMessage);
    client.publish("/esp32/aqi", jsonMessage);
    digitalWrite(LED_PIN, LOW);
    delay(5000);
}

void mqtt_callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    if (String(topic) == "/esp32/read") {
        Serial.println("Reading... ");
        // do stuff;
    }
    if (String(topic) == "/esp32") {

    }
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client")) {
            Serial.println("connected");
            client.subscribe("/esp32/read");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}