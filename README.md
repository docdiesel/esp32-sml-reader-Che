
# esp32-sml-reader-Che

## TL;DR
Code for letting an ESP32 read sml data from a smart meter via its IR
interface.

## About
I wrote this little hack because I didn't want to wire up my raspberry down
the cellar and to the smartmeter - besides, there was a smartmeter but no
power supply, so I had to use a powerbank which the raspi would drain within
short time.

Thus now an ESP32 is reading the data and writes it to an InfluxDB (Cloud) and to a Volkszaehler instance
(raspberry at home).

## License
This code is released under the MIT license. See license.txt.

## Configuration
All configuration is done in config.h. Define user and password for wlan
here, as well as target databases, baud rate of your sensor etc.

## Sensor
The sensor is a simple IR photo transistor that's hooked up to the ESP32,
using an 1K pullup and an 10K pulldown resistor to give a decent
signal. An image showing the circuit is added to the projekt as
"wiring.jpg". (The sensor is based on a simple IR reader like described
at https://wiki.volkszaehler.org/howto/simpler_ir_leser .)

Basically, it's a 1:10 voltage divider, keeping the level high (1) by default.
Once the IR photo transistor gets triggered, it shortcuts the pulldown
resistor and thus pulls the level to low (0).

## Serial interface
The data is read using one of the three hardware serial interfaces the ESP32
has. Initially I tried an ESP8266 with software serial interface on some
freely configurable GPIO, but I experienced too much data loss on that, so I
decided to go for ESP32 and hardware serial, even if the ESP8266 has the
better sleep modes.

## Power saving
I've got to power my ESP32 with a power bank. To save some power, a) it's going
to deep sleep for 57s and then starts reading from the smartmeter again, and
b) it powers the sensor over GPIO4 so the sensor is switched off during
deep sleep.

## Values
You may define which fields/values you want to read from the smartmeter. I'm
reading
- 1.8.0 : energy in kWh from grid so far
- 2.8.0 : energy in kWh to grid so far, from PV unit
- 16.7.0 : current power in W passing the smartmeter
You may want to read others. Find a list of OBIS identifiers at https://de.wikipedia.org/wiki/OBIS-Kennzahlen
.

## Targets
The first version supports three backends as target:
- Influx tineseries DB, e.g. at corlysis.com or at own site. You may use Grafana to visualize the data later.
- Volkszähler.org instance, presumably on a raspberry at home. Find more info at https://www.volkszaehler.org/
- Plain web server, in my case just to have the values in the web servers error log, for debugging purposes. Of course you may use this with an own PHP or Python script to write the data to a database.

