#ifndef AIRQUALITYMONITOR_H
#define AIRQUALITYMONITOR_H

#include <Arduino.h>
#include <driver/adc.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <iostream>

#include "config.h"

//#include "StreamPrint.h"

void mqtt_callback(char* topic, byte* message, unsigned int length);
void reconnect();

#endif // AIRQUALITYMONITOR_H