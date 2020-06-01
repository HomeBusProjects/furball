#! /bin/bash

python ~/.platformio/packages/framework-arduinoespressif32/tools/espota.py -f .pio/build/lolin32/firmware.bin -i $1 -p 3232 -r -d
