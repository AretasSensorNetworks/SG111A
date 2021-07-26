#ifndef AretasSG11xx_h
#define AretasSG11xx_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AnySerial.h>

/**
 * I think for now we'll only support SoftwareSerial and then if necessary 
 * we'll implement the SerialProxy class I've been thinking about to abstract
 * the HardwareSerial vs SoftwareSerial usage
 */

class AretasSG11xx {
	
	public:	
    AretasSG11xx(AnySerial *serialPort);
	void begin();
    int getCO2();
    void enableDebug(boolean arg);

	private:
	AnySerial *anySerial;
	int _targetBaudRate = 9600;
    boolean _DEBUGALL = false;
	uint16_t CalcCRC16(uint8_t *cmd, int cmd_length);
	static const int SER_BUF_SZ = 16;

};

#endif