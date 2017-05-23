/* //////////////////////////////////////////
  lightning configuration
* //////////////////////////////////////////*/
#ifdef ENABLE_LIGHTNING
const byte LIGHTNING_IRQ = 4; //Not really an interrupt pin, we will catch it in software
const byte slaveSelectPin = 10; //SS for AS3935
#endif
// analog I/O pins
const byte WDIR = A0;
const byte LIGHT = A1;
const byte REFERENCE_3V3 = A3;
#ifdef ENABLE_LIGHTNING
#include "AS3935.h" //Lighting detector
#include <SPI.h> //Needed for lighting sensor
byte SPItransfer(byte sendByte);
AS3935 AS3935(SPItransfer, slaveSelectPin, LIGHTNING_IRQ);
#endif


/* //////////////////////////////////////////
  lightning instance
* //////////////////////////////////////////*/
float light_lvl = 0.72;


//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;
  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);
}

float get_light_level()
{
  float operatingVoltage = averageAnalogRead(REFERENCE_3V3);
  float lightSensor = averageAnalogRead(LIGHT);
  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
  lightSensor *= operatingVoltage;
  return(lightSensor);
}


#ifdef ENABLE_LIGHTNING
void startLightning(void)
{
  pinMode(slaveSelectPin, OUTPUT); // set the slaveSelectPin as an output:
  pinMode(LIGHTNING_IRQ, INPUT_PULLUP); //Set IRQ pin as input
  SPI.begin(); //Start SPI
  SPI.setDataMode(SPI_MODE1); // NB! chip uses SPI MODE1
  SPI.setClockDivider(SPI_CLOCK_DIV16); //Uno 16MHz / 16 = 1MHz
  SPI.setBitOrder(MSBFIRST); // and chip is MSB first
  lightning_init(); //Setup the values for the sensor
  //Serial.println("Lightning sensor online");
}

void lightning_init()
{
  AS3935.reset(); // reset all internal register values to defaults
  // if lightning detector can not tune tank circuit to required tolerance,
  // calibration function will return false
  if(!AS3935.calibrate())
  {
    Serial.println("Tuning out of range, check your wiring, your sensor and make sure physics laws have not changed!");
  }
  AS3935.setOutdoors(); //The weather station is outdoors
  AS3935.enableDisturbers(); //We want to know if a man-made event happens
  AS3935.setNoiseFloor(3); //See table 16 of the AS3935 datasheet. 4-6 works. This was found through experimentation.
  //printAS3935Registers();
}

byte SPItransfer(byte sendByte)
{
  return SPI.transfer(sendByte);
}
#endif

void setup()
{
  Serial.begin(9600);  // Start serial for output

  /* //////////////////////////////////////////
      lightning Setup
  * //////////////////////////////////////////*/
  pinMode(LIGHT, INPUT);
  #ifdef ENABLE_LIGHTNING
    startLightning(); //Init the lighting sensor
  #endif
  
}

void loop()
{

  /* //////////////////////////////////////////
      light level
  * //////////////////////////////////////////*/
  light_lvl = get_light_level();
  Serial.print("light_lvl=");
  Serial.print(light_lvl, 2);

  ////////////////////////////////////////////  
  Serial.println("\n------------------------\n");
  delay(5000);  
}
