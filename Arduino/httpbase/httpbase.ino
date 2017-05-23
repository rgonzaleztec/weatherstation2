//Libraries
#include <Ethernet.h>

/* //////////////////////////////////////////
* //////////////////////////////////////////
  Ethernet config
* //////////////////////////////////////////
* //////////////////////////////////////////*/
byte mac[] = { 
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "192.168.1.14";
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


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }

  /* //////////////////////////////////////////
  * //////////////////////////////////////////
    Wind Speed
  * //////////////////////////////////////////
  * //////////////////////////////////////////*/
  pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  attachInterrupt(1, wspeedIRQ, FALLING);


/* //////////////////////////////////////////
* //////////////////////////////////////////
  Ethernet config
* //////////////////////////////////////////
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


void loop() {

  windspeedmph = get_wind_speed();
  Serial.print(",windspeedmph=");
  Serial.println(windspeedmph, 1);

/* //////////////////////////////////////////
* //////////////////////////////////////////
  Ethernet config
* //////////////////////////////////////////
* //////////////////////////////////////////*/
/*
  float a= 1.00;
  String data = (String) a; 

  if (client.connect(server, 8080)) {
    //Serial.println("connected");    
    client.println("POST /postData/"+data+" HTTP/1.1");
    //client.println("POST /postData/1 HTTP/1.1");
    client.println("Connection: close");
    client.println();
    Serial.println("Data send to API OK");
    Serial.println("------------------------\n");  

  } 
  else {
    Serial.println("connection failed");
  }  
  resetEthernet();
  */
  
  delay(5000);
}
