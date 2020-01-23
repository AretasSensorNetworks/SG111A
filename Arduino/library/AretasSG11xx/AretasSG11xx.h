#ifndef AretasSwarmBeeLE_h
#define AretasSwarmBeeLE_h

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
	byte getsTimeout(char *buf, int BUF_LENGTH, uint16_t timeout, boolean filter = false);
	AnySerial *anySerial;
	void zeroBuffer(uint8_t *buf, int bufSz);
	int _targetBaudRate = 9600;
    boolean _DEBUGALL = false;
	uint16_t CalcCRC16(uint8_t *cmd, int cmd_length);
	static const int SER_BUF_SZ = 16;
  
	//the largest command result is ~15 bytes
	uint8_t serBuf[SER_BUF_SZ]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

};

#endif