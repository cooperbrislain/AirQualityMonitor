#include "AirQualityMonitor.h"

#define     FIRE_PIN    33
#define     SENSOR_PIN  32

int         count = 0;

void setup() {
    Serial.begin(115200);

    pinMode(FIRE_PIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_GPIO32_CHANNEL, ADC_ATTEN_0db);
    Serial.println("Starting Air Quality Monitor...");
}

#define NUM_READINGS 10

void loop() {
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
    digitalWrite(FIRE_PIN, LOW);
    delay(500);
}