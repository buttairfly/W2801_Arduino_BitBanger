#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

git pull

source ${DIR}/version.sh

arduino-cli compile \
  --fqbn arduino:avr:nano \
  --format json \
  ${DIR}
arduino-cli upload  \
  --fqbn arduino:avr:nano:cpu=atmega328old \
  --port /dev/ttyUSB0 \
  --input-file ${DIR}/WS2801_Arduino_BitBanger.arduino.avr.nano.hex \
  --format json \
  --verify
