#pragma once

#include "sensor.h"

class SoundLevel_Sensor : public Sensor {
 public:
  SoundLevel_Sensor(uint8_t pin, uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : Sensor(update_frequency, accuracy, precision, calibrated), _pin(pin) { };

  void begin();
  void handle();

  uint32_t sound_level() { _mark_read(); return _sample_average; };
  uint32_t sound_max() { _mark_read(); return _sample_max; };
  uint32_t sound_min() { _mark_read(); return _sample_min; };

  uint32_t sample_count() { return _sample_count; };

 private:
  uint8_t _pin;

  uint32_t _sample_average = 0;
  uint16_t _sample_min = 0xffff;
  uint16_t _sample_max = 0;
  uint32_t _sample_count = 0;
};
