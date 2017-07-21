/******************** Rotational speed measurement ********************
 * Author: Sebastian Skata
 * 
 * Features:
 *  - Measures the revolutions per minute of e.g. a cup anemometer,
 *    each time measureSpeed() is called a measurement cycle is
 *    executed for about the specified time passed to the function 
 *    (3000 ms default). Possible to also read current speed directly 
 *    from speedMeasurement::rpm (speedCalculationEnable() must be 
 *    called first).
 *  - Trigger wheel teeth defined by PULSES_PER_REVOLUTION.
 *    
 * Anemometer used in example:
 *  - Two "teeth" per revolution, not equal pulsewidths therefore 
 *    speed is calculated as average of two pulsewidths.
 *  
 * ToDo:
 *  - Improved micros() rollover handling or alternative solution?
 *  - #define parameters vs. handling parameters as argument to init()?
 *  - Use floating point arithmetic or improve resolution of rpm?    
 *  - Remove speed calculation as average of two pulsewidths?
 *  
 * Known issues/risks:
 *  - If speed is directly read from speedMeasurement::rpm, speed will
 *    never go to zero since if speed is zero no interrupts are
 *    received, thus this has to be handled in some other way.
 *  - rpmAverage is not rounded correctly due to integer arithmetic 
 *    e.g. 108,9 rpm -> 108 rpm.
 **********************************************************************/

#ifndef SPEED_MEASUREMENT_H
#define SPEED_MEASUREMENT_H

/* SETTINGS */
#define PULSES_PER_REVOLUTION 2   //Number of pulses per revolution.
#define ISR_DEADTIME 1000         //After first detected falling edge, the ISR is disabled for this period of time in µs (typically more than 150 µs needed).
#define ZERO_SPEED_TIMEOUT 1500   //Time in ms (approximation) to wait before speed is considered 0 (while also not receiving any interrupts).
#define SAMPLING_PERIOD 10        //How fast the measurement loop runs in ms (period time). Number of samples ~(measurementTime / SAMPLING_PERIOD). 

namespace speedMeasurement{
  byte pin = 0; //Speed input pin
  bool newValueReady = 0; //Is true when rpm varible has been updated, only reset by measureSpeed()
  unsigned int rpmMax = 0;  //Max rpm during one measurement cycle
  unsigned int rpmMin = 0;  //Min rpm during one measurement cycle
  unsigned int rpmAverage = 0;  //Average rpm during one measurement cycle
  volatile unsigned int rpm = 0;  //Rotational speed in rpm
  volatile unsigned long int lastPulseWidth = 0;  //Pulsewidth in µs
  volatile unsigned long int pulseWidth = 0;  //Pulsewidth in µs
  volatile unsigned long int lastTime = 0;  //Last measurement time
 
  void speedCalculation(){  //ISR for calculating rpm
    if(micros() > lastTime + ISR_DEADTIME){
      detachInterrupt(speedCalculation);
      lastPulseWidth = pulseWidth;
      pulseWidth = micros() - lastTime;
      lastTime = micros();
      rpm = 120000000 / (PULSES_PER_REVOLUTION * (pulseWidth + lastPulseWidth)); //Average of two pulsewidths
      newValueReady = 1;
      attachInterrupt(digitalPinToInterrupt(pin), speedCalculation, FALLING);      
    }
    else if(micros() < lastTime){ //Handle micros rollover
      detachInterrupt(speedCalculation);
      lastPulseWidth = pulseWidth;
      pulseWidth = (0xFFFFFFFF - lastTime) + micros();
      lastTime = micros();
      rpm = 120000000 / (PULSES_PER_REVOLUTION * (pulseWidth + lastPulseWidth)); //Average of two pulsewidths
      newValueReady = 1;
      attachInterrupt(digitalPinToInterrupt(pin), speedCalculation, FALLING); 
    }
  }

  void speedCalculationEnable(){ //Enable continious rpm update
    attachInterrupt(digitalPinToInterrupt(pin), speedCalculation, FALLING);
  }

  void speedCalculationDisable(){ //Disable continious rpm update
    detachInterrupt(speedCalculation);
  }
  
  void init(byte inputPin){ //Initialize
    pin = inputPin;
  }

  void measureSpeed(long unsigned int measurementTime = 3000){ //Cyclic measurement, measruement time in ms
    speedCalculationEnable();
    for(unsigned int i = 0; i < ZERO_SPEED_TIMEOUT; i++){  //Wait up to ~ZERO_SPEED_TIMEOUT ms on first speed value (interrupt)
      if(newValueReady) break;
      delay(1);
    }
    if(!newValueReady){ //Return 0 rpm if no intterupts detected
      rpm = 0;
      rpmMax = 0;
      rpmMin = 0;
      rpmAverage = 0;
    }
    else{ //Speed measuring/calculation if interrupts detected
      unsigned int samples = 0;
      unsigned int numOfSamples = round(measurementTime / SAMPLING_PERIOD);
      rpmMax = 0;
      rpmMin = 0xFFFF;
      rpmAverage = 0;
      while(samples < 20000 && samples < numOfSamples){
        if(rpm > rpmMax) rpmMax = rpm;
        if(rpm < rpmMin) rpmMin = rpm;
        rpmAverage += rpm;
        samples++;
        delay(SAMPLING_PERIOD); //Wait for interrupts
      }
      rpmAverage = round(rpmAverage / samples);
    }
    speedCalculationDisable();
    newValueReady = 0;
  }
}

#endif
