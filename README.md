# SG111A
Firmware for SG111A

For now, a basic Arduino-compatible sketch for the SG111A CO2 sensor using software UART

The Arduino hook-up page is here: http://www2.aretas.ca/knowledge-base/arduino-co2-sensor-sg111a/

Update 1/20/2020

The Arduino library uses AnySerial, since it's a huge kludge to support different serial port types without it. If you need to support Leonardo, you might want to use: https://github.com/ElDuderino/AnySerial

I will also likely just put some binaries / hex files up here for anyone using the Aretas supplied dev kits that don't want to figure out how to compile the software in the IDE but want to change or refresh their sketches

For now the Arduino portion of this repo will stay as is. But I'm happy to consider any change requests or bug fixes