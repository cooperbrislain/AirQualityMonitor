#include "AirQualityMonitor.h"

WiFiClient      espClient;
PubSubClient    client(espClient);
Config          config;

DHTesp dht;

// defined in secret.h
const char* wifi_ssid = WIFI_SSID;
const char* wifi_pass = WIFI_PASS;
const char* mqtt_host = MQTT_HOST;

void setup() {
    Serial.begin(115200);

    Serial << "Starting up ESP32 Air Quality Monitor";

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
    pinMode(DHT_PIN, INPUT);

    dht.setup(DHT_PIN, DHTesp::DHT11);

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_GPIO32_CHANNEL, ADC_ATTEN_DB_0);

    Serial << "Connecting to " << wifi_ssid;

    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial << "WiFi connecdted with IP: " << WiFi.localIP() << "\n";

    client.setServer(mqtt_host, 1883);
    client.setCallback(mqtt_callback);

    Serial << "Starting Environmental Monitor...\n";
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
    digitalWrite(LED_PIN, LOW);

    for (int i=0; i<NUM_READINGS; i++) {
        int val = adc1_get_raw(ADC1_GPIO32_CHANNEL);
        val *= 11; // account for voltage splitter on module
        Serial << val << " ";
        readings[i] = val;
        total += val;
        delay(100);
    }

    float temp = dht.getTemperature();
    float hum = dht.getHumidity();
    String dht_status = dht.getStatusString();

    Serial << "DHT: T: " << temp << " H: " << hum << " S: " << dht_status << "\n";

    double avg = total/NUM_READINGS;
    char aqiString[5];
    char payload[1024];
    double voltage = (double)avg * (5.0 / 4095.0);
    double density = 0.25 * voltage - 0.1;

    if (density < 0.0) density = 0.0;
    density = density * 1000.0;
    Serial << density << "\n";
    int aqi = aqiFromPm2(density);
    Serial << "raw: " << avg << " | V: " << voltage << "v | D: " << density << "mg/m3 | AQI: " << aqi << "\n";
    dtostrf(aqi, 1, 2, aqiString);

    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["raw"] = avg;
    jsonDoc["aqi"] = aqiString;
    jsonDoc["pm2.5"] = density;
    jsonDoc["voltage"] = voltage;
    jsonDoc["temp"] = temp;
    jsonDoc["hum"] = hum;

//    JsonArray& data = jsonOb.createNestedArray("data");
//    data.copyFrom(readings);
    serializeJson(jsonDoc, payload);
    client.publish("/esp32/aqi", payload);
    digitalWrite(LED_PIN, LOW);
    delay(5000);
}

void mqtt_callback(char* topic, byte* message, unsigned int length) {
    String messageTemp;

    Serial << "Message arrived on topic, " << topic << ":\n";

    for (int i = 0; i < length; i++) {
        Serial << (char)message[i];
        messageTemp += (char)message[i];
    }

    Serial << "\n";

    if (String(topic) == "/esp32/read") {
        Serial << "Reading... ";
        // do stuff;
    }
    if (String(topic) == "/esp32") {

    }
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection to ");
        Serial.print(MQTT_HOST);
        Serial.println("...");
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

int aqiFromPm2(double pm25) {
    if (pm25 < 15) {
        return pm25 * 3.25;
    } else if (pm25 < 65) {
        return 50 + (pm25-15) * 2.00;
    } else if (pm25 < 150) {
        return 150 + (pm25-65) * 0.59;
    } else {
        return 200 + (pm25-200) * 1.00;
    }
}