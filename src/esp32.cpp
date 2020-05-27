#ifdef ESP32

#include <Arduino.h>
#include <Esp.h>


extern "C" {
#include "soc/efuse_reg.h"
};

#if 0
static int cpu_vevision() {
  return REG_READ(EFUSE_BLK0_RDATA3_REG) >> (EFUSE_RD_CHIP_VER_RESERVE_S) && EFUSE_RD_CHIP_VER_RESERVE_V;
}
#endif

int cpu_version() {
  //  return (REG_READ(EFUSE_BLK0_RDATA3_REG) >> (EFUSE_RD_CHIP_VER_RESERVE_S)&&EFUSE_RD_CHIP_VER_RESERVE_V) ;

  return REG_READ(EFUSE_BLK0_RDATA3_REG) &&  EFUSE_RD_CHIP_VER_REV1;
}


void esp_output() {
  Serial.println();

  Serial.printf("SDK version %s\n", ESP.getSdkVersion());
  Serial.printf("Flash Chip Speed %d\n", ESP.getFlashChipSpeed());
  Serial.printf("CPU version %d\n", cpu_version());
  Serial.print("REG_READ(EFUSE_BLK0_RDATA3_REG) ");
  Serial.println(REG_READ(EFUSE_BLK0_RDATA3_REG), BIN);
  Serial.print("EFUSE_RD_PART_VER_RESERVE_S ");
  Serial.println( EFUSE_RD_BLK3_PART_RESERVE_S, BIN);
  Serial.print("EFUSE_RD_PART_VER_RESERVE_V ");
  Serial.println( EFUSE_RD_BLK3_PART_RESERVE_V, BIN);
  Serial.println();
  Serial.print("Chip Revision (official version): ");
  Serial.println(cpu_version());
  Serial.print("Chip Revision from shift Opration ");
  Serial.println(REG_READ(EFUSE_BLK0_RDATA3_REG) >> 15, BIN);
  Serial.println();
  Serial.println();
}

#endif
