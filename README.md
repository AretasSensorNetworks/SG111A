# SG111A
Firmware for SG111A

For now, a basic Arduino-compatible sketch for the SG111A CO2 sensor using software UART

I'm still working on the CRC16 implementation and sensor characterization, so for now, just the test sketch. 

Update 14-07-2019 - Characterization is complete, the sensor tracks well with reference and responds well to calibration references.

Expect to see a library, example documentation and more from this repo soon. We'll also provide examples for Java, Python, etc. 

The Arduino hook-up page is here: http://www2.aretas.ca/knowledge-base/arduino-co2-sensor-sg111a/

Update 1/20/2020

The Arduino library has been created and this code base refactored to use the library instead. It's still not organized like a typical Arduino library, but it should be ok for most 

The Arduino library uses AnySerial, since it's a huge kludge to support different serial port types without it. If you need to support Leonardo, you might want to use: https://github.com/ElDuderino/AnySerial

I will also likely just put some binaries / hex files up here for anyone using the Aretas supplied dev kits that don't want to figure out how to compile the software in the IDE but want to change or refresh their sketches

For now the Arduino portion of this repo will stay as is. But I'm happy to consider any change requests or bug fixes