#include "ccs811_sensor.h"

void CCS811_Sensor::begin() {
  if(_ccs811.begin() != CCS811Core::SENSOR_SUCCESS) {
    Serial.println("CCS begin() returned with an error.");
    _status = SENSOR_NOT_FOUND;
    return;
  }

  _status = SENSOR_OKAY;
}

void CCS811_Sensor::handle() {
  if(_ccs811.dataAvailable()) {
    _ccs811.readAlgorithmResults();

    _voc = _ccs811.getTVOC();
    _eco2 = _ccs811.getCO2();
    //    _temperature = _ccs811.calculateTemperature();
  }
}
