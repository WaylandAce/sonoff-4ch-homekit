# Sonoff homekit firmware

Example of using esp-homekit library to control a simple Sonoff 4cha using HomeKit.
The esp-wifi-config library is also used in this example. This means you don't have to specify your network's SSID and password before building.

## Supported & tested devices
* Sonoff 4CH
* Sonoff MINIR2


### Sonoff 4CH
In order to flash the sonoff basic you will have to have a 3,3v (logic level) FTDI adapter.

To flash this example connect 3,3v, TX, RX, GND in this order, beginning in the (square) pin header next to the button.
Next hold down the button and connect the FTDI adapter to your computer. The sonoff is now in flash mode and you can flash the custom firmware.

*WARNING*: Do not connect the sonoff to AC while it's connected to the FTDI adapter! This may fry your computer and sonoff.

the [firmware](src/firmware/sonoff_4ch.bin) needs to be flashed to 0x4000, rboot is required (see flash.sh)

### Sonoff MINIR2
Flash HOW-TO


## How to build

```
docker build --tag=cross/esp8266 .
```

### run
```
./build.sh
```
