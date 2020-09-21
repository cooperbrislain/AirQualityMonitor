#include "AirQualityMonitor.h"

#define     FIRE_PIN    33
#define     SENSOR_PIN  32

int         count = 0;

WiFiClient espClient;
PubSubClient client(espClient);

// create a secret.h populated with your own settings
const char* wifi_ssid = WIFI_SSID;
const char* wifi_pass = WIFI_PASS;
const char* mqtt_host = MQTT_HOST;

void setup() {
    Serial.begin(115200);

    Serial.println("Starting up ESP32 Air Quality Monitor");

    pinMode(FIRE_PIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_GPIO32_CHANNEL, ADC_ATTEN_0db);

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

#define NUM_READINGS 10

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    int readings[NUM_READINGS];
    int total = 0;
    digitalWrite(FIRE_PIN, HIGH);
    delay(10);
    for (int i=0; i<NUM_READINGS; i++) {
        int val = adc1_get_raw(ADC1_GPIO32_CHANNEL);
        readings[i] = val;
        total += val;
        // 2500 = 3.3v?
//        double aqi = 0.18 * (val / 4095) - 0.12;
        Serial.print(val);
        Serial.print(" ");
        delay(250);
    }
    int avg = total/NUM_READINGS;
    Serial.print("Average: ");
    Serial.println(avg);
    char aqiString[5];
    dtostrf(avg, 1, 2, aqiString);
    client.publish("/esp32/aqi", aqiString);
    digitalWrite(FIRE_PIN, LOW);
    delay(500);
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