#pragma once

#include <Arduino.h>

class Sensor {
 public:
 Sensor(uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : _update_frequency(update_frequency), _accuracy(accuracy), _precision(precision), _calibrated(calibrated) { };

  virtual void begin();
  virtual void handle();

  enum sensor_status {
    SENSOR_OKAY = 0,
    SENSOR_NOT_FOUND = 1,
    SENSOR_NOT_READY = 2,
    SENSOR_BURN_IN = 3,
    SENSOR_RUN_IN = 4,
    SENSOR_INVALID_VALUE = 5,
    SENSOR_FAULT = 6
  };

  virtual enum sensor_status status() { return _status; };

  virtual const char* status_str() {
    switch(_status) {
    case SENSOR_OKAY:
      return "Sensor okay";
    case SENSOR_NOT_FOUND:
      return "Sensor not found";
    case SENSOR_NOT_READY:
      return "Sensor not ready";
    case SENSOR_BURN_IN:
      return "Sensor still in burn-in period";
    case SENSOR_RUN_IN:
      return "Sensor still in run-in period";
    case SENSOR_INVALID_VALUE:
      return "Sensor returned an invalid reading";
    case SENSOR_FAULT:
      return "Sensor did not operate correctly";
    default:
      return "Unknown status value";
    }
  };

  virtual boolean ready_for_update() { if(_status == SENSOR_NOT_FOUND) return false; if( (millis() - _last_checked) >= _update_frequency) return true; else return false; }

 protected:
  void _mark_read() { _last_checked = millis(); };

  enum sensor_status _status;

  uint16_t _update_frequency = 0;
  
  uint16_t _burn_in_time = 0;
  uint16_t _run_in_time = 0;

  uint16_t _accuracy = 0;
  uint16_t _precision = 0;
  boolean _calibrated = false;

  uint32_t _last_checked = 0;
};
