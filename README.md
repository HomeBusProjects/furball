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

It should be able to measure and report its own input voltage.

It should express one of its I2C ports to an external optional connector along with power (5V/3.3V/GND).

It might also express one of its SPI ports to an external optional connector (5V/3.3V/GND)..

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

Consider TSL2561  Digital Luminosity sensor - $1 in single unit quantities, AliExpress.

### VOC

Consider CCS811, VOC and CO2 sensor. $9 in single unit quantities, AliExpress.


### Total Cost

If bought through AliExpress, parts cost should run roughly:
- $6 - ESP32 LOLIN32
- $2.57 - BME280
- $1 - TSL2561
- $9 - CCS811
- $1 - random resistors, capacitors, tri-color LED

Total of roughly $20 in parts before the circuit board.

### Thoughts

I2C is unstable when too many things are connected to it. Given that the ESP32 has two I2C controllers, one should be dedicated for add-on boards and the other should avoid having too many items connected to it. So try to use SPI when possible. We really want this thing to be simple and stable.


## Optional Hardware

### Atmospheric

Furball should have an optional expansion card that reports on atmospheric gasses and particles. This board would include an I2C ADC converter, up to four MQ series gas sensors, and a PMS5003 particle sensor. Software will need to consider that MQ series sensors need both burn-in and run-in periods, and that if run full-time a PMS5003 sensor will degrade after about a year of use, so it needs to sleep as much as possible.

### Display

Furball should have optional expansion via an SPI OLED display.

### Control

Furball should have an optional relay expansion.


## Software

For simplicity, Furball will run the ESP32 Arduino SDK using HomeBus to connect to and pubish to an MQTT server.

Software should allow an update to be pushed locally over wifi or pulled automatically.


## Iterations

The intial version is a simple breadboard.

Second version is a soldered printed circuit breadboard.

The third version will be a custom printed circuit board with modules soldered into it.

The fourth, most ambituous version, will be a custom printed circuit board with surface mount components and will have components mounted directly on it rather than using 3rd party modules.


# License

Software and documentation is licensed under the [MIT license](https://romkey.mit-license.org/).

Circuits are licensed under [Creative Commons Attribution Share-Alike license](https://creativecommons.org/licenses/by-sa/4.0). 
