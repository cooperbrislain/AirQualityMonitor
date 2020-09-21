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

void setup_wifi();

#endif // AIRQUALITYMONITOR_H