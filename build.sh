#!/bin/bash

echo "Enter device serial number:"

read deviceSerialNumber

docker run -v "$(pwd)"/src/:/home/sdkbuilder/esp-homekit/src --env DEVICE_SERIAL_NUMBER=$deviceSerialNumber -it cross/esp8266
