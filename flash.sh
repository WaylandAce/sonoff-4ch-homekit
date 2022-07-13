#!/bin/bash

SONOFF_PORT="/dev/ttyUSB0"

esptool \
            -p $SONOFF_PORT \
            --baud 115200 \
            write_flash \
            -fs 8m \
            -fm dout \
            -ff 40m \
            0x0 ./src/firmware/rboot.bin \
            0x1000 ./src/firmware/blank_config.bin \
            0x2000 ./src/firmware/sonoff_4ch.bin
