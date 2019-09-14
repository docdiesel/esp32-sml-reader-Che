
# esp32-sml-reader-Che

Code for letting an ESP32 read sml data from a smart meter via its IR
interface.

## License
This code is released under the MIT license. See license.txt.

## Configuration
All configuration is done in config.h. Define user and password for wlan
here, as well as target databases, baud rate of your sensor etc.

## sensor
The sensor is a simple IR photo transistor that's hooked up to the ESP32,
using an 1K pullup and an 10K pulldown resistor to give a decent
signal. An image showing the circuit is added to the projekt as
"wiring.jpg". (The sensor is based on a simple IR reader like described
at https://wiki.volkszaehler.org/howto/simpler_ir_leser .)

Basically, it's a 1:10 voltage divider, keeping the level high (1) by default.
Once the IR photo transistor gets triggered, it shortcuts the pulldown
resistor and thus pulls the level to low (0).



