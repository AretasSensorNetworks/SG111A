#include <SoftwareSerial.h>

unsigned long POLLING_DELAY = 5000; //the polling interval in miliseconds
unsigned long BLINK_DELAY = 1000;
unsigned long CYCLE_INTERVAL = 1000; //length of time that controls the sensor gas sensor read cycle

SoftwareSerial sser(7,6);

unsigned long pm0 = 0; //polling interval millis place holder
unsigned long pm1 = 0; //secondary polling interval millis placeholder
unsigned long cm0 = 0; //main cycle millis placeholder

boolean POUT = false; //whether or not to print to console (useful for or'ing when calibrating)
boolean CALIBRATING = true;

int LED_STATUS = HIGH;

const int SER_BUF_SZ = 16;
//the largest command result is ~15 bytes
uint8_t serBuf[SER_BUF_SZ]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void setup(){
  
  Serial.begin(9600);
  Serial.println("INIT BOARD SUCCESS");
  pinMode(13, OUTPUT);
  
}

void loop(){
  
  unsigned long currentMillis = millis();

  int co2Status = 0;
  
  //main cycle interval (5 seconds or so)
  if((currentMillis - cm0 > CYCLE_INTERVAL) || (cm0 == 0)) {
    
    cm0 = currentMillis;

    if((currentMillis - pm0 > POLLING_DELAY) || (pm0 == 0)) {

      Serial.println(".");
     
      POUT = true;  
      //save the last time we polled the sensors
      pm0 = currentMillis; 

      co2Status = getCO2();

      Serial.print("STATUS:"); Serial.print(co2Status); Serial.print('\n');
      
    }else{
      
      POUT = false;
    }

    //blink
    digitalWrite(13, LED_STATUS);
    
    if(LED_STATUS == LOW){
      LED_STATUS = HIGH;
    }else {
      LED_STATUS = LOW;
    }
    
  }
}

uint16_t CalcCRC16(uint8_t *cmd, int cmd_length){
  
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

void zeroBuffer(uint8_t *buf, int bufSz){
  int i = 0;
  for(i = 0; i < bufSz; i++){
    buf[i] = 0;
  }
}

int getCO2(){

  int buf_index = 0;

  //maximum time spent reading serial port
  long TIMEOUT_MILLIS = 4000; //2 seconds

  //timeout 
  long start = millis();

  sser.begin(9600);

  byte curByte = 0x00;
  byte cmdByte = 0x00;
  byte cmdSz = 0x00;

  boolean done = false;

  while(!done){

    if((millis() - start) > TIMEOUT_MILLIS){
      //timeout
      return -1;
    }

    if(sser.available()){

      //don't overflow the buffer
      if(buf_index >= (SER_BUF_SZ - 1)){
        buf_index = 0;
      }

      curByte = sser.read();
      serBuf[buf_index] = curByte;

      Serial.print("BYTE["); Serial.print(buf_index); + Serial.print("]:"); Serial.print(curByte); Serial.print('\n');

      if(buf_index >= 3){
        
        if((serBuf[buf_index - 3] == 0xBB) && (serBuf[buf_index - 2] == 0x66)){ //sync bytes
          
          cmdByte = serBuf[buf_index - 1];
          cmdSz = serBuf[buf_index];

          Serial.print("CMD:");
          Serial.print(cmdByte);
          Serial.print(" SZ:");
          Serial.print(cmdSz);
          Serial.print('\n');

          zeroBuffer(serBuf, SER_BUF_SZ);

          if(cmdByte == 0x15){

            //read 4 more bytes
            //DATA bytes
            serBuf[buf_index + 1] = sser.read();
            serBuf[buf_index + 2] = sser.read();
            //CRC 16 bytes
            serBuf[buf_index + 3] = sser.read();
            serBuf[buf_index + 4] = sser.read();

            //PPM = (MSB x 256) + LSB
            uint16_t ppm = (serBuf[5] * 256) + serBuf[4];

            Serial.print("PPM MSB:");
            Serial.print(serBuf[5], HEX);
            Serial.print('\n');

            Serial.print("PPM LSB:");
            Serial.print(serBuf[4], HEX);
            Serial.print('\n');

            Serial.print("PPM:");
            Serial.print(ppm);
            Serial.print('\n');
            
            Serial.print("CRC LSB:");
            Serial.print(serBuf[6], HEX);
            Serial.print('\n');

            Serial.print("CRC MSB:");
            Serial.print(serBuf[7], HEX);
            Serial.print('\n');

            uint16_t crcResult = CalcCRC16(serBuf, 6);
            
            uint8_t crcLSB = 0;
            crcLSB = crcResult & 0xFF;
            uint8_t crcMSB = 0;
            crcMSB = crcResult >> 8;

            Serial.print("CRC16:");
            Serial.print(crcResult, HEX);
            Serial.print("\n");

            Serial.print("CRC LSB:");
            Serial.print(crcLSB, HEX);
            Serial.print('\n');

            Serial.print("CRC MSB:") ;
            Serial.print(crcMSB, HEX);
            Serial.print('\n');

            return ppm;
            
          }

          return 0;
          
        }
      }

      buf_index++;

    }
    
  }

  return -2;
  
}
