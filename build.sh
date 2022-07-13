#!/bin/sh
docker run -e SYSPARAM_DEBUG=3 -e WIFI_CONFIG_DEBUG -v "$(pwd)"/src/:/home/sdkbuilder/esp-homekit/src  -it cross/esp8266
