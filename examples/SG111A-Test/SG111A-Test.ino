//make sure to use an AnySerial version that supports Leonardo, such as:
//https://github.com/ElDuderino/AnySerial
#include <AnySerial.h>
#include <AretasSG11xx.h>
#include <SoftwareSerial.h>

/**
 * Comment this out to enable "standard mode (aka broadcast mode)"
 * If you use aperiodic mode, it disabled "broadcast mode" in the sensor
 * and will not be re-enabled until the sensor is reset
 * Therefore once you have called the sensor with aperiodic mode, all subsequent calls 
 * to get the CO2 value must use the aperiodic function
 */
#define APERIODIC 1

unsigned long POLLING_DELAY = 5000; //the polling interval in miliseconds
unsigned long BLINK_DELAY = 1000;
unsigned long CYCLE_INTERVAL = 1000; //length of time that controls the sensor gas sensor read cycle


unsigned long pm0 = 0; //polling interval millis place holder
unsigned long pm1 = 0; //secondary polling interval millis placeholder
unsigned long cm0 = 0; //main cycle millis placeholder

boolean POUT = false; //whether or not to print to console (useful for or'ing when calibrating)
boolean CALIBRATING = true;

int LED_STATUS = HIGH;

//for a standard Aretas Board (a.g. A800028, A800016, A800046, etc.)
//SoftwareSerial sgSerial(A0, A1); //change the pins to match your requirements
//AnySerial co2Serial(&sgSerial);
//AretasSG11xx sgxx(&co2Serial);

//for A800040
AnySerial co2Serial(&Serial1);
AretasSG11xx sgxx(&co2Serial);

void setup(){
  
  Serial.begin(9600);
  sgxx.begin();
  Serial.println("INIT BOARD SUCCESS");
  pinMode(13, OUTPUT);

/** UNCOMMENT TO TEST THE COMMAND FUNCTIONS 
 * 
 * testCommandFunctions();
 *
 */

}
/**
 * Some tests for the command functions
 * 
 * These tests will:
 * Read ABC Mode
 * Disable ABC Mode
 * Read ABC Mode
 * Enable ABC Mode
 * Read ABC Mode
 * Read ABC Duration
 * Set ABC Duration
 * Read ABC Duration
 * Set ABC Duration back to default (7)
 * Read ABC Duration
 */
void testCommandFunctions(){

  //read the ABC setting
  int8_t abcMode = sgxx.getABCMode();
  Serial.print("ABC MODE:");
  Serial.println(abcMode);

  //set the ABC setting
  Serial.print("Disable ABC Mode Result:");
  int8_t abcModeSetResult = sgxx.setABCMode(false);
  Serial.println(abcModeSetResult);

  //read the ABC setting
  abcMode = sgxx.getABCMode();
  Serial.print("Read ABC MODE:");
  Serial.println(abcMode);

  //turn it back on again
  Serial.print("Enable ABC Mode Result:");
  abcModeSetResult = sgxx.setABCMode(true);
  Serial.println(abcModeSetResult);

  //aaaand read it back one more time
  abcMode = sgxx.getABCMode();
  Serial.print("Read ABC MODE:");
  Serial.println(abcMode);

  //read the ABC duration
  uint16_t abcDuration = sgxx.getABCDuration();
  Serial.print("Read ABC Duration:");
  Serial.println(abcDuration);

  //set the ABC to something else
  int8_t setABCDurationResult = sgxx.setABCDuration(5); //set it to 5 days
  Serial.print("Set ABC Duration to 5 days result:");
  Serial.println(setABCDurationResult);

  abcDuration = sgxx.getABCDuration();
  Serial.print("Read ABC Duration after set:");
  Serial.println(abcDuration);

  setABCDurationResult = sgxx.setABCDuration(7);
  Serial.print("Set ABC Duration back to 7 days result:");
  Serial.println(setABCDurationResult);

  abcDuration = sgxx.getABCDuration();
  Serial.print("Read ABC Duration after set back to 7:");
  Serial.println(abcDuration);

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

#ifndef APERIODIC
      co2Status = sgxx.getCO2();
#else
      co2Status = sgxx.getCO2aPeriodic();
#endif
      if((co2Status < 350) || (co2Status > 10000)){
        Serial.print("READ ERROR:");
        Serial.print(co2Status);
        Serial.print('\n');
      }else{
        Serial.print("CO2 VAL:"); Serial.print(co2Status); Serial.print('\n');
      }
      
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
