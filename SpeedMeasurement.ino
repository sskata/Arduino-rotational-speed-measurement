//Rotational speed measurement examples

#include "SpeedMeasurement.h"
#define SPEED_INPUT_PIN 2

unsigned int allTimeMax = 0;  //Keep track of all time high speed in example #1

void setup() {
  Serial.begin(9600);
  speedMeasurement::init(SPEED_INPUT_PIN);  //Initialize by defining input pin
  //speedMeasurement::speedCalculationEnable(); //Allow continuous speed (rpm) update, only needed for example #2
}

void loop() {
  /* Example #1 */
  speedMeasurement::measureSpeed(1000); //Update speed variables, 1000 ms measurement period
  Serial.print(speedMeasurement::rpm);  //Last measured (current) rpm
  Serial.print("; ");
  Serial.print(speedMeasurement::rpmMin); //Min rpm during measurment cycle
  Serial.print("; ");
  Serial.print(speedMeasurement::rpmMax); //Max rpm during measurment cycle
  Serial.print("; ");
  Serial.print(speedMeasurement::rpmAverage); //Average rpm during measurment cycle
  Serial.print("; ");
  if(speedMeasurement::rpmMax > allTimeMax) allTimeMax = speedMeasurement::rpmMax;
  Serial.print(allTimeMax); //All time highest measured speed
  Serial.print("\n");

  /* Example #2
  if(speedMeasurement::newValueReady == 0) speedMeasurement::rpm = 0; //Assume rpm to be 0 if no interrupts received within below delay
  Serial.println(speedMeasurement::rpm);
  speedMeasurement::newValueReady = 0; //Reset variable
  delay(1000);
  */
}
