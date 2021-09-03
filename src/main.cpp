// Teploměr a vlhkoměr AM2320

// připojení potřebných knihoven
#include <Wire.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoOTA.h>

// inicializace modulu z knihovny
#define DHTPIN 2
#define DHTType DHT11

DHT mojeDHT(DHTPIN, DHTType);

//----------------  Fill in your credentails   ---------------------
const char* ssid = "internet50";                   // your network SSID (name)
const char* pass = "12041996Au";              // your network password
unsigned long myChannelNumber = 318641;         // Replace the 0 with your channel number
const char *myWriteAPIKey = "13IVW9HLQW68FKHQ"; // Paste your ThingSpeak Write API Key between the quotes
//------------------------------------------------------------------
float temp;
float lastTemp = 0;
float hum;
float lastHum = 0;


WiFiClient client;

void setup()
{

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  mojeDHT.begin();

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }


  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void mereni()
{
  temp = mojeDHT.readTemperature() - 1.5;
  hum = mojeDHT.readHumidity();

  if (temp != lastTemp)
  {
    lastTemp = temp;
    ThingSpeak.setField(1,lastTemp);
  }
  if (hum != lastHum)
  {
    lastHum = hum;
    ThingSpeak.setField(2,lastHum);
  }

  if (temp != lastTemp || hum != lastHum)
  {
    ThingSpeak.writeFields(myChannelNumber,myWriteAPIKey);
  }
}

void loop()
{
  ArduinoOTA.handle();
  mereni();
  delay(1000*60*20);
}
