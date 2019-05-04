# The Furball

Furball is an open-source hardware project intended to help with the research and development of HomeBus, an IoT auto-provisioning framework.

The name 'Furball' is an homage to Dave Mills' ["Fuzzball"](https://en.wikipedia.org/wiki/Fuzzball_router), one of the first routers on the nascent Internet.

The Furball hardware performs environmental monitoring and is intended to be deployed indoors with fixed power source. It will auto-provision with HomeBus to feed sensor readings into an IoT system.

## Hardware

Furball is based on the ESP32 processor. ESP32 is more capable than its predecessor, the ESP8266 while remaining inexpensive. The ESP32 supports both 802.11b/g/n Wifi and Bluetooth 4.2/BLE. It also has hardware acceleration for encryption. It includes 4 SPI controllers, 2 I2C controllers, 3 UARTs, up to 18 channels of ADC and two 8 bit DACs. 

The hardware should support several common environmental sensors in order of importance:
- temperature
- humidity
- occupancy 
- air pressure 
- light
- sound pressure
- volatile organics

It should express one of its I2C ports to an external optional connector along with power (5V/3.3V/GND).

It shoud be powered via USB.

Its USB port should be accessible externally for programming in addition to power.

It should include a tri-color LED for debugging and identification purposes.

### Temperature, Humidity and Pressure

Consider BME280 - temperature/pressure/humidity. $2.57 in single unit quantities, AliExpress.

### Occupancy

Consider a PIR sensor connected to a digital input.

### Sound Pressure

Consider an analog circuit.

Research microphones to find a decent but cheap one. Saw reference to POM-3535L-3-LW100-R.

### Light

Furball uses the SL2561  Digital Luminosity sensor - $1 in single unit quantities, AliExpress.

### VOC

Furball uses the BME680, which is a BME280 plus a VOC sensor. If you're building a Furball which can read VOCs, you only need the BME680 and not the BME280.

(note: I originally used a CCS811 but had a lot of problems with cheaper units. It also doesn't truly measure CO2 but infers it from the VOC reading.)

### Total Cost

If bought through AliExpress, parts cost should run roughly:
- $6 - ESP32 LOLIN32
- $2.57 - BME280
- $1 - TSL2561
- $9 - BME680
- $1 - random resistors, capacitors, tri-color LED

Total of roughly $20 in parts before the circuit board.

## Optional Hardware

### Atmospheric

Furball should have an optional expansion card that reports on atmospheric gasses and particles. This board would include an I2C ADC converter, up to four MQ series gas sensors, and a PMS5003 particle sensor. Software will need to consider that MQ series sensors need both burn-in and run-in periods, and that if run full-time a PMS5003 sensor will degrade after about a year of use, so it needs to sleep as much as possible.

## Software

Furball's software is built on the Arduino Core.

Software should allow an update to be pushed locally over wifi or pulled automatically.


## Iterations

The intial version is a simple breadboard.

Second version is a soldered printed circuit breadboard.

The third version will be a custom printed circuit board with modules soldered into it.

The fourth will be a custom printed circuit board with surface mount components and will have components mounted directly on it rather than using 3rd party modules.


# License

Software and documentation is licensed under the [MIT license](https://romkey.mit-license.org/).

Circuits are licensed under [Creative Commons Attribution Share-Alike license](https://creativecommons.org/licenses/by-sa/4.0). 
