#!/bin/bash

set -e

PROGRAMM_NAME="serial_test"
ARDUINO_TYPE="arduino:avr:nano"
DEVICE="/dev/ttyUSB0"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

BASE_DIR="$( dirname "${DIR}" )"

source ${BASE_DIR}/scripts/upload-base.sh $PROGRAMM_NAME $ARDUINO_TYPE $DEVICE $DIR
