
/*************
sed library            Version Path                                                                          #include <arduino.h>
Rtc by Makuna           2.3.6   C:\Arduino\User\libraries\Rtc_by_Makuna                                       
PubSubClient            2.7     C:\Arduino\User\libraries\PubSubClient                                        
Ethernet                2.0.2   C:\Arduino\User\libraries\Ethernet                                            
WiFi                    2.0.0   C:\Arduino\Data\packages\esp32\hardware\esp32\2.0.7\libraries\WiFi            
Wire                    2.0.0   C:\Arduino\Data\packages\esp32\hardware\esp32\2.0.7\libraries\Wire            
Adafruit GFX Library    1.11.5  C:\Arduino\User\libraries\Adafruit_GFX_Library                                
Adafruit BusIO          1.14.1  C:\Arduino\User\libraries\Adafruit_BusIO                                      
SPI                     2.0.0   C:\Arduino\Data\packages\esp32\hardware\esp32\2.0.7\libraries\SPI             
Adafruit SSD1306        2.5.7   C:\Arduino\User\libraries\Adafruit_SSD1306                                    
EEPROM                  2.0.0   C:\Arduino\Data\packages\esp32\hardware\esp32\2.0.7\libraries\EEPROM          
ESP8266 Influxdb        3.13.1  C:\Arduino\User\libraries\ESP8266_Influxdb                                    
HTTPClient              2.0.0   C:\Arduino\Data\packages\esp32\hardware\esp32\2.0.7\libraries\HTTPClient      
WiFiClientSecure        2.0.0   C:\Arduino\Data\packages\esp32\hardware\esp32\2.0.7\libraries\WiFiClientSecure
DHT sensor library      1.4.3   C:\Arduino\User\libraries\DHT_sensor_library                                  
Adafruit Unified Sensor 1.1.9   C:\Arduino\User\libraries\Adafruit_Unified_Sensor
WiFiManager     https://github.com/tzapu/WiFiManager
****************/

//#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define DS1302 1

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
#include "base64.hpp"
#include <SPI.h>
#include <Wire.h>
#include <Time.h>

/*
extern "C" {
#include "crypto/base64.h"
}*/

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
 
 //WiFiManager wm;

 //EthernetClient ethClient;
 WiFiClient wifiClient;
 PubSubClient mqttClient(wifiClient);


/****************************************************************
 * WiFi Manager
 ****************************************************************/



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

 const int Relays[] = { RELAY1, RELAY2 };

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

  Serial.print(F("\r\n\r\n\tStarting TermoSifao Version 1.0\r\n\r\n"));


  setupSensors();

  
  //wm.resetSettings();

/*
  if (!wm.autoConnect("AutoConnectAP", "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(300000);
  }

  Serial.println("Auto connect ok");
  */
  
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
  
  //setupMQTT();

  showMemoryUsage();

  setupWebServer();
  
  

 


  //setupInfluxDB();

  log(F("Setup is done.\n"));

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
