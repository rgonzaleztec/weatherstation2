/*
  Arduino Wheater Station v 1.0
  Marcos Rodríguez Ovares 2016
*/

String idStation = "developmentStation";

//Ethernet
#include <Ethernet.h>
//Pressure
#include <SparkFunMPL3115A2.h>
#include <Wire.h>
//Humidity
#include <SparkFunHTU21D.h>

/*
 Status leds
*/
const byte STAT1 = 7;
const byte STAT2 = 8;

/* //////////////////////////////////////////
  Ethernet config and instance
* //////////////////////////////////////////*/
byte mac[] = { 
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//char server[] = "172.24.180.59"; //TEC SIRZEE
//int port = 6060; //TEC SIRZEE
char server[] = "54.218.29.64"; //Amazon Web Services
int port = 80; ////Amazon Web Services
IPAddress ip(192, 168, 1, 15);
EthernetClient client;

/* //////////////////////////////////////////
* //////////////////////////////////////////
  Wind Speed
* //////////////////////////////////////////
* //////////////////////////////////////////*/
const byte WSPEED = 3;
long lastWindCheck = 0;
volatile long lastWindIRQ = 0;
volatile byte windClicks = 0;
float windspeedmph; // [mph instantaneous wind speed]
String windspeedmphString;

/* //////////////////////////////////////////
  Wind direction PIN and instance
* //////////////////////////////////////////*/
String winddirString;
const byte WDIR = A0;
int winddir; // [0-360]

/* //////////////////////////////////////////
  lightning configuration
* //////////////////////////////////////////*/
#ifdef ENABLE_LIGHTNING
const byte LIGHTNING_IRQ = 4; //Not really an interrupt pin, we will catch it in software
const byte slaveSelectPin = 10; //SS for AS3935
#endif
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
String light_lvlString;

/*///////////////////////////////////////////////
 * rain instance
 *//////////////////////
const byte RAIN = 2;
long lastSecond; //The millis counter to see when a second rolls by
byte seconds; //When it hits 60, increase the current minute
byte seconds_2m; //Keeps track of the "wind speed/dir avg" over last 2 minutes array of data
byte minutes; //Keeps track of where we are in various arrays of data
byte minutes_10m; //Keeps track of where we are in wind gust/dir over last 10 minutes array of data


volatile float rainHour[60]; //60 floating numbers to keep track of 60 minutes of rain

float rainin = 0; // [rain inches over the past hour)] -- the accumulated rainfall in the past 60 min

volatile float dailyrainin = 0; // [rain inches so far today in local time]

volatile unsigned long raintime, rainlast, raininterval, rain;

/* //////////////////////////////////////////
  How to get light level
* //////////////////////////////////////////*/
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

/* //////////////////////////////////////////
  Pressure instance
* //////////////////////////////////////////*/
MPL3115A2 pressureSensor; //Pressure Sernsor
String altitudeMString;
String altitudeFtString;
String pressureString;
String temperatureCString;
String temperatureFString;
float altitudeM;
float altitudeFt;
float pressure;
float temperatureC;
float temperatureF;

/* //////////////////////////////////////////
  Humidity instance
* //////////////////////////////////////////*/
HTU21D humiditySensor; 
String humidityString;
float humidity;

//Setup
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  /*
  * Status leds output mode
  */
  pinMode(STAT1, OUTPUT);
  pinMode(STAT2, OUTPUT);
  /* //////////////////////////////////////////
      Wind direction Setup
  * //////////////////////////////////////////*/
  pinMode(WDIR, INPUT);

  /* //////////////////////////////////////////
      lightning Setup
  * //////////////////////////////////////////*/
  pinMode(LIGHT, INPUT);
  #ifdef ENABLE_LIGHTNING
    startLightning(); //Init the lighting sensor
  #endif

/* //////////////////////////////////////////
    Pressure Setup
* //////////////////////////////////////////*/
  pressureSensor.begin(); // Get sensor online
  // Configure the sensor
  pressureSensor.setModeAltimeter(); // Measure altitude above sea level in meters
  //pressureSensor.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa  
  pressureSensor.setOversampleRate(7); // Set Oversample to the recommended 128
  pressureSensor.enableEventFlags(); // Enable all three pressure and temp event flags 

  /* //////////////////////////////////////////
      Humidity Setup
  * //////////////////////////////////////////*/
  humiditySensor.begin();

  /* //////////////////////////////////////////
  * //////////////////////////////////////////
    Wind Speed
  * //////////////////////////////////////////
  * //////////////////////////////////////////*/
  pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  attachInterrupt(1, wspeedIRQ, FALLING);

  /*/////////////////////////////////
   * rains setup
   */////////////////
  pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor
  seconds = 0;
  lastSecond = millis();
  // attach external interrupt pins to IRQ functions
  attachInterrupt(0, rainIRQ, FALLING);
  /* //////////////////////////////////////////
    Ethernet Setup
  * //////////////////////////////////////////*/
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  interrupts();
  delay(1000);
  digitalWrite(STAT2, HIGH);   
}
void resetEthernet() {
 client.stop();
 //Ethernet.begin(mac);
}


/* //////////////////////////////////////////
* //////////////////////////////////////////
  Wind Speed
* //////////////////////////////////////////
* //////////////////////////////////////////*/
//Returns the instataneous wind speed
float get_wind_speed()
{
  float deltaTime = millis() - lastWindCheck; //750ms

  deltaTime /= 1000.0; //Covert to seconds

  float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

  windClicks = 0; //Reset and start watching for new wind
  lastWindCheck = millis();

  windSpeed *= 1.492; //4 * 1.492 = 5.968MPH

  return(windSpeed);
}


/*//////////////////////////////////////////
 * RAIN
 */////////////////////////
//Interrupt routines (these are called by the hardware interrupts, not by the main code)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void rainIRQ()
// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
{
    raintime = millis(); // grab current time
    raininterval = raintime - rainlast; // calculate interval between this and last event

    if (raininterval > 10) // ignore switch-bounce glitches less than 10mS after initial edge
    {
        dailyrainin += 0.011; //Each dump is 0.011" of water
        rainHour[minutes] += 0.011; //Increase this minute's amount of rain

        rainlast = raintime; // set up for next event
    }
}

void calcWeather()
{

    //Total rainfall for the day is calculated within the interrupt
    //Calculate amount of rainfall for the last 60 minutes
    rainin = 0;
    for(int i = 0 ; i < 60 ; i++)
        rainin += rainHour[i];
}

/* //////////////////////////////////////////
* //////////////////////////////////////////
  Wind Speed
* //////////////////////////////////////////
* //////////////////////////////////////////*/
void wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
{
  if (millis() - lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
  {
    lastWindIRQ = millis(); //Grab the current time
    windClicks++; //There is 1.492MPH for each click per second.
  }
}

/* //////////////////////////////////////////
  How to read a PIN
* //////////////////////////////////////////*/
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);
}

/* //////////////////////////////////////////
  How to calc wind direction
* //////////////////////////////////////////*/
int get_wind_direction()
// read the wind direction sensor, return heading in degrees
{
  unsigned int adc;

  adc = averageAnalogRead(WDIR); // get the current reading from the sensor

  // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
  // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
  // Note that these are not in compass degree order! See Weather Meters datasheet for more information.

  if (adc < 380) return (113);
  if (adc < 393) return (68);
  if (adc < 414) return (90);
  if (adc < 456) return (158);
  if (adc < 508) return (135);
  if (adc < 551) return (203);
  if (adc < 615) return (180);
  if (adc < 680) return (23);
  if (adc < 746) return (45);
  if (adc < 801) return (248);
  if (adc < 833) return (225);
  if (adc < 878) return (338);
  if (adc < 913) return (0);
  if (adc < 940) return (293);
  if (adc < 967) return (315);
  if (adc < 990) return (270);
  return (-1); // error, disconnected?
}

/*
  Main loop
*/
void loop() {
  digitalWrite(STAT1, LOW);   

  /* //////////////////////////////////////////
      Pressure
  * //////////////////////////////////////////*/
  altitudeM = pressureSensor.readAltitude();
  altitudeMString = (String) altitudeM;
  altitudeFt = pressureSensor.readAltitudeFt();
  altitudeFtString = (String) altitudeFt;
  pressure = pressureSensor.readPressure();
  pressureString = (String) pressure;
  temperatureC = pressureSensor.readTemp();
  temperatureCString = (String) temperatureC;  
  temperatureF = pressureSensor.readTempF();
  temperatureFString = (String) temperatureF;

  /* //////////////////////////////////////////
      Humidity
  * //////////////////////////////////////////*/
  humidity = humiditySensor.readHumidity();
  humidityString = (String) humidity;

  /* //////////////////////////////////////////
      light level
  * //////////////////////////////////////////*/
  light_lvl = get_light_level();
  light_lvlString = (String)light_lvl;

  /* //////////////////////////////////////////
      Wind direction
  * //////////////////////////////////////////*/
  winddir = get_wind_direction();
  winddirString = (String)winddir;

  /* //////////////////////////////////////////
       Wind direction
   * //////////////////////////////////////////*/
  windspeedmph = get_wind_speed();
  windspeedmphString = (String) windspeedmph;

  /*/////////////////////////////////
   * RAIN 
   */////////////////////

lastSecond += 1000;
  if(++seconds > 59)
      {
            seconds = 0;

            if(++minutes > 59) minutes = 0;
            if(++minutes_10m > 9) minutes_10m = 0;

            rainHour[minutes] = 0; //Zero out this minute's rainfall amount
        }
   
/* //////////////////////////////////////////
  HTTP POST
* //////////////////////////////////////////*/
  if (client.connect(server, port)) {
    
    client.println("POST /postMeasure/"+ idStation + "/" +
        altitudeMString +"/"+ altitudeFtString +"/"+ pressureString +"/"+ temperatureCString +"/"+ temperatureFString + "/"+
        humidityString + "/" + light_lvlString + "/" + winddirString + "/" + windspeedmphString+ "/" + rainin+ "/" + dailyrainin
        +" HTTP/1.1");
    client.println("Connection: close");
    client.println();
    Serial.println("Data send to API OK");
    Serial.println("------------------------\n");     
    digitalWrite(STAT1, HIGH);   
  } 
  else {
    Serial.println("connection failed");
  }  
  Serial.println(idStation + "/" +
        altitudeMString +"/"+ altitudeFtString +"/"+ pressureString +"/"+ temperatureCString +"/"+ temperatureFString + "/"+
        humidityString + "/" + light_lvlString + "/" + winddirString + "/" + windspeedmphString);     
    Serial.println();
    Serial.print(",rainin=");
    Serial.print(rainin, 2);
    Serial.print(",dailyrainin=");
    Serial.print(dailyrainin, 2);
    Serial.println("#");
  resetEthernet();

  
  delay(10000);
  
}
