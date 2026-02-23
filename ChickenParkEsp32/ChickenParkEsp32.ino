#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <TimeLib.h>
#include <time.h>

// ===================== REDE =====================
#define USE_DEV_NETWORK 0   // 0 = rede final, 1 = rede dev/teste

#if USE_DEV_NETWORK
const char* ssid = "Quintinha_Dos_Lirios_Sala";
const char* password = "!fsnunes2020!";
IPAddress localIP(192,168,0,33);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,0,1);
const char* mqttServer = "192.168.0.7";
#else
const char* ssid = "QuintinhaDosLirios";
const char* password = "2ddf1ce803";
IPAddress localIP(192,168,1,33);
IPAddress gateway(192,168,1,254);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,1,254);
const char* mqttServer = "192.168.1.50";
#endif

// ===== HARDWARE =====
#define ONE_WIRE_BUS 27
const int relays[] = {16,17,25,26};
const int numRelays = 4;

// ===== EEPROM =====
#define EEPROM_SETP_ADDR 0
#define EEPROM_HYST_ADDR 10
float setPoint, hysteresis;
float maxSafetyTemp = 40.0;
float minHysteresis = 0.1;
float maxHysteresis = 5.0;
float currentTemp;
bool relayState[4] = {false,false,false,false};

// ===== MQTT =====
const int mqttPort = 1883;
const char* mqttUser = "termosifao";
const char* mqttPass = "Kusku2026";

const uint16_t MQTT_KEEPALIVE_SEC = 30;
const uint16_t MQTT_SOCKET_TIMEOUT_SEC = 10;
const unsigned long MQTT_RETRY_BASE = 5000;
const unsigned long MQTT_RETRY_MAX  = 60000;

unsigned long lastMqttAttempt = 0;
unsigned long mqttRetryInterval = MQTT_RETRY_BASE;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ===== OLED =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== SENSORES =====
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ===== WEB SERVER =====
WebServer server(80);

// ===== BUFFERS =====
char msg[256];
char msgLong[256];

// ===== FLAGS =====
bool wifiReady = false;
bool mqttReady = false;
bool webReady = false;
bool ntpReady = false;

// ===== TIMERS =====
unsigned long lastWiFiAttempt = 0;
unsigned long lastNtpAttempt = 0;
unsigned long lastTempRead = 0;

const unsigned long WIFI_RETRY_INTERVAL = 30000;
const unsigned long NTP_RETRY_INTERVAL = 60000;
const unsigned long TEMP_READ_INTERVAL = 5000;

// =============================================================
// DISPLAY
// =============================================================
void showSplashScreen(){
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(10,10);
    display.println(F("ChickenPark"));
    display.setTextSize(1);
    display.setCursor(30,40);
    display.println(F("Iniciando..."));
    display.display();
    delay(1500);
}

void showMainScreen(float temp,bool relayOn){
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0,0);
    display.println(F("ChickenPark"));

    display.setTextSize(3);
    display.setCursor(0,15);
    display.print(temp,1);
    display.println("C");

    display.setTextSize(2);
    display.setCursor(0,50);


    if ((millis() / 2000) % 2 == 0) 
    {
        display.print(F("Calor: "));
        display.println(relayOn ? F("ON") : F("OFF"));
    }
    else
    {
        display.print(F("Lim: "));
        display.println(String(setPoint,1));
    }

    display.display();
}

// =============================================================
// LOG COM TIMESTAMP
// =============================================================
void logPrint(const char* str){
    snprintf(msgLong,256,"%04d-%02d-%02d %02d:%02d:%02d | %s",
             year(now()),month(now()),day(now()),
             hour(now()),minute(now()),second(now()),
             str);
    Serial.println(msgLong);
}

// =============================================================
// WEB
// =============================================================
void handleWebRoot(){

    // ===== SETPOINT =====
    if(server.hasArg("temp")){
        String s = server.arg("temp");
        if(s.length() > 0){
            float newSet = s.toFloat();
            if(newSet>=10 && newSet<=40){
                setPoint=newSet;
                EEPROM.put(EEPROM_SETP_ADDR,setPoint);
                EEPROM.commit();
                logPrint("Novo setpoint guardado (WEB)");
            } else logPrint("Setpoint WEB fora de limites");
        } else logPrint("Setpoint WEB vazio ignorado");
    }

    // ===== HISTERESIS =====
    if(server.hasArg("hyst")){
        String s = server.arg("hyst");
        if(s.length() > 0){
            float newHyst = s.toFloat();
            if(newHyst>=minHysteresis && newHyst<=maxHysteresis){
                hysteresis=newHyst;
                EEPROM.put(EEPROM_HYST_ADDR,hysteresis);
                EEPROM.commit();
                logPrint("Nova hysterese guardada (WEB)");
            } else logPrint("Histerese WEB fora de limites");
        } else logPrint("Histerese WEB vazia ignorada");
    }

    String html = "<!DOCTYPE HTML><html><h1>ChickenPark</h1>";
    html += "<p>Temperatura: "+String(currentTemp,1)+" C</p>";
    html += "<p>Setpoint: "+String(setPoint,1)+" C</p>";
    html += "<p>Histerese: "+String(hysteresis,1)+" C</p>";
    html += "<p>Calor: "+String(relayState[0]?"ON":"OFF")+"</p>";
    html += "<form method='get'>";
    html += "Novo setpoint: <input name='temp' type='number' step='0.1'><br>";
    html += "Nova hysterese: <input name='hyst' type='number' step='0.1'><br>";
    html += "<input type='submit' value='Alterar'></form>";
    html += "</html>";

    server.send(200,"text/html",html);
}

// =============================================================
// BUILD TIME FALLBACK
// =============================================================
time_t getBuildTimeEpoch(){
    tmElements_t tm;
    const char monthNames[]="JanFebMarAprMayJunJulAugSepOctNovDec";
    char monthStr[4];
    memcpy(monthStr,__DATE__,3);
    monthStr[3]='\0';

    tm.Month=(strstr(monthNames,monthStr)-monthNames)/3+1;
    tm.Day=atoi(__DATE__+4);
    tm.Year=CalendarYrToTm(atoi(__DATE__+7));
    tm.Hour=atoi(__TIME__);
    tm.Minute=atoi(__TIME__+3);
    tm.Second=atoi(__TIME__+6);
    return makeTime(tm);
}

// =============================================================
// MQTT CALLBACK
// =============================================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {

    char buffer[64];
    if (length >= sizeof(buffer)) length = sizeof(buffer) - 1;

    memcpy(buffer, payload, length);
    buffer[length] = '\0';

    String value = String(buffer);

    if (String(topic) == "chickenpark/newsetpoint") {
        float v = value.toFloat();
        if (v >= 10 && v <= 40) {
            setPoint = v;
            EEPROM.put(EEPROM_SETP_ADDR,setPoint);
            EEPROM.commit();
            snprintf(msg,256,"New setpoint value received via MQT: %.1f", setPoint);
            logPrint(msg);
        }
    }

    if (String(topic) == "chickenpark/newhysteresis") {
        float v = value.toFloat();
        if (v >= minHysteresis && v <= maxHysteresis) {
            hysteresis = v;
            EEPROM.put(EEPROM_HYST_ADDR,hysteresis);
            EEPROM.commit();
            snprintf(msg,256,"New histerese value received via MQTT: %.1f", hysteresis);
            logPrint(msg);
        }
    }
}

// =============================================================
// MQTT CONNECT
// =============================================================
void ensureMqttConnected(unsigned long now) {

    if (!wifiReady) return;

    if (mqttClient.connected()) {
        mqttReady = true;
        mqttRetryInterval = MQTT_RETRY_BASE;
        return;
    }

    if (now - lastMqttAttempt < mqttRetryInterval) return;

    lastMqttAttempt = now;
    logPrint("Tentando ligar MQTT...");

    bool ok = mqttClient.connect(
        "ChickenParkESP",
        mqttUser,
        mqttPass,
        "chickenpark/status",
        1,
        true,
        "offline"
    );

    if (ok) {
        mqttReady = true;
        mqttRetryInterval = MQTT_RETRY_BASE;

        mqttClient.publish("chickenpark/status","online",true);
        mqttClient.subscribe("chickenpark/newsetpoint");
        mqttClient.subscribe("chickenpark/newhysteresis");

        logPrint("MQTT conectado com sucesso");
    } else {
        mqttReady = false;
        snprintf(msg,256,"Falha MQTT: %d",mqttClient.state());
        logPrint(msg);

        mqttRetryInterval *= 2;
        if (mqttRetryInterval > MQTT_RETRY_MAX)
            mqttRetryInterval = MQTT_RETRY_MAX;
    }
}

// =============================================================
// SETUP
// =============================================================
void setup(){
    Serial.begin(115200);

    EEPROM.begin(512);

    EEPROM.get(EEPROM_SETP_ADDR,setPoint);
    if(isnan(setPoint) || setPoint<10 || setPoint>40){
        logPrint("Setpoint invalido na EEPROM, usando default");
        setPoint=36.0;
        EEPROM.put(EEPROM_SETP_ADDR,setPoint);
        EEPROM.commit();
    } else {
        snprintf(msg,128,"Setpoint carregado da EEPROM: %.2f",setPoint);
        logPrint(msg);
    }

    EEPROM.get(EEPROM_HYST_ADDR,hysteresis);
    if(isnan(hysteresis) || hysteresis<minHysteresis || hysteresis>maxHysteresis){
        logPrint("Histerese invalida na EEPROM, usando default");
        hysteresis=1.0;
        EEPROM.put(EEPROM_HYST_ADDR,hysteresis);
        EEPROM.commit();
    } else {
        snprintf(msg,128,"Histerese carregada da EEPROM: %.2f",hysteresis);
        logPrint(msg);
    }

    for(int i=0;i<numRelays;i++){
        pinMode(relays[i],OUTPUT);
        digitalWrite(relays[i],HIGH);
    }

    sensors.begin();

    if(!display.begin(SSD1306_SWITCHCAPVCC,SCREEN_ADDRESS)){
        Serial.println(F("Falha OLED"));
        for(;;);
    }

    showSplashScreen();

    WiFi.config(localIP,dns,gateway,subnet);
    WiFi.begin(ssid,password);

    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setKeepAlive(MQTT_KEEPALIVE_SEC);
    mqttClient.setSocketTimeout(MQTT_SOCKET_TIMEOUT_SEC);

    setTime(getBuildTimeEpoch());
    logPrint("Tentando conectar Wi-Fi...");
}

// =============================================================
// LOOP
// =============================================================
void loop(){
    unsigned long now = millis();

    // ================= TEMPERATURA =================
    if(now - lastTempRead >= TEMP_READ_INTERVAL){
        lastTempRead = now;

        sensors.requestTemperatures();
        currentTemp = sensors.getTempCByIndex(0);

        if(currentTemp==-127.0 || currentTemp>maxSafetyTemp){
            digitalWrite(relays[0], HIGH);
            relayState[0]=false;
            logPrint("ALERTA: temperatura fora de limites");
        } else {
            if(!relayState[0] && currentTemp<setPoint){
                digitalWrite(relays[0],LOW);
                relayState[0]=true;
            }
            if(relayState[0] && currentTemp>=(setPoint+hysteresis)){
                digitalWrite(relays[0],HIGH);
                relayState[0]=false;
            }
        }

        showMainScreen(currentTemp, relayState[0]);

        snprintf(msg,256,"Temp: %.1f | Calor: %s",
                 currentTemp,
                 relayState[0]?"ON":"OFF");
        logPrint(msg);

        if (mqttClient.connected() && currentTemp > 0){
            dtostrf(currentTemp,4,2,msg);
            mqttClient.publish("chickenpark/temperatura",msg);

            dtostrf(setPoint,4,2,msg);
            mqttClient.publish("chickenpark/setpoint",msg);

            sprintf(msg, "%d", relayState[0]);
            mqttClient.publish("chickenpark/relay1", msg);

            dtostrf(hysteresis,4,2,msg);
            mqttClient.publish("chickenpark/hysteresis",msg);

            dtostrf((float)millis(),8,0,msg);
            mqttClient.publish("chickenpark/uptime_ms",msg);
        }
    }

    // ================= WIFI =================
    static bool lastWifiState = false;
    bool currentWifiState = (WiFi.status() == WL_CONNECTED);

    if(currentWifiState && !lastWifiState){
        wifiReady=true;
        ntpReady=false;
        logPrint("Wi-Fi conectado com sucesso");
    }

    if(!currentWifiState && lastWifiState){
        wifiReady=false;
        mqttReady=false;
        logPrint("Wi-Fi perdeu ligação");
    }

    lastWifiState = currentWifiState;

    // ================= MQTT =================
    ensureMqttConnected(now);
    if (wifiReady) mqttClient.loop();

    // ================= WEB =================
    if(wifiReady && !webReady){
        server.on("/",handleWebRoot);
        server.begin();
        webReady=true;
        logPrint("Webserver iniciado");
    }
    if(webReady) server.handleClient();

    // ================= NTP =================
    if(wifiReady && !ntpReady && now - lastNtpAttempt > NTP_RETRY_INTERVAL){
        lastNtpAttempt = now;
        configTime(0,0,"pt.pool.ntp.org");
        struct tm timeinfo;
        if(getLocalTime(&timeinfo)){
            tmElements_t tmElem;
            tmElem.Second = timeinfo.tm_sec;
            tmElem.Minute = timeinfo.tm_min;
            tmElem.Hour   = timeinfo.tm_hour;
            tmElem.Day    = timeinfo.tm_mday;
            tmElem.Month  = timeinfo.tm_mon + 1;
            tmElem.Year   = CalendarYrToTm(timeinfo.tm_year + 1900);

            setTime(makeTime(tmElem));
            ntpReady = true;
            logPrint("Hora NTP sincronizada com sucesso");
        } else {
            logPrint("Falha NTP, tentaremos mais tarde");
        }
    }
}