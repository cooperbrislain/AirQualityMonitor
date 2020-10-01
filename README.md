# AIR QUALITY MONITOR

A really quick and dirty IoT implementation of an air quality 
monitor using an ESP32 and a Sharp Dust Sensor module. This 
is by no means accurate, and I haven't managed to come even 
close to calibrating it. 

## Features

* JSON over MQTT
* SPIFFS Config

## Issues

* This ReadMe is not complete.
* Still having a hard time getting consistent and accurate 
    readings from the 5v sensor module using the ESP32's 
    3.3v ADC
    
## References

* [WaveShare Dust Sensor Module Manual](https://www.waveshare.com/w/upload/0/0a/Dust-Sensor-User-Manual-EN.pdf)
* [ESP32 ADC Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)
* [ESP32 ADC Range and Linearity](https://www.researchgate.net/figure/ADC-linearity-test-at-12-bit-resolution_fig4_320273388)