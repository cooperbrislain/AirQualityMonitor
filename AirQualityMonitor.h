#ifndef AIRQUALITYMONITOR_H
#define AIRQUALITYMONITOR_H

#include <Arduino.h>
#include <driver/adc.h>
#include <string>
#include <iostream>

#include <SPIFFS.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "config.h"

#include "StreamPrint.h"

struct Config {
    String name;
    String wifi_ssid;
    String wifi_pass;
};

void mqtt_callback(char* topic, byte* message, unsigned int length);
void reconnect();

#endif // AIRQUALITYMONITOR_H