#include "hw.h"
#include "pms_sensor.h"

void PMS_Sensor::begin(HardwareSerial port) {
  //  Serial1.begin(9600, (uint32_t)SERIAL_8N1, 25, 26);

  //  _pms = new PMS(Serial1);

  // 16 - ESP32 xmt
  // 17 - ESP32 rcv
  port.begin(9600, (uint32_t)SERIAL_8N1, 17, 16);
  _pms = new PMS(port);

  _pms->passiveMode();
  _pms->wakeUp();

  Serial.println("PMS begin done");
}

void PMS_Sensor::begin() {
}

#define PMS_READ_DELAY 1000

void PMS_Sensor::handle() {
  PMS::DATA data;
  
  if(millis() - _last_read_request < PMS_READ_DELAY)
    return;
  
  if(_last_read_request > 0 && _pms->readUntil(data)) {
    _density_1_0 = data.PM_AE_UG_1_0;
    _density_2_5 = data.PM_AE_UG_2_5;
    _density_10_0 = data.PM_AE_UG_10_0;
  }

    _pms->requestRead();
    _last_read_request = millis();
}
