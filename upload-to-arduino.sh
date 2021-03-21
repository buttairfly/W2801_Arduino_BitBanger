#!/bin/bash

set -e

ARDUINO_TYPE="arduino:avr:nano"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BUILD_DIR="${DIR}/build/${ARDUINO_TYPE//:/.}"

git pull

source ${DIR}/version.sh

arduino-cli compile \
  --fqbn ${ARDUINO_TYPE} \
  --format json \
  --export-binaries \
  ${DIR}
arduino-cli upload  \
  --fqbn ${ARDUINO_TYPE}:cpu=atmega328old \
  --port /dev/ttyUSB0 \
  --input-file ${BUILD_DIR}/WS2801_Arduino_BitBanger.ino.hex \
  --format json \
  --verify
