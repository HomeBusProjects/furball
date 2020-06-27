#pragma once

#include <Arduino.h>

#include <Adafruit_ADS1015.h>

#include "sensor.h"

class ADS1115_Sensor : public Sensor {
 public:
  ADS1115_Sensor(uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : Sensor(update_frequency, accuracy, precision, calibrated) { _present = true; };

  void begin();
  void handle();

  uint16_t channel0() { return _channel0; };
  uint16_t channel1() { return _channel1; };
  uint16_t channel2() { return _channel2; };
  uint16_t channel3() { return _channel3; };

  void all_channels(uint16_t *channels) { channels[0] = _channel0; channels[1] = _channel1; channels[2] = _channel2; channels[3] = _channel3; };

 private:
  uint16_t _channel0;
  uint16_t _channel1;
  uint16_t _channel2;
  uint16_t _channel3;

  Adafruit_ADS1115 _ads1115;
};
