# Arduino-rotational-speed-measurement
Measure rotational speed with an Arduino.

## Features

- Measures the revolutions per minute of e.g. a cup anemometer, each time measureSpeed() is called a measurement cycle is executed for about the specified time passed to the function (3000 ms default). Possible to also read current speed directly from speedMeasurement::rpm (speedCalculationEnable() must be called first).
- Trigger wheel teeth defined by PULSES_PER_REVOLUTION.

 ### Anemometer used in the example
 - Two "teeth" per revolution, not equal pulsewidths therefore speed is calculated as average of two pulsewidths.

## Known issues/risks

- If speed is directly read from speedMeasurement::rpm, speed will never go to zero since if speed is zero no interrupts are received, thus this has to be handled somehow.
 - The variable speedMeasurement::rpmAverage is not rounded correctly due to integer arithmetic e.g. 108,9 rpm -> 108 rpm.
 - The resolution of the speedMeasurement::rpm variable is quite poor.
