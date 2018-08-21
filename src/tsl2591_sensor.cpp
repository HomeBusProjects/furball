#include "tsl2591_sensor.h"

#if 0
/**************************************************************************/
/*
  Displays some basic information on this sensor from the unified
  sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution, 4); Serial.println(F(" lux"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}
#endif


void TSL2591_Sensor::begin() {
  if (!_tsl2591.begin()) {
    _status = SENSOR_NOT_FOUND;
    Serial.println("TSL2591 not found");
    return;
  }
  
  Serial.println("TSL2591 okay");

  _status = SENSOR_OKAY;

  _tsl2591.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  //  _tsl2591.setGain(TSL2591_GAIN_MED);      // 25x gain
  // _tsl2591.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  _tsl2591.setTiming(TSL2591_INTEGRATIONTIME_300MS);
}

void TSL2591_Sensor::handle() {
  uint32_t lum = _tsl2591.getFullLuminosity();

  _ir = lum >> 16;
  _full = lum & 0xFFFF;
  _visible = _full - _ir;
  _lux = _tsl2591.calculateLux(_full, _ir);
}
