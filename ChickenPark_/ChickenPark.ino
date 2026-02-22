// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, modified for integer use, public domain

#define DS1307 1

#include <ThreeWire.h>  
#ifdef DS1302
#include <RtcDS1302.h>
#endif
#include <arduino.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "EEPROM.h"
#include <Base64.h>
#include <SPI.h>
#include <Wire.h>
#include <Time.h>
#include "RTClib.h"

/*
extern "C" {
#include "crypto/base64.h"
}*/

int log(const __FlashStringHelper* flashStr, ...);

 String msgStatus;
 
/****************************************************************
 * Serial buffer input data
 *************************************************************/
 const int INPUTBUFFERSIZE = 200;
 char inputBuffer[INPUTBUFFERSIZE];   // a String to hold incoming data
 int inputBufferPos = 0;
 boolean inputBufferComplete = false;      // whether the string is complete
 boolean ntpSync = false; //Ntp was sync

/****************************************************************
 * Serial buffer input data
 ****************************************************************/

 unsigned long lastInternetCheck = 0;
 unsigned long lastCheck = 0;
 unsigned long lastNtpCheck = 0;
 unsigned long lastClockCheckOk = 0;
 unsigned long lastClockSetup = 0;
 unsigned long lastMqttCheck = 0;
 unsigned long lastMqttStatusUpdate = 0;
 unsigned long lastDisplayStatus = 0;

 unsigned long lastSensorsCheck = 0;
 

 //EthernetClient ethClient;
 WiFiClient wifiClient;
 PubSubClient mqttClient(wifiClient);

/****************************************************************
 * WiFi Router
 ****************************************************************/

 bool internetConnectionState = false;
 unsigned long lastConnectionChange = 0;
 unsigned long internetResetAfter = 300000;
 unsigned long internetCheckPeriod = 300000;
 unsigned long DisplayStatusCheckPeriod = 1000;
 unsigned long SensorsCheckPeriod = 20000;

 /****************************************************************
 * Relay specific data
 ****************************************************************/

const int EEPRomVersison = 4;
 
#define RELAY1  26
#define RELAY2  25
#define RELAY3  17
#define RELAY4  16
#define RELAY5  13
#define RELAY6  12
#define RELAY7  14
#define RELAY8  27

#define INPUTRELAY  39

 bool inputRelayState = HIGH;
 bool lastReadInputRelayState = HIGH;
 unsigned long lastInputRelayDebounceTime = 0;
 unsigned long debounceDelay = 50;

 const int Relays[] = { RELAY1, RELAY2, RELAY3, RELAY4, RELAY5, RELAY6, RELAY7, RELAY8 };

 const int nRelays = sizeof(Relays)/sizeof(int);

 struct
 {
  unsigned long duration;
  unsigned long lastDuration;
  char lastOn[20];
  char relayName[64];
  int  maxDuration;
  bool enabled;
 } relayStatus[nRelays];

 struct EEPromRelay
 {
  bool enabled;
  char relayName[32];
  int  maxDuration;
 } eepromRelay;

/****************************************************************
 * Scheduler specific data
 ****************************************************************/
 
 const int maxSchedules PROGMEM = 16;
 struct
 {
   bool enabled;
   char lastCheck[20];
   char cronString[128];
   char lastRun[20];
 } schedule[maxSchedules];


 struct EEPromSchedule
 {
  bool enabled;
  char cronString[128];
 } eepromSchedule;

 const int FLASH_MEM_ALOC_SIZE = 1 + maxSchedules * sizeof(EEPromSchedule) + nRelays*sizeof(EEPromSchedule);

#define countof(a) (sizeof(a) / sizeof(a[0]))

/********************************************************************/
 #define LED_BUILTIN 2
 char dateTime[20];

/****************************************************************
 * Mqtt data
 ****************************************************************/

 const char  mqttId[] = "ChickenPark";


/****************************************************************
 * Other variables
 ****************************************************************/
 
int flag = true;

/****************************************************************
 * Setup
 ****************************************************************/


void setup()
{
	
  lastClockCheckOk = millis();
  pinMode(LED_BUILTIN, OUTPUT);
  clearInputBuffer();
  unsigned long lastNtpCheck = millis();
  // Initilize hardware:
  Serial.begin(115200);

  Serial.print(F("\r\n\r\n\tStarting ChickenPark Version 1.1\r\n\r\n"));

  SetupDisplay();
  DisplayStatus();

  SetupRTC();
    
  for(int i=0; i< nRelays; i++)
   {
     RelayCommand(i+1, 0, 0);
     pinMode(Relays[i], OUTPUT);
  }
    
  // Connect to the WiFi network (see function below loop)
  connectToWiFi();

  setupNtp();
  
  setupMQTT();

  showMemoryUsage();

  setupWebServer();
  
  log(F("Setup is done.\n"));

  /* Setup dht22   */
 
  setupSensors();

  setupInfluxDB();

}

/****************************************************************
 * Loop
 ****************************************************************/

void loop() {

  delay(10);

  /*  Input command */
    
   char buffer[200];
   
   serialEvent();
   
   if (inputBufferComplete)
   {
     char dateTime[20];
     
     Serial.print(getDateTime());
     Serial.print(F("\tInput data '"));
     Serial.print(inputBuffer);
     Serial.print(F("'.\n"));

     processCommand(inputBuffer);
     clearInputBuffer();
   }

  /*  Schedules */
  
  if (ClockOk())
  {
   char cmd[200]; 
   for(int i=0; i< maxSchedules; i++)
   {
     if (schedule[i].enabled)
     {
      //Serial.printf("%s\tFound schedule %d.\n", getDateTime(), i);
      if (schedTimeMatch(i, cmd))
      {
       if (strcmp(cmd,"") != 0)
        {
         sprintf(buffer, "%s\tFound schedule cmd (%s).\n", getDateTime(), cmd);
         Serial.print(buffer);
         processCommand(cmd);
        }
      }
     }
   }
  }

  /*  Global checkup */
     
  if (millis() > (lastCheck + 30000))
   {
     lastCheck = millis();
     sprintf(buffer, "%s\tCheckup start.\n", getDateTime());
     Serial.print(buffer);
     connectToWiFi();
     sprintf(buffer, "%s\tChecking mqtt.\n", getDateTime());
     Serial.print(buffer);
     setupMQTT();
     setClockFromNtp();
     sprintf(buffer, "%s\tCheckup ended.\n", getDateTime()); 
     Serial.print(buffer);
   }

   /*  Mqtt */
     
  if (millis() > (lastMqttStatusUpdate + 60000))
   {
     lastMqttStatusUpdate = millis(); 
     MqttStatus();
   }

   if (mqttClient.connected())
    {
     mqttClient.loop();
    }

  /*  Relays */

   VerifyRelaysTimeout();

  /*  InternetCheck */
  
  if (millis() > (lastInternetCheck + internetCheckPeriod))
   {
     checkInternet();
     lastInternetCheck = millis();
   }

  /*  WebServer */
  
  WebServer();

  /*  Lcd */
  if (millis() > (lastDisplayStatus + DisplayStatusCheckPeriod))
   {
     DisplayStatus();
     lastDisplayStatus = millis();
   }
   
  /*  Sensors */
  if (millis() > (lastSensorsCheck + SensorsCheckPeriod))
   {
     lastSensorsCheck = millis();
     loopSensors();
   }
}
