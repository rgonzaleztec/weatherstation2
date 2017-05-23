#include <SparkFunMPL3115A2.h>
#include <Wire.h>

MPL3115A2 pressureSensor; //Pressure Sernsor

void setup()
{
  Wire.begin();        // Join i2c bus
  Serial.begin(9600);  // Start serial for output

  /* //////////////////////////////////////////
   * //////////////////////////////////////////
      Pressure Setup
   * //////////////////////////////////////////
  * //////////////////////////////////////////*/
  pressureSensor.begin(); // Get sensor online
  // Configure the sensor
  //pressureSensor.setModeAltimeter(); // Measure altitude above sea level in meters
  pressureSensor.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa  
  pressureSensor.setOversampleRate(7); // Set Oversample to the recommended 128
  pressureSensor.enableEventFlags(); // Enable all three pressure and temp event flags 
  
}

void loop()
{

  /* //////////////////////////////////////////
   * //////////////////////////////////////////
      Pressure
   * //////////////////////////////////////////
  * //////////////////////////////////////////*/
  float altitudeM = pressureSensor.readAltitude();
  Serial.print(" Altitude(m):");
  Serial.println(altitudeM, 2);

  /*float altitudeFt = pressureSensor.readAltitudeFt();
  Serial.print(" Altitude(ft):");
  Serial.println(altitudeFt, 2);*/

  float pressure = pressureSensor.readPressure();
  Serial.print(" Pressure(Pa):");
  Serial.println(pressure, 2);

  float temperatureC = pressureSensor.readTemp();
  Serial.print(" Temp(c):");
  Serial.println(temperatureC, 2);

  float temperatureF = pressureSensor.readTempF();
  Serial.print(" Temp(f):");
  Serial.println(temperatureF, 2);

  ////////////////////////////////////////////  
  Serial.println("------------------------\n");
  delay(5000);  
}
