# The Furball

Furball is an open-source hardware project intended to help with the research and development of [Homebus](https://github.com/HomeBusProjects), an IoT auto-provisioning framework and data processing network built on top of MQTT. Note that Homebus is not even alpha quality software at this time and is still undergoing architectural changes and development.

The name 'Furball' is an homage to Dave Mills' ["Fuzzball"](https://en.wikipedia.org/wiki/Fuzzball_router), one of the first routers on the nascent Internet.

The Furball hardware performs environmental monitoring and is intended to be deployed indoors with fixed power source. It will auto-provision with Homebus to feed sensor readings into an IoT system.

## Hardware

Furball is based on the ESP32 processor. ESP32 is more capable than its predecessor, the ESP8266 while remaining inexpensive. The ESP32 supports both 802.11b/g/n Wifi and Bluetooth 4.2/BLE. It also has hardware acceleration for encryption. It includes 4 SPI controllers, 2 I2C controllers, 3 UARTs, up to 18 channels of ADC and two 8 bit DACs. You'll never use all of those at once as it doesn't have enough GPIO pins to express all of those functions, though you can map many of the hardware controllers to particular GPIO pins.

The hardware supports several environmental sensors:
- temperature
- humidity
- air pressure 
- light intensity
- occupancy 
- sound intensity
- volatile organics

It's powered via USB. The hardware currently uses a LOLIN32 board which has a JST connector for a LiPo battery. The software is currently in no way optimized for battery operation.

The hardware also includes either a tri-color LED or RGB LED with controller.

While the Furball hardware is designed to work with the Furball software, it should also work with other software like [ESPEasy32](https://www.letscontrolit.com/wiki/index.php?title=ESPEasy32), [ESPHome](https://esphome.io/) or custom software.

### Temperature, Humidity and Pressure

Furball currently uses the [BME280](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/) - temperature/pressure/humidity sensor, $2.57 in single unit quantities, AliExpress, or the [BME680](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors-bme680/) - temperature/pressure/humidity/VOC sensor, $9.92 in single unit quanitties on AliExpress.

### Occupancy

Furball hardware has a 3 pin header for a [PIR](https://en.wikipedia.org/wiki/Passive_infrared_sensor) sensor.

### Sound Pressure

Furball hardware currently uses a small amplified microphone on a breakout board, compatible with [Sparkfun's Sound Detector](https://www.sparkfun.com/products/12642).

### Light

Furball uses the [TSL2561](https://ams.com/tsl2561)  Digital Luminosity sensor - $1 in single unit quantities, AliExpress.

### VOC

Furball uses the BME680, which is a BME280 plus a VOC sensor. If you're building a Furball which can read VOCs, you only need the BME680 and not the BME280.

(note: I originally used a [CCS811](https://ams.com/ccs811) but had a lot of problems with cheaper units. It also doesn't truly measure CO2 but infers it from the VOC reading.)

### Total Cost

If bought through AliExpress, parts cost should run roughly:
- $6 - ESP32 LOLIN32
- $2.57 - BME280
- $1 - TSL2561
- $9 - BME680
- $1 - random resistors, capacitors, tri-color LED

Total of roughly $20 in parts before the circuit board.

## Optional Hardware

### Particle sensor

Furball has an optional port to connect a Plantower [PMS5003](http://www.plantower.com/en/content/?108.html) particle sensor. The particle sensor allows you to monitor air quality.

## Software

Furball's software is built on the Arduino Core using [PlatformIO](https://platformio.org/) as its build system. I prefer PlatformIO because it's fast, has a good library manager, lets you build for arbitrary platforms without having to manually install support for them, and lets you use your editor of choice.

Once you've installed PlatformIO you can build the Furball firmware by running:
```
platformio run
```

in its top level directory.

It currently supports OTA updates.

## Iterations

The intial version is a simple breadboard.

Second version is a soldered printed circuit breadboard.

The third and current version will be a custom printed circuit board with modules soldered into it.

A hypothetical fourth version would be a custom printed circuit board with surface mount components and will have components mounted directly on it rather than using 3rd party modules.


# License

Software and documentation is licensed under the [MIT license](https://romkey.mit-license.org/).

Circuits are licensed under [Creative Commons Attribution Share-Alike license](https://creativecommons.org/licenses/by-sa/4.0). 
