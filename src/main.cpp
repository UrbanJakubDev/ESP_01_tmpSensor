// Teploměr a vlhkoměr AM2320

// připojení potřebných knihoven
#include <Wire.h>
#include <AM2320/AM2320.h>
#include <thingspeak-arduino-master/src/ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <WiFiUdp.h>
#include <NTPClient-master/NTPClient.h>
// inicializace modulu z knihovny

#define DHTPIN 2
#define typDHT11 DHT11 
AM2320 senzor;
DHT mojeDHT(DHTPIN, typDHT11);

//----------------  Fill in your credentails   ---------------------
char ssid[] = "wifiname";             // your network SSID (name) 
char pass[] = "pass";         // your network password 
unsigned long myChannelNumber = 0;  // Replace the 0 with your channel number
const char * myWriteAPIKey = "maykey";    // Paste your ThingSpeak Write API Key between the quotes 
//------------------------------------------------------------------
float teplota;
float vlhkost;
const long utcOffsetInSeconds = 3600;

WiFiClient  client;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  // zahájení komunikace po sériové lince
  // rychlostí 9600 baud
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  mojeDHT.begin();
  timeClient.begin();

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
}

void mereni(){
    float tep = mojeDHT.readTemperature() - 1.5;
  float vlh = mojeDHT.readHumidity();
  ThingSpeak.setField(1, tep);
  ThingSpeak.setField(2, vlh);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

void loop() {

  timeClient.update();
  int min = timeClient.getMinutes();
  Serial.print(min);

  switch (min)
  {
  case 0:
    mereni();
    break;

  case 15:
    mereni();
    break;

   case 30:
    mereni();
    break;

  case 45:
    mereni();
    break; 
  }
  

  delay(20000);
}

