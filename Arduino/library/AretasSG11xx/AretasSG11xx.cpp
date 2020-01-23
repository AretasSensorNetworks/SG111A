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
 */
int AretasSG11xx::getCO2(){

  int buf_index = 0;

  //maximum time spent reading serial port
  long TIMEOUT_MILLIS = 4000; //4 seconds

  //timeout 
  long start = millis();

  byte curByte = 0x00;
  byte cmdByte = 0x00;
  byte cmdSz = 0x00;

  boolean done = false;

  zeroBuffer(serBuf, SER_BUF_SZ);

  int hasCO2 = 0;

  while(!done){

    if((millis() - start) > TIMEOUT_MILLIS){
      //timeout
      return -1;
    }


    if(anySerial->available()){

      //don't overflow the buffer
      if(buf_index >= (SER_BUF_SZ - 1)){
        buf_index = 0;
      }

      curByte = anySerial->read();
      serBuf[buf_index] = curByte;

      if(_DEBUGALL){
        Serial.print("BYTE["); 
        Serial.print(buf_index); 
        Serial.print("]:"); 
        Serial.print(serBuf[buf_index], HEX); 
        Serial.print('\n');
      }
        
      if(buf_index >= 5){
        
        if((serBuf[buf_index - 5] == 0xBB) && (serBuf[buf_index - 4] == 0x66)){ //sync bytes

          if(_DEBUGALL){
            Serial.print("GOT HEAD\n\n");
          }
          
          cmdByte = serBuf[buf_index - 3];
          cmdSz = serBuf[buf_index - 2];

          if(_DEBUGALL){
            
            Serial.print("CMD:");
            Serial.print(cmdByte);
            Serial.print(" SZ:");
            Serial.print(cmdSz);
            Serial.print('\n');

          }

          if(cmdByte == 0x15){

            if(_DEBUGALL){
              Serial.print("GOT CMD BYTE\n\n");
            }

            //PPM = (MSB x 256) + LSB
            uint16_t ppm = (serBuf[buf_index] * 256) + serBuf[buf_index - 1];
            //uint16_t ppm = (msb * 256) + lsb;

            if(_DEBUGALL){
              
              Serial.print("PPM MSB:");
              Serial.print(serBuf[buf_index], HEX);
              Serial.print('\n');

              Serial.print("PPM LSB:");
              Serial.print(serBuf[buf_index - 1], HEX);
              Serial.print('\n');

              Serial.print("PPM:");
              Serial.print(ppm);
              Serial.print('\n');

            }

            return ppm;
            
          }

          return -2;
          
        }

      }

      buf_index++;

    }
    
  }

  return -3;

}

void AretasSG11xx::zeroBuffer(uint8_t *buf, int bufSz){

  int i = 0;
  for(i = 0; i < bufSz; i++){
    buf[i] = 0;
  }
}



byte AretasSG11xx::getsTimeout(char *buf, int BUF_LENGTH, uint16_t timeout, boolean filter = false) {
    
    byte count = 0;
    long timeIsOut = 0;
    byte c;
    *buf = 0;
    timeIsOut = millis() + timeout;

    anySerial->listen();
    
    while (timeIsOut > millis() && count < (BUF_LENGTH - 1)) {
        
        if (anySerial->available() > 0) {
            count++;
            c = anySerial->read();
            if(filter == true)
                if(c == '=') continue;
            *buf++ = c;
            timeIsOut = millis() + timeout;
        }
    }
    
    if (count != 0) {
        *buf = 0;
        count++;
    }
    
    return count;
}

/**
 * Note that this CRC16 implementation provided in the datasheet does NOT seem to work 
 * therefore it's not being used
 */
uint16_t AretasSG11xx::CalcCRC16(uint8_t *cmd, int cmd_length) {
  
  uint16_t ret = 0xffff;
  uint16_t polynomial = 0xa001;
  int shift = 0x0;
  int i = 0;
  for (i = cmd_length - 1; i >= 0 ; i-- ){
    
    uint16_t code = ( uint16_t )( cmd [ cmd_length -1 - i ]  & 0xff );
    ret = ret^code;
    shift = 0x0;
    while ( shift <= 7 ){           
      if ( ret & 0x1 ) {                     
        ret = ret >> 1;                     
        ret = ret^polynomial ;
      } else {
        ret = ret >> 1;
      }
      shift++;
    }
  }      
  return ret;
}

