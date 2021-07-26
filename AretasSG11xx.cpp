#include "Arduino.h"
#include "AretasSG11xx.h"
#include <string.h>
#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
#include <AnySerial.h>

/**
 * pass in an AnySerial object,
 * preferably an Aretas forked AnySerial to support Leonardo correctly
 * 
 * https://github.com/ElDuderino/AnySerial
 * 
 */
AretasSG11xx::AretasSG11xx(AnySerial *serial){
    anySerial = serial;
}

/**
 * enable verbose debugging (output all received bytes etc.)
 */
void AretasSG11xx::enableDebug(boolean arg){
    _DEBUGALL = arg;
}

void AretasSG11xx::begin(){
    anySerial->begin(9600);
}

/**
 * one basic function for now, just return the CO2 as an int value
 * -1 return value indicates something weird
 * -2 indicates timeout reading the serial port
 * -3 indicates checksum fail after nRetries
 * 
 */
int AretasSG11xx::getCO2(){

	//the largest command result is ~15 bytes
	uint8_t serBuf[SER_BUF_SZ];

  for(byte i = 0; i < SER_BUF_SZ; i++){
    serBuf[i] = 0;
  }

  //maximum time spent reading serial port
  uint32_t TIMEOUT_MILLIS = 4000; //4 seconds

  //begin time
  uint32_t start = millis();

  uint16_t ppm = 0;
  uint8_t len = 0;
  uint16_t checksumA = 0;
  uint16_t checksumB = 0;

  boolean done = false;

  uint8_t nRetries = 5;

  anySerial->listen();

  while(!done){

    if((millis() - start) > TIMEOUT_MILLIS){
      //timeout
      return -2; //timeout returns it's own value
    }
    
    //if nRetries is decremented, it means we got to where checksums are checked and they didn't match
    if(nRetries == 0){
      return -3;
      break;
    }

    if(anySerial->available()){

      if(anySerial->readBytes(serBuf, 4) == 4){

#ifdef DEBUG

        Serial.print(serBuf[0], HEX);
        Serial.print(serBuf[1], HEX);  
        Serial.print(serBuf[2], HEX); 
        Serial.print(serBuf[3], HEX);
        Serial.print('\n');
#endif
        if(serBuf[0] == 0xBB && serBuf[1] == 0x66 && serBuf[2] == 0x15){
          
          //the length of the payload is defined in the 4th byte
          len = serBuf[3];
#ifdef DEBUG
          Serial.print("LEN:");
          Serial.println(len);
#endif
          if(anySerial->readBytes(serBuf, len + 2) == (len + 2)){
#ifdef DEBUG
            Serial.print(serBuf[0], HEX);
            Serial.print(' ');
            Serial.print(serBuf[1], HEX);
            Serial.print(' ');
            Serial.print(serBuf[2], HEX); 
            Serial.print(' ');
            Serial.print(serBuf[3], HEX);
            Serial.print('\n');
#endif            
            ppm = serBuf[1] << 8;
            ppm = ppm + serBuf[0];

#ifdef DEBUG
            Serial.print("PPM:");
            Serial.println(ppm);
#endif
            checksumA = serBuf[3] << 8;
            checksumA = checksumA + serBuf[2];

            //rearrange the buffer
            serBuf[4] = serBuf[0];
            serBuf[5] = serBuf[1];
            serBuf[6] = serBuf[2];
            serBuf[7] = serBuf[3];

            serBuf[0] = 0xBB;
            serBuf[1] = 0x66;
            serBuf[2] = 0x15;
            serBuf[3] = len;

            checksumB = CalcCRC16(serBuf, 6);

#ifdef DEBUG
            Serial.println("Checksums:");
            Serial.println(checksumA, HEX);
            Serial.println(checksumB, HEX);
#endif
            //the only way we get here is if we got the sync bytes, command byte and tried to read ppm / checksum
            if(checksumA == checksumB){
              return ppm;
            }else{
              //try again
              nRetries--;
            }

          }

        }

      }

    }//endif(anySerial->available())

  }

  return -1; //some other strange condition?
}

/**
 * Calculate a CRC 16 checksum 
 */
uint16_t AretasSG11xx::CalcCRC16 ( uint8_t *cmd , int cmd_length ){
  
  uint16_t ret = 0xffff , polynomial = 0xa001 ;
  int shift = 0x0;
  
  for (int i = cmd_length - 1; i >= 0; i--) {
    
    uint16_t code = ( uint16_t )( cmd [ cmd_length -1 - i ] & 0xff );
    ret = ret ^ code ;
    shift = 0x0;
    
    while ( shift <= 7 ) {
      
      if ( ret & 0x1 ) {
        ret = ret >> 1;
        ret = ret ^ polynomial ;
      } else {    
        ret = ret >> 1;
      }
      
      shift++;
    }
  }
  return ret;
}

