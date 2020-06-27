#include "sensors/ads1115_sensor.h"

/*
 * the ADS1115 datasheet suggests the following configurations:
 *
 * 4 channel ADC, commonly used on furballs to hook up MQ series gas sensors
 *
 */
void ADS1115_Sensor::begin() {
  _ads1115.begin();

#if 0
  if(!_ads1115.begin()) {
    Serial.println("Could not find a valid ADS1115 sensor, check wiring!");
    _status = SENSOR_NOT_FOUND;
    return;
  }
#endif

  _present = true;
}

void ADS1115_Sensor::handle() {
  _channel0 = _ads1115.readADC_SingleEnded(0);
  _channel1 = _ads1115.readADC_SingleEnded(0);
  _channel2 = _ads1115.readADC_SingleEnded(0);
  _channel3 = _ads1115.readADC_SingleEnded(0);
}
