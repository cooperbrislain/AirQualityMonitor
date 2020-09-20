#include "AirQualityMonitor.h"

#define     FIRE_PIN    12
#define     SENSOR_PIN  14

int         count = 0;

void setup() {
    Serial.begin(115200);

    pinMode(FIRE_PIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);
    Serial.println("Starting Air Quality Monitor...");
}

#define NUM_READINGS 5

void loop() {
    int readings[NUM_READINGS];
    digitalWrite(FIRE_PIN, HIGH);
    for (int i=0; i<NUM_READINGS; i++) {
        int val = analogRead(SENSOR_PIN);
        readings[i] = val;
        Serial.println(val);
        delay(5);
    }
    digitalWrite(FIRE_PIN, LOW);
    delay(500);
}