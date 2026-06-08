
const char* ssid = "Quintinha_Dos_Lirios_Sala";
const char* pwd =  "!fsnunes2020!";
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
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print(getDateTime());
    Serial.print(F("\tConnecting to WiFi network:" ));
    Serial.println(String(ssid));

   /************ Static IP ***************/

   if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
    {
      log(F("Failed to config WiFi Static IP..."));
    }

   /**************************************/
    
    WiFi.begin(ssid, pwd);
    for(int i=0; i< 10;i++)
    {
      Serial.print(getDateTime());
      Serial.println(F("\tConnecting to WiFi.."));
      if (WiFi.status() == WL_CONNECTED)
      {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print(getDateTime());
        Serial.print(F("\tWiFi connected with IP address: "));
        Serial.println(WiFi.localIP());
        return;
      }
      delay(100);
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) 
  {
   digitalWrite(LED_BUILTIN, HIGH);
   Serial.print(getDateTime());
   Serial.print(F("\tWiFi connected with IP address: "));
   Serial.println(WiFi.localIP());
  }
}
