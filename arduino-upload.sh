#!/bin/bash

set -e

sudo arduino-cli compile --fqbn arduino:avr:nano /home/pi/Arduino/WS2801_Arduino_BitBanger
sudo arduino-cli upload  --fqbn arduino:avr:nano:cpu=atmega328old --port /dev/ttyUSB0 --input /home/pi/Arduino/WS2801_Arduino_BitBanger/WS2801_Arduino_BitBanger.arduino.avr.nano.hex --verify

