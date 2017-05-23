/*
  Arduino Wheater Station v 1.0
  Marcos Rodríguez Ovares 2016
*/

//Ethernet
#include <Ethernet.h>
//Pressure
#include <SparkFunMPL3115A2.h>
#include <Wire.h>

/* //////////////////////////////////////////
  Ethernet config and instance
* //////////////////////////////////////////*/
byte mac[] = { 
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//char server[] = "192.168.1.14";
char server[] = "172.24.181.99";
IPAddress ip(192, 168, 1, 15);
EthernetClient client;

/* //////////////////////////////////////////
  Pressure instance
* //////////////////////////////////////////*/
MPL3115A2 pressureSensor; //Pressure Sernsor
String altitudeMString;
String altitudeFtString;
String pressureString;
String temperatureCString;
String temperatureFString;

//Setup
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }

/* //////////////////////////////////////////
    Pressure Setup
* //////////////////////////////////////////*/
  pressureSensor.begin(); // Get sensor online
  // Configure the sensor
  //pressureSensor.setModeAltimeter(); // Measure altitude above sea level in meters
  pressureSensor.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa  
  pressureSensor.setOversampleRate(7); // Set Oversample to the recommended 128
  pressureSensor.enableEventFlags(); // Enable all three pressure and temp event flags 

/* //////////////////////////////////////////
  Ethernet Setup
* //////////////////////////////////////////*/
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  delay(1000);
  
}
void resetEthernet() {
 client.stop();
 //Ethernet.begin(mac);
}

/*
  Main loop
*/
void loop() {

  /* //////////////////////////////////////////
      Pressure
  * //////////////////////////////////////////*/
  float altitudeM = pressureSensor.readAltitude();
  altitudeMString = (String) altitudeM;
  float altitudeFt = pressureSensor.readAltitudeFt();
  altitudeFtString = (String) altitudeFt;
  float pressure = pressureSensor.readPressure();
  pressureString = (String) pressure;
  float temperatureC = pressureSensor.readTemp();
  temperatureCString = (String) temperatureC;  
  float temperatureF = pressureSensor.readTempF();
  temperatureFString = (String) temperatureF;

/* //////////////////////////////////////////
  HTTP POST
* //////////////////////////////////////////*/
  if (client.connect(server, 6060)) {
    
    client.println("POST /postPressure/"+
        altitudeMString +"/"+ altitudeFtString +"/"+ pressureString +"/"+ temperatureCString +"/"+ temperatureFString +
        +" HTTP/1.1");
    client.println("Connection: close");
    client.println();
    Serial.println("Data send to API OK");
    Serial.println("------------------------\n");     
  } 
  else {
    Serial.println("connection failed");
  }  
  resetEthernet();
  
  delay(5000);
}
