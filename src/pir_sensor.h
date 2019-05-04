#pragma once

#include "sensor.h"

class PIR_Sensor : public Sensor {
 public:
 PIR_Sensor(uint8_t pin, uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : Sensor(update_frequency, accuracy, precision, calibrated), _pin(pin) {};

  void begin() { pinMode(_pin, INPUT_PULLDOWN); }
  void handle() {};

  uint32_t presence() { _mark_read(); return digitalRead(_pin); }

 private:
  uint8_t _pin = 0;
};
