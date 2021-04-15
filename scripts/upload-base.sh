#!/bin/bash

set -e

PROGRAMM_NAME="$1"
ARDUINO_TYPE="$2"
DEVICE="$3"
DIR="$4"

BUILD_DIR="${DIR}/build/${ARDUINO_TYPE//:/.}"
MAIN_FILE="${PROGRAMM_NAME}.ino"

git pull

source ${DIR}/version.sh ${PROGRAMM_NAME}

arduino-cli compile \
  --fqbn ${ARDUINO_TYPE} \
  --format json \
  --export-binaries \
  ${DIR}

arduino-cli upload  \
  --fqbn ${ARDUINO_TYPE}:cpu=atmega328old \
  --port ${DEVICE} \
  --input-file ${BUILD_DIR}/${MAIN_FILE}.hex \
  --format json \
  --verify
