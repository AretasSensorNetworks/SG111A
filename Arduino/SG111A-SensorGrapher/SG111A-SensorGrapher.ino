unsigned long POLLING_DELAY = 5000; //the polling interval in miliseconds
unsigned long BLINK_DELAY = 1000;
unsigned long CYCLE_INTERVAL = 1000; //length of time that controls the sensor gas sensor read cycle

//make sure to use an AnySerial that supports Leonardo (if you need that)
//e.g. https://github.com/ElDuderino/AnySerial
#include <AnySerial.h>
#include <AretasSG11xx.h>

unsigned long pm0 = 0; //polling interval millis place holder
unsigned long pm1 = 0; //secondary polling interval millis placeholder
unsigned long cm0 = 0; //main cycle millis placeholder

boolean POUT = false; //whether or not to print to console (useful for or'ing when calibrating)
boolean CALIBRATING = true;

int LED_STATUS = HIGH;

AnySerial co2Serial(&Serial1);
AretasSG11xx sgxx(&co2Serial);

//if you have a valid account or want to submit data to the API, 
//change this to the device MAC you created in your account
unsigned long mac = 0; 

void setup(){

  Serial.begin(9600);
  //this basically blocks initialization until USB is connected on Leonardo
  while(!Serial);

  sgxx.begin();
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

      POUT = true;  
      //save the last time we polled the sensors
      pm0 = currentMillis; 
      
      co2Status = sgxx.getCO2();

      if((co2Status != 0) && (co2Status != -1) && (co2Status > 0)){

        Serial.print(mac);
        Serial.print(',');
        Serial.print(181);
        Serial.print(',');
        Serial.print(co2Status);
        Serial.print('\n');

      }else{
        Serial.print("CO2ERR [");
        Serial.print(co2Status);
        Serial.print("]\n");
      }

    }else{
      
      POUT = false;
    }

    //blink
    digitalWrite(10, LED_STATUS);
    
    if(LED_STATUS == LOW){
      LED_STATUS = HIGH;
    }else {
      LED_STATUS = LOW;
    }
    
  }
}
