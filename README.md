# MiniWeather

MiniWeather is a small Arduino project for a ESP8266 with a small OLED + DHL11 sensors. It shows the temperature in Celcius and the Humidity. Can also connect to a Wifi network and make the values available there.

![images/iot-board.png](images/iot-board.png)

Available from places like Banggood. Look for "Geekcreit ESP8266 IoT Development".

Was coded for fun and the challenge to use [PlatformIO](https://platformio.org).

Install PlatformIO. There is a `Makefile` with which one can do

- `make build` build binary
- `make install` (_build and_) upload to a device if connected
- `make clean` clean directory
