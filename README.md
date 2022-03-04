# SG111A
Firmware for Sensera / Nanotron / Inpixon CO2 sensor

Firmware for SG111A / SG112A / SG112B / PSA112ASN / PSA112BSN

For now, a basic Arduino-compatible sketch for the SG111A CO2 sensor using software UART

The Arduino hook-up page is here: 

http://www2.aretas.ca/knowledge-base/arduino-co2-sensor-sg111a/
and here:
http://www2.aretas.ca/knowledge-base/arduino-compatible-co2-sensor-sg112a/

All the sensors share the same protocol 

The A800040 also supports the sensors but runs an Atmega32u4:

![A800040 USB CO2 Developer Kit](http://www2.aretas.ca/wp-content/uploads/2019/11/A800040-1.0-Top-300x300.png)

Update 1/20/2020

The Arduino library uses AnySerial, since it's a huge kludge to support different serial port types without it. If you need to support Leonardo, you might want to use: https://github.com/ElDuderino/AnySerial

I can also supply binaries / hex files up here for anyone using the Aretas supplied dev kits and doesn't want to figure out how to compile the software in the IDE but want to change or refresh their sketches

For now the Arduino portion of this repo will stay as is. But I'm happy to consider any change requests or bug fixes!
