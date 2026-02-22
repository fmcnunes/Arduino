# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"

/*************

sed library            Version Path                                                                          

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
# 22 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
//#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager



# 27 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2

# 29 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2

# 31 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 32 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 33 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 34 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 35 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 36 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 37 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 38 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 39 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 40 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 41 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 42 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2
# 43 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino" 2

/*

extern "C" {

#include "crypto/base64.h"

}*/
# 49 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
 String msgStatus;

/****************************************************************

 * Serial buffer input data

 *************************************************************/
# 54 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
 const int INPUTBUFFERSIZE = 200;
 char inputBuffer[INPUTBUFFERSIZE]; // a String to hold incoming data
 int inputBufferPos = 0;
 boolean inputBufferComplete = false; // whether the string is complete
 boolean ntpSync = false; //Ntp was sync

/****************************************************************

 * Serial buffer input data

 ****************************************************************/
# 64 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
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
# 92 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
 bool internetConnectionState = false;
 unsigned long lastConnectionChange = 0;
 unsigned long internetResetAfter = 300000;
 unsigned long internetCheckPeriod = 300000;
 unsigned long DisplayStatusCheckPeriod = 1000;
 unsigned long SensorsCheckPeriod = 20000;

 /****************************************************************

 * Relay specific data

 ****************************************************************/
# 103 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
const int EEPRomVersison = 4;
# 116 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
 bool inputRelayState = 0x1;
 bool lastReadInputRelayState = 0x1;
 unsigned long lastInputRelayDebounceTime = 0;
 unsigned long debounceDelay = 50;

 const int Relays[] = { 26, 25, 17, 16, 13, 12, 14, 27 };

 const int nRelays = sizeof(Relays)/sizeof(int);

 struct
 {
  unsigned long duration;
  unsigned long lastDuration;
  char lastOn[20];
  char relayName[64];
  int maxDuration;
  bool enabled;
 } relayStatus[nRelays];

 struct EEPromRelay
 {
  bool enabled;
  char relayName[32];
  int maxDuration;
 } eepromRelay;

/****************************************************************

 * Scheduler specific data

 ****************************************************************/
# 146 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
 const int maxSchedules = 16;
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



/********************************************************************/

 char dateTime[20];

/****************************************************************

 * Mqtt data

 ****************************************************************/
# 174 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
 const char mqttId[] = "ChickenPark";


/****************************************************************

 * Other variables

 ****************************************************************/
# 181 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
int flag = true;

/****************************************************************

 * Setup

 ****************************************************************/
void setup()
{

  lastClockCheckOk = millis();
  pinMode(2, 0x03);
  clearInputBuffer();
  unsigned long lastNtpCheck = millis();
  // Initilize hardware:
  Serial.begin(115200);

  Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("\r\n\r\n\tStarting ChickenPark Version 1.2\r\n\r\n")))));

  //wm.resetSettings();

/*

  if (!wm.autoConnect("AutoConnectAP", "password")) {

    Serial.println("failed to connect and hit timeout");

    delay(300000);

  }



  Serial.println("Auto connect ok");

  */
# 211 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
  SetupDisplay();
  DisplayStatus();

  SetupRTC();

  for(int i=0; i< nRelays; i++)
   {
     RelayCommand(i+1, 0, 0);
     pinMode(Relays[i], 0x03);
  }

  // Connect to the WiFi network (see function below loop)
  connectToWiFi();

  setupNtp();

  setupMQTT();

  showMemoryUsage();

  setupWebServer();



  /* Setup dht22   */

  setupSensors();

  setupInfluxDB();

  log(((reinterpret_cast<const __FlashStringHelper *>(("Setup is done.\n")))));

}

/****************************************************************

 * Loop

 ****************************************************************/
# 249 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\ChickenPark.ino"
void loop() {

  delay(10);

  /*  Input command */

   char buffer[200];

   serialEvent();

   if (inputBufferComplete)
   {
     char dateTime[20];

     Serial.print(getDateTime());
     Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("\tInput data '")))));
     Serial.print(inputBuffer);
     Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("'.\n")))));

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
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Commands.ino"

void processCommand(char *cmdBuffer)
{
  int relay=-1;
  int schedId=-1;
  int duration=-1;
  char op[200];
  char buffer[200];
  int y;
  int mm;
  int d;
  int h;
  int m;
  int s;
  char strMinute[50];
  char strHour[50];
  char strWeekDay[50];
  char strDay[50];
  char strMonth[50];
  int freq;

  sprintf(buffer,"%s\tProcessing command '%s'", getDateTime(), cmdBuffer);
  Serial.println(buffer);
  mqttSendLog(buffer);

  if (! isValidCmd(cmdBuffer))
  {
    sprintf(buffer,"%s\tCommand '%s' is invalid.", getDateTime(), cmdBuffer);
    mqttSendLog(buffer);
    Serial.println(buffer);
    return;
  }

  if (sscanf(cmdBuffer,"sched,%d,change,\"%[^\"]s",&schedId,op) == 2)
  {
    strcpy(schedule[schedId-1].cronString,op);
    writeSchedulerToEeprom(schedId-1);
    return;
  }

  if (sscanf(cmdBuffer,"datetime,%d-%d-%d,%d:%d:%d",&y,&mm,&d,&h,&m,&s) == 6)
  {
    setDateTime(y,mm,d,h,m,s);
    return;
  }

  if (sscanf(cmdBuffer,"sched,%d,%[^\n]s",&schedId,op) == 2)
  {

    if (schedId < 1 || schedId > maxSchedules) return;
    if (strcmp(op,"enable") == 0)
    {
      schedule[schedId-1].enabled = true;
      writeSchedulerToEeprom(schedId-1);
    }
    if (strcmp(op,"disable") == 0)
    {
      schedule[schedId-1].enabled = false;
      writeSchedulerToEeprom(schedId-1);
    }
    return;
  }

  if (sscanf(cmdBuffer,"relay,%d,name,%[^\n]s",&relay,buffer) == 2)
  {
    RelaySetName(relay, buffer);
    return ;
  }

 if (sscanf(cmdBuffer,"relay,%d,on,%d",&relay,&duration) == 2)
  {
    RelayCommand(relay, 1, duration);
    return;
  }

 if (sscanf(cmdBuffer,"relay,%d,off",&relay) == 1)
  {
    RelayCommand(relay, 0, 0);
    return;
  }


  if (strcmp(cmdBuffer,"reload") == 0)
   {
    Reload();
    return;
   }

  if (strcmp(cmdBuffer,"ntpsync") == 0)
   {
    setClockFromNtp();
    return;
   }

  if (strcmp(cmdBuffer,"help") == 0 || strcmp(cmdBuffer,"h") == 0)
   {
    help();
    return;
   }

  if (strcmp(cmdBuffer,"status") == 0 || strcmp(cmdBuffer,"s") == 0)
  {
    //TestEeprom();
    MqttStatus();
    ShowStatus();
    return;
  }
}

void Reload()
{
  char buffer[200];
  sprintf(buffer,"%s\tReloading.", getDateTime());
  mqttSendLog(buffer);
  Serial.println(buffer);
  esp_restart();
}

/***************************************

 *

 * Help

 *

 **************************************/
# 124 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Commands.ino"
 void help()
 {
  String buff;


    Serial.print(String(((reinterpret_cast<const __FlashStringHelper *>(("---------------------------------- Help ----------------------------------\n"))))));
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> Status                                   --- Device status\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> ntpsync                                   --- Sync time from ntp internet server\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> relay,1,on,4                             --- Activate relay 1 for 4 seconds\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> relay,1,off                              --- Deactivate relay 1\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> reload                                   --- Reload board\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> relay,1,name,<name>                      --- Name for relay 1 set to <name>\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> datetime,yyyy-mm-dd,hh:mm:ss             --- Change RTC date and time\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> sched,1,disable                          --- Disable sched 1\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> sched,1,enable                           --- Enable sched 1\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> sched,1,change,\"* * * * * 1 relay,1,on,30\" --- Change sched 1 <minute 00..60> <hour 0..23> <weekday 0..6> <day 1..31> <month 1..11> <repeat 1..31> <cmd>\n")))));
    Serial.print(buff);
    buff = getDateTime() + String(((reinterpret_cast<const __FlashStringHelper *>(("\tCMD> sched,1,change,\"* * * * * 1 reset\" --- Change sched 1\n")))));
    Serial.print(buff);
    Serial.print("--------------------------------------------------------------------------\n");
 }

/***************************************

 *

 * Display Relay status

 *

 **************************************/
# 160 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Commands.ino"
 void ShowStatus()
 {
  char buffer[512];

  Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\n---------------------------------- Status ----------------------------------")))));

  Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\n")))));

  for(int i=0;i< nRelays; i++)
  {
    if (digitalRead(Relays[i])==0x1)
    {
      sprintf(buffer,"%s\tStatus\t'%s'\tOFF", getDateTime(), relayStatus[i].relayName);
    }
    else
    {
      sprintf(buffer,"%s\tStatus\t'%s'\tON %d sec remaining.", getDateTime(), relayStatus[i].relayName, (int)((relayStatus[i].duration - millis())/1000.0));
    }
    Serial.println(buffer);
  }

  Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\n")))));

  for(int i=0;i< maxSchedules; i++)
  {
    if (schedule[i].enabled)
     sprintf(buffer,"%s\tScheduler\t%d\tEnabled\t%s", getDateTime(), i+1, schedule[i].cronString);
    else
     sprintf(buffer,"%s\tScheduler\t%d\tDisabled\t%s", getDateTime(), i+1, schedule[i].cronString);

    Serial.println(buffer);
  }

  Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\n")))));

  if (readInputRelay())
     sprintf(buffer,"%s\tInput relay\tOFF", getDateTime());
    else
     sprintf(buffer,"%s\tInput relay\tON", getDateTime());

  Serial.println(buffer);

    Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\n")))));

  if (internetConnectionState)
     sprintf(buffer,"%s\tInternet state\tCONNECTED", getDateTime());
    else
     sprintf(buffer,"%s\tInternet state\tDISCONNECTED", getDateTime());

  Serial.println(buffer);

  if (mqttClient.connected())
     sprintf(buffer,"%s\tMqtt\tCONNECTED", getDateTime());
    else
     sprintf(buffer,"%s\tMqtt\tDISCONNECTED", getDateTime());

  Serial.println(buffer);

  Serial.print(String(((reinterpret_cast<const __FlashStringHelper *>(("\n----------------------------------------------------------------------------\n\n"))))));
 }
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Display.ino"



// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

Adafruit_SSD1306 display(128 /* OLED display width, in pixels*/, 64 /* OLED display height, in pixels*/, &Wire, -1 /* Reset pin # (or -1 if sharing Arduino reset pin)*/);


/*

#define NUMFLAKES     10 // Number of snowflakes in the animation example



#define LOGO_HEIGHT   16

#define LOGO_WIDTH    16

static const unsigned char PROGMEM logo_bmp[] =

{ B00000000, B11000000,

  B00000001, B11000000,

  B00000001, B11000000,

  B00000011, B11100000,

  B11110011, B11100000,

  B11111110, B11111000,

  B01111110, B11111111,

  B00110011, B10011111,

  B00011111, B11111100,

  B00001101, B01110000,

  B00011011, B10100000,

  B00111111, B11100000,

  B00111111, B11110000,

  B01111100, B11110000,

  B01110000, B01110000,

  B00000000, B00110000 }; */
# 32 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Display.ino"
static const unsigned char WiFiFull[] = {
  0x00, 0x00, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00, 0x1f, 0xfc, 0x1f, 0xfc, 0x00, 0x00, 0x07, 0xf0,
  0x07, 0xf0, 0x00, 0x00, 0x01, 0xc0, 0x01, 0xc0, 0x00, 0x00,
};


static const unsigned char Rega[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x01, 0x80,
  0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x07, 0xe0,
  0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xfc,
  0x00, 0x10, 0x3f, 0xfc, 0x00, 0x38, 0x3f, 0xfc, 0x00, 0x38, 0x3f, 0xfe, 0x00, 0x7c, 0x3f, 0xfe,
  0x00, 0x7c, 0x2f, 0xfc, 0x00, 0xfe, 0x37, 0xfc, 0x00, 0xfe, 0x13, 0xf8, 0x01, 0xff, 0x0d, 0xf8,
  0x03, 0xff, 0x87, 0xe0, 0x03, 0xff, 0x80, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x07, 0xff, 0xc0, 0x00,
  0x0f, 0xff, 0xe0, 0x00, 0x0f, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xf8, 0x00,
  0x3f, 0xff, 0xf8, 0x00, 0x3f, 0xff, 0xf8, 0x00, 0x7f, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xfc, 0x00,
  0x7f, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xfc, 0x00, 0x6f, 0xff, 0xfc, 0x00, 0x2f, 0xff, 0xf8, 0x00,
  0x37, 0xff, 0xf8, 0x00, 0x33, 0xff, 0xf8, 0x00, 0x19, 0xff, 0xf0, 0x00, 0x0c, 0x7f, 0xe0, 0x00,
  0x07, 0xff, 0xc0, 0x00, 0x03, 0xff, 0x80, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

static const unsigned char Idle[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00,
  0x00, 0x00, 0xf8, 0x00, 0x00, 0x38, 0xf8, 0x00, 0x00, 0x7e, 0xf8, 0x00, 0x00, 0x7f, 0xf8, 0x00,
  0x00, 0x7f, 0xf3, 0x80, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x1f, 0xff, 0xe0, 0x00, 0x3f, 0x7f, 0xe0,
  0x00, 0xfe, 0x3f, 0xc0, 0x01, 0xff, 0x7f, 0x80, 0x01, 0xff, 0xfc, 0x00, 0x01, 0xff, 0xfe, 0x00,
  0x00, 0xf7, 0xff, 0x80, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x0f, 0xf7, 0x80,
  0x00, 0x0f, 0xb3, 0x00, 0x00, 0x07, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x70, 0x30, 0x00,
  0x00, 0xf8, 0x30, 0x00, 0x00, 0xfe, 0x30, 0x00, 0x00, 0x7f, 0x30, 0x00, 0x00, 0x7f, 0xb0, 0x00,
  0x00, 0x3f, 0xf0, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x0f, 0xf3, 0x80, 0x00, 0x00, 0x67, 0xc0,
  0x00, 0x00, 0x6f, 0xc0, 0x00, 0x00, 0x6f, 0xc0, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x7f, 0x80,
  0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00, 0x60, 0x00,
  0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};



void SetupDisplay()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(0x02 /*|< Gen. display voltage from 3.3V*/, 0x3C)) {
    Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("SSD1306 allocation failed")))));
  }
}

void DisplayStatus()
{
    char buffer[200];

    display.clearDisplay();

    //Full wifi Signal

    if (WiFi.status() == WL_CONNECTED)
      {
        display.drawBitmap(128-16-1, 0, WiFiFull, 16, 13, 1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
      }

    int activeRelay=-1;

    for (int relayId = 0; relayId < nRelays; relayId++)
     {
        if (relayStatus[relayId].duration > 0)
        {
          activeRelay = relayId;
        }
     }

    if (activeRelay >= 0 && activeRelay < nRelays)
    {
      display.drawBitmap(0, 64-45-1, Rega, 32, 45, 1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);

      unsigned long currentMillis = relayStatus[activeRelay].duration - millis();

      if (currentMillis < 0) currentMillis = 0;
      unsigned long sec = currentMillis / 1000;
      unsigned long min = sec / 60;
      unsigned long hr = min / 60;
      sec %= 60;
      min %= 60;
      hr %= 24;

      sprintf(buffer,("%02d:%02d:%02d"), hr, min, sec);

      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
      display.setCursor(0, 0);
      display.print(buffer);

      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
      display.setCursor(64, 30);
      display.println(((reinterpret_cast<const __FlashStringHelper *>(("Setor")))));
      display.setCursor(84, 50);
      display.println(activeRelay+1);
    }
    else
    {
     //display.drawBitmap(0, 64-45-1, Idle, 32, 45, WHITE);

     if (mqttClient.connected())
     {
      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
      display.setCursor(0, 0);
      display.print(((reinterpret_cast<const __FlashStringHelper *>(("Mq")))));
     }

     if (internetConnectionState)
     {
      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
      display.setCursor(30, 0);
      display.print(((reinterpret_cast<const __FlashStringHelper *>(("I")))));
     }

     if (ntpSync)
     {
      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
      display.setCursor(50, 0);
      display.print(((reinterpret_cast<const __FlashStringHelper *>(("Ntp")))));
     }

     RtcDateTime now = getLocalDateTime();

     sprintf(buffer, ("%02d:%02d:%02d"), now.Hour(), now.Minute(), now.Second() );

      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
      display.setCursor(15, 30);
      display.println(buffer);
    }

    display.display();
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\InfluxDB.ino"

# 3 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\InfluxDB.ino" 2
WiFiMulti wifiMulti;







# 12 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\InfluxDB.ino" 2
# 13 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\InfluxDB.ino" 2






// Time zone info


// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client("https://europe-west1-1.gcp.cloud2.influxdata.com", "36a23540c9faa123", "ChickenPark", "bQhhYbvhTmCopKiaYem0mEvQBB2ZwkSdRpBvL2uCL9O6Ub4Ygi_6kMldkB1_0aKEdblNv0N2eTBjSpzsnMk7Yw==", InfluxDbCloud2CACert);

Point sensor("ambiente");


void setupInfluxDB()
{
  /* Setup influxdb   */
  // Check server connection
  if (client.validateConnection())
    {
      log(((reinterpret_cast<const __FlashStringHelper *>(("Connected to InfluxDB: %s")))), "url");
      Serial.println(client.getServerUrl());
    }
    else
    {
      log(((reinterpret_cast<const __FlashStringHelper *>(("InfluxDB connection failed: %s")))), client.getLastErrorMessage());
    }
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\InputRelay.ino"
/***************************************

 *

 * Configure input relay

 *

 **************************************/
# 7 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\InputRelay.ino"
void configureInputRelay()
{
   pinMode(39, 0x01); // Set as input
   pinMode(39, 0x05); // Input with pullup resitor
}
/***************************************

 *

 * return inuo relay value

 *

 **************************************/
# 18 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\InputRelay.ino"
bool readInputRelay()
{
  return digitalRead(39);
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Internet.ino"
void checkInternet()
{
  char buffer[512];
   bool newInternetConnectionState = checkInternetConnection();
   if (newInternetConnectionState != internetConnectionState)
     {

       if (newInternetConnectionState)
         sprintf(buffer, "%s\tWiFI connection changed to CONNECTED.\n", getDateTime());
        else
         sprintf(buffer, "%s\tWiFI connection changed to DISCONNECTED.\n", getDateTime());
      Serial.print(buffer);
      lastConnectionChange = millis();
      internetConnectionState = newInternetConnectionState;
     }

   if ( (! internetConnectionState) && (millis() - lastConnectionChange > internetResetAfter))
     {
      sprintf(buffer, "%s\tRecovering router WIFI.\n", getDateTime());
      Serial.print(buffer);
      RelayCommand(8, 1, 10);
      lastConnectionChange = millis();
     }
}

boolean checkInternetConnection()
{
  if (WiFi.status() != WL_CONNECTED)
    return false;

  char buffer[256];
  IPAddress googleDns(8, 8, 8, 8);
  char googleServerName[] = "www.google.com";
  IPAddress ip(0,0,0,0);

  WiFi.hostByName(googleServerName, ip);

  //sprintf(buffer, "IP is %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

  if ((ip[0] == 0) && (ip[1] == 0) && (ip[2] == 0) && (ip[3] == 0))
    return false;
  else
    return true;
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"



# 5 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 2
# 6 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 2



char temp[300 /*Buffer for storing intermediate strings. Performance may vary depending on size.*/+1];

/*

int log(const __FlashStringHelper* flashStr)

{

  Serial.print(getDateTime());

  Serial.print(F("\t"));

  Serial.println(flashStr);

}

*/
# 19 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
int log(const __FlashStringHelper* flashStr, ...)
{

  char str[300];
  int i, count=0, j=0, flag=0;

  Fcopy(str, 300,flashStr);

  Serial.print(getDateTime());
  Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("\t")))));

  for(i=0; str[i]!='\0';i++) if(str[i]=='%') count++; //Evaluate number of arguments required to be printed

  if (count==0)
  {
    Serial.println(str);
    return 0;
  }
  va_list argv;
  
# 38 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
 __builtin_va_start(
# 38 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
 argv
# 38 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
 ,
# 38 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
 count
# 38 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
 )
# 38 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                      ;
  for(i=0,j=0; str[i]!='\0';i++) //Iterate over formatting string
  {
    if(str[i]=='%')
    {
      //Clear buffer
      temp[j] = '\0';
      Serial.print(temp);
      j=0;
      temp[0] = '\0';

      //Process argument
      switch(str[++i])
      {
        case 'd': Serial.print(
# 52 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              __builtin_va_arg(
# 52 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              argv
# 52 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              ,
# 52 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              int
# 52 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              )
# 52 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                               );
                  break;
        case 'l': Serial.print(
# 54 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              __builtin_va_arg(
# 54 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              argv
# 54 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              ,
# 54 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              long
# 54 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              )
# 54 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                                );
                  break;
        case 'f': Serial.print(
# 56 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              __builtin_va_arg(
# 56 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              argv
# 56 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              ,
# 56 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              double
# 56 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              )
# 56 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                                  );
                  break;
        case 'c': Serial.print((char)
# 58 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                                    __builtin_va_arg(
# 58 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                    argv
# 58 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                                    ,
# 58 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                    int
# 58 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                                    )
# 58 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                                     );
                  break;
        case 's': Serial.print(
# 60 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              __builtin_va_arg(
# 60 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              argv
# 60 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              ,
# 60 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              char *
# 60 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              )
# 60 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                                  );
                  break;
        default: ;
      };
    }
    else
    {
      //Add to buffer
      temp[j] = str[i];
      j = (j+1)%300 /*Buffer for storing intermediate strings. Performance may vary depending on size.*/;
      if(j==0) //If buffer is full, empty buffer.
      {
        temp[300 /*Buffer for storing intermediate strings. Performance may vary depending on size.*/] = '\0';
        Serial.print(temp);
        temp[0]='\0';
      }
    }
  };

  temp[j] = '\0';
  Serial.println(temp); //Print trailing newline

  return count + 1; //Return number of arguments detected
}

int log(char *str, ...)
{
  int i, count=0, j=0, flag=0;

  Serial.print(getDateTime());
  Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("\t")))));

  for(i=0; str[i]!='\0';i++) if(str[i]=='%') count++; //Evaluate number of arguments required to be printed

  if (count==0)
  {
    Serial.println(str);
    return 0;
  }
  va_list argv;
  
# 100 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
 __builtin_va_start(
# 100 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
 argv
# 100 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
 ,
# 100 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
 count
# 100 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
 )
# 100 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                      ;
  for(i=0,j=0; str[i]!='\0';i++) //Iterate over formatting string
  {
    if(str[i]=='%')
    {
      //Clear buffer
      temp[j] = '\0';
      Serial.print(temp);
      j=0;
      temp[0] = '\0';

      //Process argument
      switch(str[++i])
      {
        case 'd': Serial.print(
# 114 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              __builtin_va_arg(
# 114 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              argv
# 114 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              ,
# 114 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              int
# 114 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              )
# 114 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                               );
                  break;
        case 'l': Serial.print(
# 116 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              __builtin_va_arg(
# 116 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              argv
# 116 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              ,
# 116 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              long
# 116 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              )
# 116 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                                );
                  break;
        case 'f': Serial.print(
# 118 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              __builtin_va_arg(
# 118 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              argv
# 118 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              ,
# 118 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              double
# 118 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              )
# 118 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                                  );
                  break;
        case 'c': Serial.print((char)
# 120 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                                    __builtin_va_arg(
# 120 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                    argv
# 120 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                                    ,
# 120 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                    int
# 120 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                                    )
# 120 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                                     );
                  break;
        case 's': Serial.print(
# 122 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              __builtin_va_arg(
# 122 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              argv
# 122 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              ,
# 122 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                              char *
# 122 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino" 3 4
                              )
# 122 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Logging.ino"
                                                  );
                  break;
        default: ;
      };
    }
    else
    {
      //Add to buffer
      temp[j] = str[i];
      j = (j+1)%300 /*Buffer for storing intermediate strings. Performance may vary depending on size.*/;
      if(j==0) //If buffer is full, empty buffer.
      {
        temp[300 /*Buffer for storing intermediate strings. Performance may vary depending on size.*/] = '\0';
        Serial.print(temp);
        temp[0]='\0';
      }
    }
  };

  temp[j] = '\0';
  Serial.println(temp); //Print trailing newline

  return count + 1; //Return number of arguments detected
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\MQTTClient.ino"


void setupMQTT()
{
   lastMqttCheck = millis();

   mqttClient.setServer("m20.cloudmqtt.com", 10182);

   connect2Mqtt();

   int retry=0;
   while((!mqttClient.connected()) && retry < 3)
   {
    connect2Mqtt();
    retry++;
    delay(1000);
   }
}

void connect2Mqtt()
{
  char subscribeTopic[100];

  lastMqttCheck = millis();

  sprintf(subscribeTopic,"%s/cmd/txt", mqttId);

  if (!mqttClient.connected())
    {
      log(((reinterpret_cast<const __FlashStringHelper *>(("Connecting to MQTT...")))));

      if (mqttClient.connect("jardimEsp32Rega", "jardim", "!J@ardim2021!"))
      {
        mqttClient.publish("jardimEsp32Rega/status/txt", "jardimEsp32Rega connected.");
        mqttClient.setCallback(mqttCallback);
        mqttClient.subscribe(subscribeTopic);
        log(((reinterpret_cast<const __FlashStringHelper *>(("MQTT client connected !")))));
      }
      else
      {
       log(((reinterpret_cast<const __FlashStringHelper *>(("MQTT connect failed with state %d")))), mqttClient.state());
      }
   }
   else
   {
     log(((reinterpret_cast<const __FlashStringHelper *>(("MQTT is connected, no need to reconnect.")))));
     mqttClient.loop();
   }
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  char subscribeTopic[100];

  sprintf(subscribeTopic,"%s/cmd/txt", mqttId);

  if (length > 0)
   {
     char charPayload[length+1];

     for (int i = 0; i < length; i++)
     {
      charPayload[i]=payload[i];
     }
     charPayload[length] = 0;

    log(((reinterpret_cast<const __FlashStringHelper *>(("Received MQTT Message, topic is (%s), payload: %s")))), topic, charPayload);

    if (strcmp(topic,subscribeTopic) == 0)
     {
      processCommand(charPayload);
     }
   }
}

void mqttSendLog(char* msg)
{
  char buffer[100];

  sprintf(buffer,"%s/status/txt", mqttId);
  mqttClient.publish(buffer, msg);
}

void mqttSendStatus(const char* component, const char* msg)
{
  char buffer[100];

  sprintf(buffer,"%s/%s/status", mqttId, component);
  mqttClient.publish(buffer, msg);
}

void MqttStatus()
{
  char topic[100];
  char payload[100];
  char buffer[128];

  for (int relayId = 0; relayId < nRelays; relayId++)
  {
    sprintf(topic, "relay%d", relayId+1);

        if (digitalRead(Relays[relayId]) == 0x1)
        {
          sprintf(buffer,"{ \"status\"=\"Off\", \"duration\"=0  }", relayStatus[relayId].duration);
          mqttSendStatus(relayStatus[relayId].relayName, buffer);
        }
        else
        {
          sprintf(buffer,"{ \"status\"=\"On\", \"duration\"=%d }", relayStatus[relayId].duration - millis());
          mqttSendStatus(relayStatus[relayId].relayName, buffer);
        }
  }

  for(int schedId=0; schedId< maxSchedules; schedId++)
  {
    sprintf(topic, "sched%d", schedId+1);
    if (schedule[schedId].enabled)
     sprintf(buffer,"{ \"status\": \"enabled\", \"cron\": \"%s\"}", schedule[schedId].cronString);
    else
     sprintf(buffer,"{ \"status\": \"disabled\", \"cron\": \"%s\"}", schedule[schedId].cronString);

    mqttSendStatus(topic, buffer);
  }
  //mqttSendStatus("temperature", "23");
  //mqttSendStatus("humidity", "77");
  sprintf(payload, "%d", freeMemory());
  mqttSendStatus("freeMemory", payload);
  if (readInputRelay())
     mqttSendStatus("InputRelay","{ \"status\"=\"Off\" }");
   else
     mqttSendStatus("InputRelay","{ \"status\"=\"On\"} }");
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Persistence.ino"

/*********************************************************************

 * 

 * 

 * EEProm read write data

 * 

 * 

 *********************************************************************/
# 9 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Persistence.ino"
  void TestEeprom()
 {
   char buffer[200];

   //EEPROM.begin(FLASH_MEM_ALOC_SIZE);
   delay(2000); // Some delay

   Serial.print(getDateTime());
   Serial.println("Testing eeprom.......................");
   sprintf(buffer,"Write CheckByte = %d", EEPRomVersison);
   Serial.println(buffer);
   EEPROM.write(0, EEPRomVersison);
   EEPROM.commit();
   //EEPROM.end();
 }

 void InitEeprom()
 {
   char buffer[200];

   //EEPROM.begin(FLASH_MEM_ALOC_SIZE);

   Serial.print(getDateTime());
   Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("Init eeprom, size = ")))));
   Serial.println(FLASH_MEM_ALOC_SIZE);
   EEPROM.write(0, EEPRomVersison);

   for(int schedId=0; schedId < maxSchedules; schedId++)
   {
    schedule[schedId].enabled = false;
    strcpy(schedule[schedId].cronString, "");
    writeSchedulerToEeprom(schedId);
   }

   for(int relayId=0; relayId < nRelays; relayId++)
   {
    relayStatus[relayId].enabled = true;
    relayStatus[relayId].maxDuration = 82800;
    sprintf(relayStatus[relayId].relayName, "Relay%d", relayId+1);
    writeRelayToEeprom(relayId);
   }

   EEPROM.commit();
   Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("eeprom data commited.")))));
   //EEPROM.end();
 }

 void readAllFromEeprom()
 {
   EEPROM.begin(FLASH_MEM_ALOC_SIZE);

   for(int schedId=0; schedId < maxSchedules; schedId++)
   {
      readSchedulerFromEeprom(schedId);
   }

   for(int relayId=0; relayId < nRelays; relayId++)
   {
      readRelayFromEeprom(relayId);
   }

   //EEPROM.commit();
   //EEPROM.end();
 }

 void writeAllToEeprom()
 {
   //EEPROM.begin(FLASH_MEM_ALOC_SIZE);

   for(int schedId=0; schedId<maxSchedules;schedId++)
   {
    writeSchedulerToEeprom(schedId);
   }

   for(int relayId=0; relayId < nRelays;relayId++)
   {
    writeRelayToEeprom(relayId);
   }

   EEPROM.commit();
   //EEPROM.end();
 }

 /*********************************************************************

 * 

 * 

 * EEProm Relay functions

 * 

 * 

 *********************************************************************/
# 100 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Persistence.ino"
 void readRelayFromEeprom(int relayId)
 {
   int baseAddress = 1 + maxSchedules * sizeof(EEPromSchedule);

   byte checkByte = EEPROM.read(0);

   Serial.print("EEPROM checkbyte is: ");
   Serial.println(checkByte);
   Serial.print("EEPROM version is: ");
   Serial.println(EEPRomVersison);

   if (checkByte != EEPRomVersison)
   {
    Serial.print(getDateTime());
    Serial.print("\tError: the eeprom is not initialized.\n");
    Serial.print(getDateTime());
    Serial.print("\tInitializing EEProm...\n");
    InitEeprom();
   }

   EEPROM_readAnything(baseAddress + relayId * sizeof(EEPromRelay), eepromRelay);

   relayStatus[relayId].enabled = eepromRelay.enabled;
   relayStatus[relayId].maxDuration = eepromRelay.maxDuration;
   strcpy(relayStatus[relayId].relayName, eepromRelay.relayName);

   relayStatus[relayId].duration = 0;
   relayStatus[relayId].lastDuration = 0;
   strcpy(relayStatus[relayId].lastOn, "");
  }

 void writeRelayToEeprom(int relayId)
  {

   //EEPROM.begin(FLASH_MEM_ALOC_SIZE);

   int baseAddress = 1 + maxSchedules * sizeof(EEPromSchedule);

   eepromRelay.enabled = relayStatus[relayId].enabled;
   eepromRelay.maxDuration = relayStatus[relayId].maxDuration;
   strcpy(eepromRelay.relayName, relayStatus[relayId].relayName);

   //relayStatus[relayId].relayName.toCharArray(eepromRelay.relayName, 32);

   EEPROM_writeAnything(baseAddress + relayId * sizeof(EEPromRelay), eepromRelay);

   Serial.print(getDateTime());
   Serial.print("\tWrite: Relay " );
   Serial.print(relayId);
   Serial.print(" base addr is " );
   Serial.print(baseAddress);
   Serial.println(relayId * sizeof(EEPromRelay));

   Serial.print(getDateTime());
   Serial.print("\tWrite: Relay " );
   Serial.print(relayId);
   Serial.print(" name is '" );
   Serial.print(eepromRelay.relayName );
   Serial.println("'");

   Serial.print(getDateTime());
   Serial.print("\tStore relay ");
   Serial.println(relayId);

   EEPROM.commit();
   //EEPROM.end();
  }

 /*********************************************************************

 * 

 * 

 * EEProm Scheduler functions

 * 

 * 

 *********************************************************************/
# 176 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Persistence.ino"
 void readSchedulerFromEeprom(int schedId)
 {
   int baseAddress = 1;
   byte checkByte = EEPROM.read(0);

/*

Serial.print("EEPROM checkbyte is: ");

Serial.println(checkByte);

Serial.print("EEPROM version is: ");

Serial.println(EEPRomVersison);

*/
# 187 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Persistence.ino"
   if (checkByte != EEPRomVersison)
   {
    Serial.print(getDateTime());
    Serial.println("\tError: eeprom not initialized.");
    Serial.print(getDateTime());
    Serial.println("\tInitializing EEProm...");
    InitEeprom();
   }

   EEPROM_readAnything(baseAddress + schedId * sizeof(EEPromSchedule), eepromSchedule);

   schedule[schedId].enabled = eepromSchedule.enabled;
   strcpy(schedule[schedId].cronString, eepromSchedule.cronString);
   schedule[schedId].lastCheck[0] = 0;
   schedule[schedId].lastRun[0] = 0;
  }

 void writeSchedulerToEeprom(int schedId)
  {
    int baseAddress = 1;

    //EEPROM.begin(FLASH_MEM_ALOC_SIZE);

    eepromSchedule.enabled = schedule[schedId].enabled;
    strcpy(eepromSchedule.cronString, schedule[schedId].cronString);
    //schedule[schedId].cronString.toCharArray(eepromSchedule.cronString,128);

    EEPROM_writeAnything(baseAddress + schedId * sizeof(EEPromSchedule), eepromSchedule);

    EEPROM.commit();
    //EEPROM.end();
  }

/*********************************************************************

 * 

 * 

 * EEProm function

 * 

 * 

 *********************************************************************/
# 228 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Persistence.ino"
template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\RealtimeClock.ino"

// CONNECTIONS:
// DS1302 CLK/SCLK --> 17
// DS1302 DAT/IO --> 25
// DS1302 RST/CE --> 26
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND
//Para usar o DS1302


 ThreeWire myWire(19, 18, 23); // IO, SCLK, CE
 RtcDS1302<ThreeWire> rtc(myWire);



//Para usar o Ds1307





/********************************

 * Setup RTC

 ********************************/
# 26 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\RealtimeClock.ino"
void SetupRTC()
{
  //Wire.begin();

    rtc.Begin();
# 46 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\RealtimeClock.ino"
    RtcDateTime compiled = RtcDateTime("Aug 29 2023", "22:09:13");
    Serial.print("Code compiled at ");
    serialPrintDateTime(compiled);
    Serial.println();


    if (!rtc.IsDateTimeValid())





      {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Serial.print("RTC setting date time to ");
        serialPrintDateTime(compiled);
        Serial.println();


        rtc.SetDateTime(compiled);




    }


    if (rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        rtc.SetIsWriteProtected(false);
    }


    if (!rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        rtc.SetIsRunning(true);
    }



    RtcDateTime now = rtc.GetDateTime();





    if (now < compiled)
    {
        Serial.println("RTC is older than compile time!");
        Serial.println("Updating DateTime !");


        rtc.SetDateTime(compiled);




    }
}


/********************************

 * Get Local DateTime

 ********************************/
# 117 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\RealtimeClock.ino"
RtcDateTime getLocalDateTime()
{

    RtcDateTime dt = rtc.GetDateTime();






  if (! isDst(dt))
  {
    dt.InitWithEpoch32Time(dt.Epoch64Time() - 3600u);
  }

  return dt;
}


/***************************************

 * Verify if DayLighSaving Time active

 ***************************************/
# 140 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\RealtimeClock.ino"
bool isDst(RtcDateTime dt)
{
  if (dt.Month() < 3 || dt.Month() > 10) return false;
  if (dt.Month() > 3 && dt.Month() < 10) return true;

  int previousSunday = dt.Day() - dt.DayOfWeek();

  if (dt.Month() == 3) return previousSunday >= 25;
  if (dt.Month() == 10) return previousSunday < 25;

  return false; // this line never gonna happend
}

/********************************

 * Adjust RTC from unixtime - epoch

 ********************************/
# 157 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\RealtimeClock.ino"
void adjustRTC(unsigned long epoch)
{
  RtcDateTime dt;
  dt.InitWithEpoch64Time(epoch);
  //sprintf(buffer, "Setting RTC to %04d-%02d-%02d %02d:02%d:%02d\n", dt.Year(),dt.Month(),dt.Day,dt.Hour(),dt.Minute(), dt.Second());  


  rtc.SetDateTime(dt);




}

/********************************

 * Change RTC datetime

 ********************************/
# 174 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\RealtimeClock.ino"
void setDateTime(String cmdStr)
{
  cmdStr.replace("-",",");
  cmdStr.replace(":",",");

  String y = getValue(cmdStr, ',', 1);
  String mm = getValue(cmdStr, ',', 2);
  String d = getValue(cmdStr, ',', 3);
  String h = getValue(cmdStr, ',', 4);
  String m = getValue(cmdStr, ',', 5);
  String s = getValue(cmdStr, ',', 6);

  char buffer[200];
  sprintf(buffer,"%s\tSetting DateTime to %d-%d-%d %d:%d:%d",getDateTime(), y, mm, d, h ,m ,s);
  Serial.println(buffer);


  rtc.SetDateTime(RtcDateTime(y.toInt(),mm.toInt(),d.toInt(),h.toInt(),m.toInt(),s.toInt()));





}

void setDateTime(int year, int month, int day, int hour, int minute, int second)
{
  char buffer[200];

  Serial.print(getDateTime());
  sprintf(buffer,"\tSetting dateTime to %04d-%02d-%02d %02d:%02d:%02d",year,month,day,hour,minute,second);


  rtc.SetDateTime(RtcDateTime(year,month,day,hour,minute,second));





}


void serialPrintDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf(datestring,
            (sizeof(datestring) / sizeof(datestring[0])),
            ("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Relay.ino"
/***************************************

 *

 * return relay status

 *

 **************************************/
# 7 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Relay.ino"
bool isRelayOn(int relayId)
{
  relayId=relayId-1;

  if (digitalRead(Relays[relayId])==0x1)
    return false;

  return true;
}


/***************************************

 *

 * Check if On onRelay is ended

 *

 **************************************/
# 24 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Relay.ino"
void VerifyRelaysTimeout()
{
  for(int i=1; i<= nRelays; i++)
    if (relayStatus[i-1].duration < millis() && isRelayOn(i))
    {
       RelayCommand(i, 0, 0);
    }
}

/***************************************

 *

 * Change Name

 *

 **************************************/
# 39 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Relay.ino"
void RelaySetName(int relayId, char *newName)
{
  relayId=relayId-1;
  sprintf(relayStatus[relayId].relayName,"%s",newName);
  writeRelayToEeprom(relayId);
}

/***************************************

 *

 * Send Relays commands

 *

 **************************************/
# 52 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Relay.ino"
void RelayCommand(int relayId, int action, int duration)
{
  boolean outBit = 0x0;
  unsigned long elapsed = duration;
  char buffer[256];


  relayId= relayId-1;

  if (action > 0)
   {
    sprintf(buffer,"{ \"status\"=\"ON\", \"duration\"=%d }", duration);
    mqttSendStatus(relayStatus[relayId].relayName, buffer);

    sprintf(relayStatus[relayId].lastOn, "%s", getDateTimeShort());

    sprintf(buffer,"%s\tActivating relay %d for %d seconds", getDateTime(), relayId+1, duration);
    mqttSendLog(buffer);


    relayStatus[relayId].lastDuration = elapsed;
    relayStatus[relayId].duration = millis() + elapsed*1000L;
    log(((reinterpret_cast<const __FlashStringHelper *>(("Setting relay %d lastDuration to %d")))), relayId, relayStatus[relayId].lastDuration);
    outBit = 0x0;
   }
  else
   {
    sprintf(buffer,"%s\tDeactivating relay %d", getDateTime(), relayId+1);
    mqttSendLog(buffer);
    Serial.println(buffer);

    mqttSendStatus(relayStatus[relayId].relayName, "{ \"status\"=\"OFF\" }");

    relayStatus[relayId].duration = 0;
    log(((reinterpret_cast<const __FlashStringHelper *>(("Setting relay %d duration to %d")))), relayId, relayStatus[relayId].duration);
    outBit = 0x1;
   }

  digitalWrite(Relays[relayId], outBit);

}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Scheduler.ino"
//schedTimeMatch(i, cmd);


bool schedTimeMatch(int schedId, char* cmd)
{
  char strMinute[50];
  char strHour[50];
  char strWeekDay[50];
  char strDay[50];
  char strMonth[50];
  int repeatFrequency;

  if (! schedule[schedId].enabled ||
         strcmp(schedule[schedId].lastCheck,getDateTimeShort()) == 0)
      return false;

   strcpy(schedule[schedId].lastCheck,getDateTimeShort());

   RtcDateTime now = getLocalDateTime();

   bool rc = false;

  if (sscanf(schedule[schedId].cronString,"%[0-9,*] %[0-9,*] %[0-9,*] %[0-9,*] %[0-9,*] %d %[^\n]s",strMinute,strHour,strWeekDay,strDay,strMonth,&repeatFrequency,cmd) == 7)
  {
   //Serial.printf("%s\tCron testing command to execute: %s\n", getDateTime(), cmd);
   rc = cronCheck(strMinute, now.Minute());
   rc = rc && cronCheck(strHour, now.Hour());

   if (repeatFrequency < 2)
   {
    rc = rc && cronCheck(strWeekDay, now.DayOfWeek() + 1);
    rc = rc && cronCheck(strDay, now.Day());
    rc = rc && cronCheck(strMonth, now.Month());
   }
   else
   {
     rc && repeatCheck(repeatFrequency, now.Day());
   }
  }

  return rc;
}

bool cronCheck(char* cronStr, int timeVal)
{
  int i;
  char buffer[256];
  bool rc = false;


  strcpy(buffer, cronStr);
  char* token = strtok(buffer, ",");
  while (token)
  {
    if (strcmp(token,"*") == 0) return true;

    if (sscanf(token, "%d", &i) == 1)
      {
        if (i == timeVal) return true;
      }

    //Serial.printf("token: %s\n", token);
    token = strtok(
# 63 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Scheduler.ino" 3 4
                  __null
# 63 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Scheduler.ino"
                      , ",");
  }
  return false;
}

bool repeatCheck(int repeatFrequency, int dayOfMonth)
{
   return ( (dayOfMonth % repeatFrequency) == 0);
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Sensores.ino"
# 2 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Sensores.ino" 2




/****************************************************************

 * DHT22 data

 ****************************************************************/



// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)


DHT dht(18, DHT21 /* DHT 21 (AM2301)*/);


void setupSensors()
{
  dht.begin();

  timeSync("UTC0", "pool.ntp.org", "time.nis.gov");
}

void loopSensors()
{
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature(0);

  // check if returns are valid then something went wrong!
  if (isnan(h) || isnan(t))
   {
    log(((reinterpret_cast<const __FlashStringHelper *>(("Failed to read from DHT")))));
   }
   else
   {

    sensor.clearFields();

    // Store measured value into point
    // Report RSSI of currently connected network
    sensor.addField("temperatura", t);
    sensor.addField("humidade", h);

    // Print what are we exactly writing
    log(((reinterpret_cast<const __FlashStringHelper *>(("Writing to influxDB: %s")))), sensor.toLineProtocol());

    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED) {
      log(((reinterpret_cast<const __FlashStringHelper *>(("Wifi connection lost")))));
    }

    // Write point
    if (!client.writePoint(sensor)) {
      log(((reinterpret_cast<const __FlashStringHelper *>(("InfluxDB write failed: %s")))), client.getLastErrorMessage());
    }


    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");
   }
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\SerialComm.ino"


/*
   SerialEvent occurs whenever a new data comes in the hardware serial RX. This
   routine is run between each time loop() runs, so using delay inside loop can
   delay response. Multiple bytes of data may be available.
 */
void serialEvent()
{
   char inChar = 0;
   while (Serial.available())
   {
     // get the new byte:
     inChar = (char)Serial.read();
     // add it to the input Buffer
     if (inChar == '\r')
     {
       inputBufferComplete = true;
     }
     else
     {
      if (inChar != '\n' && inputBufferPos < (INPUTBUFFERSIZE-1))
      {
        inputBuffer[inputBufferPos++] = inChar;
        inputBuffer[inputBufferPos] = 0;
      }
     }
   }
}

void clearInputBuffer()
{
  for(int i=0; i< INPUTBUFFERSIZE; i++)
    inputBuffer[i] = 0;
  inputBufferPos = 0;
  inputBufferComplete = false;
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\TimeSync.ino"
// ntp

 unsigned int localPort = 8888; // local port to listen for UDP packets

 //char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server
 char timeServer[] = "pool.ntp.org"; // time.nist.gov NTP server

 const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

 byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP


WiFiUDP udp;


void setupNtp()
{
   udp.begin(localPort);
   lastNtpCheck = millis();
   setClockFromNtp();
}

void setClockFromNtp()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(getDateTime());
    Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\tNtp Sync aborted, no Internet connection...")))));
    return;
  }
  Serial.print(getDateTime());
  Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\tNtp Sync from internet...")))));

  lastNtpCheck = millis();
  unsigned long STDTZOFFSET = 1;

   sendNTPpacket(timeServer); // send an NTP packet to a time server

   // wait to see if a reply is available
   delay(1000);
   if (udp.parsePacket())
   {
     ntpSync = true;
     Serial.print(getDateTime());
     Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\tNtp Sync packet received...")))));

     // We've received a packet, read the data from it
     udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

     // the timestamp starts at byte 40 of the received packet and is four bytes,
     // or two words, long. First, extract the two words:

     unsigned long highWord = makeWord(packetBuffer[40], packetBuffer[41]);
     unsigned long lowWord = makeWord(packetBuffer[42], packetBuffer[43]);
     // combine the four bytes (two words) into a long integer
     // this is NTP time (seconds since Jan 1 1900):
     unsigned long secsSince1900 = highWord << 16 | lowWord;
     //Serial.print(getDateTime() + "\tSeconds since Jan 1 1900 = ");
     //Serial.println(secsSince1900);

     // now convert NTP time into everyday time:
     //Serial.print("Unix time = ");
     // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
     const unsigned long seventyYears = 2208988800UL;
     // subtract seventy years:
     unsigned long epoch = secsSince1900 - seventyYears + (STDTZOFFSET * 60 * 60);
     // print Unix time:

     //Serial.println(getDateTime() + "\tEpoch: " + epoch);

     adjustRTC(epoch);

     // print the hour, minute and second:
     Serial.print(getDateTime());
     Serial.print("\tThe UTC time is "); // UTC is the time at Greenwich Meridian (GMT)
     Serial.print((epoch % 86400L) / 3600); // print the hour (86400 equals secs per day)
     Serial.print(':');
     if (((epoch % 3600) / 60) < 10) {
       // In the first 10 minutes of each hour, we'll want a leading '0'
       Serial.print('0');
     }
     Serial.print((epoch % 3600) / 60); // print the minute (3600 equals secs per minute)
     Serial.print(':');
     if ((epoch % 60) < 10) {
       // In the first 10 seconds of each minute, we'll want a leading '0'
       Serial.print('0');
     }
     Serial.println(epoch % 60); // print the second
   }
   else
   {
     Serial.print(getDateTime());
     Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\tNtp Sync FAIL.")))));
     ntpSync = false;
   }
}

// send an NTP request to the time server at the given address
void sendNTPpacket(char* address) {
   // set all bytes in the buffer to 0
   memset(packetBuffer, 0, NTP_PACKET_SIZE);
   // Initialize values needed to form NTP request
   // (see URL above for details on the packets)
   packetBuffer[0] = 0b11100011; // LI, Version, Mode
   packetBuffer[1] = 0; // Stratum, or type of clock
   packetBuffer[2] = 6; // Polling Interval
   packetBuffer[3] = 0xEC; // Peer Clock Precision
   // 8 bytes of zero for Root Delay & Root Dispersion
   packetBuffer[12] = 49;
   packetBuffer[13] = 0x4E;
   packetBuffer[14] = 49;
   packetBuffer[15] = 52;

   // all NTP fields have been given values, now
   // you can send a packet requesting a timestamp:
   udp.beginPacket(address, 123); //NTP requests are to port 123
   udp.write(packetBuffer, NTP_PACKET_SIZE);
   udp.endPacket();
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Utils.ino"

int freeMemory()
{
 return ESP.getFreeHeap();
}


char* getDateTime(RtcDateTime dt)
{
   sprintf(dateTime,"%04d-%02d-%02d %02d:%02d:%02d", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second());

   return dateTime;
}


char* getDateTime()
{
   RtcDateTime now = getLocalDateTime();

   sprintf(dateTime,"%04d-%02d-%02d %02d:%02d:%02d", now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), now.Second());

   return dateTime;
}

bool ClockOk()
{
  RtcDateTime now = getLocalDateTime();

  return (now.Year() >= 2018);
}

char* getDateTimeShort()
{

   RtcDateTime now = getLocalDateTime();

   if (now.Year() >= 2018)
   {
    lastClockCheckOk = millis();
   }
   else
   {
    if (lastClockSetup < (millis() - 10000))
    {
     lastClockSetup = millis();
     Serial.print("Clock failure...Performing RTC Setup\n");
     mqttSendLog("Clock failure...Performing RTC Setup");
     SetupRTC();
     now = getLocalDateTime();
    }
   }

   if (now.Year() < 2018 && lastClockCheckOk < (millis() - 120000))
   {
    Serial.print("Clock failure. Year=");
    Serial.print(now.Year());
    Serial.print("Performing system reset\n");
    mqttSendLog("Clock failure...Performing system reset");
    Reload();
   }
   sprintf(dateTime,"%04d-%02d-%02d %02d:%02d", now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute());

   return dateTime;
}

/*

char* getDateTime()

{

  sprintf(dateTime,"%4d-%2d-%2d %2d:%2d:%2d", 2018, 8, 15, 16, 43, 45);

  return dateTime;

}



char* getDateTimeShort()

{  

   sprintf(dateTime,"%4d-%2d-%2d %2d:%2d", 2018, 8, 15, 16, 43);

    return dateTime;

}

*/
# 80 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Utils.ino"
void GetFreeMemory()
{
 log(((reinterpret_cast<const __FlashStringHelper *>(("Free memory: %d")))), (int)(freeMemory()));
}

void showMemoryUsage()
{
  char buffer[100];
  sprintf(buffer,"%s\tFree heap memory %d bytes\n", getDateTime(), ESP.getFreeHeap());
  Serial.print(buffer);
  sprintf(buffer,"%d",ESP.getFreeHeap());
  mqttSendStatus("freeMemory", buffer);
}


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean isValidNumber(String str)
{
   for(byte i=0;i<str.length();i++)
   {
      if(! isDigit(str.charAt(i))) return false;
   }
   return true;
}

void Fcopy(char* buf, int n, const __FlashStringHelper *ifsh)
{
  const char *p = (const char *)ifsh;
  int i = 0;
  uint8_t c = 0;
  do
  {
    c = (*(const unsigned char *)(p++));
    buf[i++] = c;
  } while ( c != 0 && i < n);
}


void decodeFromBase64(char *decoded, int len, char* encoded)
{

  // decode_base64() does not place a null terminator, because the output is not always a string
  unsigned int string_length = decode_base64((unsigned char*) encoded, (unsigned char*) decoded);
  decoded[string_length] = '\0';

/*





  decoded[0]=0;





  int decodedLength = base64.decodedLength(encoded, strlen(encoded));





  if (decodedLength >= len || decodedLength < 1)

  {

    log(F("Error in base64 string decode, decoded string size=%d max size is %d"), decodedLength, len);

    return;

  }



  base64.decode(decoded, encoded, decodedLength);

*/
# 158 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Utils.ino"
  return;
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Validate.ino"



bool isValidCmd(char *cmdBuffer)
{
 int n;
 int relay=-1;
 int sched=-1;
 int duration=-1;
 char op[200];
  int y;
  int mm;
  int dd;
  int h;
  int m;
  int s;
  char strMinute[50];
  char strHour[50];
  char strWeekDay[50];
  char strDay[50];
  char strMonth[50];
  int freq;


  if (sscanf(cmdBuffer,"sched,%d,change,\"%[0-9,*] %[0-9,*] %[0-9,*] %[0-9,*] %[0-9,*] %d %[^\"]s",&sched,strMinute,strHour,strWeekDay,strDay,strMonth,&freq,op) == 8)
  {
    if (!isValidateNumberArray(strMinute,0,59)) return false;
    if (!isValidateNumberArray(strHour,0,23)) return false;
    if (!isValidateNumberArray(strWeekDay,1,7)) return false;
    if (!isValidateNumberArray(strDay,1,31)) return false;
    if (!isValidateNumberArray(strMonth,1,12)) return false;
    if (sched < 1 || sched > maxSchedules) return false;
    if (freq < 1 || freq > 30) return false;
    if (! isValidCmd(op)) return false;
    return true;
  }

  if (sscanf(cmdBuffer,"datetime,%d-%d-%d,%d:%d:%d",&y,&mm,&dd,&h,&m,&s) == 6)
  {
    if (y < 2017 || y > 3000) return false;
    if (mm < 1 || mm > 12) return false;
    if (dd < 1 || dd > 31) return false;
    if (h < 0 || h > 23) return false;
    if (m < 0 || m > 59) return false;
    if (s < 0 || s > 59) return false;
    return true;
  }

  if (sscanf(cmdBuffer,"sched,%d,%[^\n]s",&sched,op) == 2)
  {
    if (sched < 1 || sched > maxSchedules) return false;
    if (strcmp(op,"enable") != 0 && strcmp(op,"disable") != 0)
      return false;
    return true;
  }

  if (sscanf(cmdBuffer,"relay,%d,name,%[^\n]s",&relay,op) == 2)
  {
    if (relay < 1 || relay > nRelays) return false;
    return true;
  }

 if (sscanf(cmdBuffer,"relay,%d,on,%d",&relay,&duration) == 2)
  {
    if (duration < 1) return false;
    if (relay < 1 || relay > nRelays) return false;
    return true;
  }

 if (sscanf(cmdBuffer,"relay,%d,%[of]",&relay, op) == 2)
  {
    if (strcmp(op, "off") != 0) return false;
    if (relay < 1 || relay > nRelays) return false;

    return true;
  }

  if (strcmp(cmdBuffer,"status") == 0 || strcmp(cmdBuffer,"s") == 0)
  {
    return true;
  }

  if (strcmp(cmdBuffer,"h") == 0 || strcmp(cmdBuffer,"help") == 0)
  {
    return true;
  }

  if (strcmp(cmdBuffer,"reload") == 0)
  {
    return true;
  }

  if (strcmp(cmdBuffer,"ntpsync") == 0)
  {
    return true;
  }

 return false;
}

bool isValidateNumberArray(char* str, int vMin, int vMax)
{
  int i;
  char buffer[256];

  strcpy(buffer, str);
  char* token = strtok(buffer, ",");
  while (token)
  {
    if (strcmp(token,"*") != 0)
    {
      if (sscanf(token, "%d", &i) == 1)
      {
        if (i<vMin || i > vMax) return false;
      }
      else
      {
        return false;
      }
    }
    //Serial.printf("token: %s\n", token);
    token = strtok(
# 122 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Validate.ino" 3 4
                  __null
# 122 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\Validate.ino"
                      , ",");
  }
  return true;
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\WebServer.ino"
char buffer[400];



char readString[250]; //string for fetching data from address
int readStringLen = 0;
char* cmds[4];

WiFiClient tcpClient;

WiFiServer server(80);

void setupWebServer()
{
  readString[0]=0;

  server.begin();
  //log(F("Server is at %s"), WiFi.localIP());
}

void WebServer() {
  // listen for incoming tcpClients
  tcpClient = server.available();

  if (tcpClient)
  {
    readString[0] = 0;
    readStringLen = 0;
    readString[250 -1] = 0;

    log(((reinterpret_cast<const __FlashStringHelper *>(("New HTTP tcpClient connected.")))));
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    unsigned long connectTime = millis();

    while (tcpClient.connected() && (millis() < (connectTime + 4000L)))
    {
      if (tcpClient.available())
      {
        char c = tcpClient.read();

        //log("WebServer (%d)(%d) c=%d %c", strlen(readString), readStringLen, (int) c, c);

        //read char by char HTTP request
        if (readStringLen < 250 -1)
        {
          //store characters to string
          readString[readStringLen]=c;
          readString[readStringLen+1]=0;
          readStringLen++;
        }
        else
        {
          log("WebServer, readString buffer is full.");
        }

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          log("WebServer, 'c' is NL and currentLineIsBlank=true");
          //Serial.println(readString);
          // send a standard http response header

          int j = 0;
          int nCmds = 0;
          cmds[0] = readString;

          log(((reinterpret_cast<const __FlashStringHelper *>(("CMD is %s")))), readString);

          for (j = 6; (j < 250 && readString[j] != 0 && readString[j] != '\n' && nCmds < 4); j++)
           {
             if ((readString[j - 1] == ',' && readString[j] != ',') ||
                 (readString[j] != ',' &&
                  readString[j - 1] == '=' &&
                  readString[j - 2] == 'd' &&
                  readString[j - 3] == 'm' &&
                  readString[j - 4] == 'c' &&
                  readString[j - 5] == '?'))
              {
               readString[j - 1] = 0;
               cmds[nCmds++] = readString + j;
              }
              if (readString[j - 1] == ',') readString[j - 1] = 0;
              if (readString[j] == ',' && readString[j+1] == 0) readString[j] = 0;
            }

            for (j = 0; j < nCmds; j++)
            {
              for(int i=0; i < strlen(cmds[j]); i++)
              {
                if (cmds[j][i] == ' ')
                  {
                    cmds[j][i]=0;
                  }
              }

              decodeFromBase64(buffer, 100, cmds[j]);
              log(((reinterpret_cast<const __FlashStringHelper *>(("Received command: >%s< Decoded: >%s<")))), cmds[j], buffer);
              processCommand(buffer);
            }


          tcpClient.println("HTTP/1.1 200 OK");

          tcpClient.println("Content-Type: application/json; charset=UTF-8");
          tcpClient.println("Cache-Control: no-cache");
          tcpClient.println("Connection: close"); // the connection will be closed after completion of the response
          tcpClient.println("Transfer-Encoding: chunked");
          tcpClient.println();

          GetSystemStatus();
          /*

          GetSystemStatus_header(buffer);

          sprintf(teststring, "%02X", strlen(buffer));

          tcpClient.println(teststring);

          tcpClient.println(buffer);

          

          GetSystemStatus_relay(buffer);

          sprintf(teststring, "%02X", strlen(buffer));

          tcpClient.println(teststring);

          tcpClient.println(buffer);

                  

          GetSystemStatus_sch(buffer);

          sprintf(teststring, "%02X", strlen(buffer));

          tcpClient.println(teststring);

          tcpClient.println(buffer);

          

          GetSystemStatus_footer(buffer);

          sprintf(teststring, "%02X", strlen(buffer));

          tcpClient.println(teststring);

          tcpClient.println(buffer);

          */
# 137 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\WebServer.ino"
          tcpClient.println(0);
          tcpClient.println("");
          // give the web browser time to receive the data
          delay(10);
          // close the connection:
          tcpClient.stop();
          log(((reinterpret_cast<const __FlashStringHelper *>(("Tcp client disconnected")))));
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }


  }
}

/***********************************************/
void SendAndFlushBuffer()
{

  char buffLen[4];
  int len = strlen(buffer);
  if (len > 0)
  {
   sprintf(buffLen, "%02X", strlen(buffer));
   tcpClient.println(buffLen);
   tcpClient.println(buffer);
   strcpy(buffer, "");
  }
}
/********************* Status ******************/
void GetSystemStatus()
{
  char str[200];

  strcpy(buffer, "{\r\n");

  sprintf(str, "\"Command Status\": \"%s\",\r\n", "ok");
  strcat(buffer, str);

  sprintf(str, "\"DateTime\": \"%s\",\r\n", getDateTime());
  strcat(buffer, str);

  SendAndFlushBuffer();

  buffer[0] = 0;

  // Relay Status

  GetFreeMemory();

  strcat(buffer, "\"Relays\": [ \r\n");


  for (int i = 0; i < nRelays; i++)
  {
    if (i > 0) strcat(buffer, ", \r\n");

    strcat(buffer, " { ");

    sprintf(str, "\"Id\": %d,", i+1);
    strcat(buffer, str);

    sprintf(str, "\"Name\": \"%s\",", relayStatus[i].relayName);
    strcat(buffer, str);

    sprintf(str, "\"LastOn\": \"%s\",", relayStatus[i].lastOn);
    strcat(buffer, str);

    sprintf(str, "\"LastDuration\": %d,", relayStatus[i].lastDuration);
    strcat(buffer, str);

    //Serial.println(getDateTime() + i + String(F("\tduration: ")) + relayStatus[i].duration + "\t" + millis());

    if (digitalRead(Relays[i]) == 0x1)
    {
     sprintf(str, "\"Duration\": %d,", 0);
     strcat(buffer, str);
     sprintf(str, "\"Status\": \"%s\"","off");
     strcat(buffer, str);
    }
    else
    {
      if (relayStatus[i].duration > millis())
      {
        sprintf(str, "\"Duration\": %d,", (int)((relayStatus[i].duration - millis()) / 1000.0));
        strcat(buffer, str);
      }
      else
      {
        sprintf(str, "\"Duration\": %d,", 0);
        strcat(buffer, str);
      }
      sprintf(str, "\"Status\": \"%s\"", "On");
      strcat(buffer, str);
    }


    strcat(buffer, " } ");
    if (i%2 == 0) SendAndFlushBuffer();
  }


  strcat(buffer,"\r\n], \r\n");

  SendAndFlushBuffer();

  // Scheduler Status
  GetFreeMemory();

  buffer[0] = 0;

  strcat(buffer, "\"Schedulers\": [ \r\n");

//maxSchedules
  for (int i = 0; i < maxSchedules; i++)
  {
    if (i > 0) strcat(buffer, ", \r\n");

    strcat(buffer, " { ");

    sprintf(str, "\"Id\": %d,", i+1);
    strcat(buffer, str);

    if (schedule[i].enabled)
      strcat(buffer,"\"Status\": \"Enabled\", ");
    else
      strcat(buffer,"\"Status\": \"Disabled\", ");

    sprintf(str, "\"LastRun\": \"%s\",", schedule[i].lastRun);
    strcat(buffer, str);

    sprintf(str, "\"Schedule\": \"%s\"", schedule[i].cronString);
    strcat(buffer, str);

    strcat(buffer, "}");
    GetFreeMemory();
    if (i%2 == 0) SendAndFlushBuffer();
  }

  SendAndFlushBuffer();
  log(((reinterpret_cast<const __FlashStringHelper *>(("Total json response size is %d bytes.")))), strlen(buffer));

/*****************/

  buffer[0] = 0;

  strcpy(buffer, "\r\n],\r\n");

  strcat(buffer, "\"InputRelay\": [ {\r\n");

  strcat(buffer,"\"Name\": \"Bomba do furo\", ");

  if (readInputRelay())
      strcat(buffer,"\"Status\": \"OFF\", ");
    else
      strcat(buffer,"\"Status\": \"ON\", ");

  sprintf(str, "\"Duration\": %d", (int)(millis() - lastInputRelayDebounceTime));
  strcat(buffer, str);

  strcat(buffer, "}\r\n]\r\n }");


  log(((reinterpret_cast<const __FlashStringHelper *>(("Total json response size is %d bytes.")))), strlen(buffer));

  SendAndFlushBuffer();
  return;
}
# 1 "C:\\Users\\fmcnu\\OneDrive\\Documents\\Arduino\\ChickenPark\\WiFi.ino"

const char* ssid = "Quintinha_Dos_Lirios_Sala";
const char* pwd = "!fsnunes2020!";
//const char * ssid = "QuintinhaDosLirios";
//const char * pwd = "2ddf1ce803";


// Set your Static IP address
//Para wifi sala
IPAddress local_IP(192, 168, 0, 27);
IPAddress gateway(192, 168, 0, 1);

//IPAddress local_IP(192, 168, 1, 21);
//IPAddress gateway(192, 168, 1, 254);

//Para wifi abrigo de jardim
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4); //optional


void connectToWiFi()
{
  int ledState = 0;

  if (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(2, 0x0);
    Serial.print(getDateTime());
    Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("\tConnecting to WiFi network:")))));
    Serial.println(String(ssid));

   /************ Static IP ***************/

   if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
    {
      log(((reinterpret_cast<const __FlashStringHelper *>(("Failed to config WiFi Static IP...")))));
    }

   /**************************************/

    WiFi.begin(ssid, pwd);
    for(int i=0; i< 10;i++)
    {
      Serial.print(getDateTime());
      Serial.println(((reinterpret_cast<const __FlashStringHelper *>(("\tConnecting to WiFi..")))));
      if (WiFi.status() == WL_CONNECTED)
      {
        digitalWrite(2, 0x0);
        Serial.print(getDateTime());
        Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("\tWiFi connected with IP address: ")))));
        Serial.println(WiFi.localIP());
        return;
      }
      delay(100);
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
   digitalWrite(2, 0x1);
   Serial.print(getDateTime());
   Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("\tWiFi connected with IP address: ")))));
   Serial.println(WiFi.localIP());
  }
}
