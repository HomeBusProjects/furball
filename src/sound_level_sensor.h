#pragma once

#include "sensor.h"

class SoundLevel_Sensor : public Sensor {
 public:
  SoundLevel_Sensor(uint8_t pin, uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : Sensor(update_frequency, accuracy, precision, calibrated), _pin(pin) {};

  void begin() { pinMode(_pin, INPUT); }
  void handle() { sample_value(); };

  uint32_t sound_level() { _mark_read(); return analogRead(_pin); };

  void sample_take() { _sample_average = ((_sample_average * _sample_count) + analogRead(_pin))/(_sample_count + 1); _sample_count++; };
  uint32_t sample_value() { _mark_read(); return _sample_average; }
  void start_sampling() { _sample_count = 0; sample_take(); };
  uint32_t sample_count() { return _sample_count; };
  
 private:
  uint8_t _pin = 0;

  uint32_t _sample_start_time = 0;
  uint32_t _sample_average = 0;
  uint32_t _sample_count = 0;
};
