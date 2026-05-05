#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <ArduinoOTA.h>

// =====================================================
// WIFI CONFIG
// =====================================================
#define USE_DEV_NETWORK 0

static unsigned long lastAttempt = 0;


// =====================================================
// REDE
// =====================================================
#if   USE_DEV_NETWORK  
const char* ssid = "Quintinha_Dos_Lirios_Sala";
const char* password = "!fsnunes2020!";
IPAddress localIP(192,168,0,33);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,0,1);
const char* mqttServer = "192.168.0.7";
#else
const char* ssid = "QuintinhaDosLirios";
const char* password = "Lirios!2025#";
IPAddress localIP(192,168,1,33);
IPAddress gateway(192,168,1,254);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,1,254);
const char* mqttServer = "quintinhadoslirios.ddns.net";
#endif

unsigned long wifiConnectedAt = 0;
bool wifiReady = false;
#define WIFI_STABILIZE_TIME 3000

// =====================================================
// PROTOTYPES
// =====================================================
void mqttCallback(char* topic, byte* payload, unsigned int length);
void handleRoot();
void handleWiFi();
void updateRelays();
void showDisplay();
void publishMQTT();

// =====================================================
// RELAYS
// =====================================================
const int relays[] = {25, 14, 27, 26};

#define RELAY_CLEAN 3
#define RELAY_MOTOR 1
#define RELAY_PUMP 0

// =====================================================
// EEPROM
// =====================================================
#define EEPROM_INTERVAL_ADDR 0
#define EEPROM_DURATION_ADDR 10

#define CLEAN_INTERVAL 7

int cleanIntervalSec = 3 * CLEAN_INTERVAL * 60; // 3*motorIntervalSec
int cleanDurationSec  = 68; // 1 volta completa + 1/8 volta

int motorIntervalSec = CLEAN_INTERVAL * 60; //
int motorDurationSec  = 12;

// =====================================================
// STATE / TIMERS
// =====================================================

bool showCleaning = true;

unsigned long lastSensorRead= 0;

unsigned long lastMotorChangeMs = 0;
unsigned long lastMotorStartMs  = 0;

unsigned long lastCycleChangeMs = 0;
unsigned long lastStateStartMs  = 0;
unsigned long lastDisplay = 0;

bool cleanActive = false;
bool motorActive = false;

// =====================================================
// MQTT
// =====================================================
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char* mqttUser = "termosifao";
const char* mqttPass = "Kusku2026";
const int mqttPort = 1883;

static unsigned long lastMQTTPublish = 0;
const unsigned long MQTT_PUBLISH_INTERVAL = 10000; // 1 minuto
// =====================================================
// WEB
// =====================================================
WebServer server(80);

// =====================================================
// DISPLAY
// =====================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// =====================================================
// MQTT CALLBACK
// =====================================================
void mqttCallback(char* topic, byte* payload, unsigned int length){

    char buf[32];
    if(length >= sizeof(buf)) length = sizeof(buf) - 1;

    memcpy(buf, payload, length);
    buf[length] = 0;

    int v = atoi(buf);

    if(String(topic) == "filtro/newCleanInterval"){
        if(v >= 10 && v <= 3600){
            cleanIntervalSec = v;
            EEPROM.put(EEPROM_INTERVAL_ADDR, cleanIntervalSec);
            EEPROM.commit();
        }
    }

    if(String(topic) == "filtro/newCleanDuration"){
        if(v >= 1 && v <= 600){
            cleanDurationSec = v;
            EEPROM.put(EEPROM_DURATION_ADDR, cleanDurationSec);
            EEPROM.commit();
        }
    }
}

// =====================================================
// MQTT PUBLISH
// =====================================================
void publishMQTT(){

    if(!mqttClient.connected()) return;

    char msg[32];

    mqttClient.publish("filtro/state", cleanActive ? "CLEAN" : "WAIT", true);

    unsigned long now = millis();
    unsigned long elapsed = (now - lastStateStartMs) / 1000;

    unsigned long remaining;

    if(cleanActive)
        remaining = (cleanDurationSec > elapsed) ? cleanDurationSec - elapsed : 0;
    else
        remaining = (cleanIntervalSec > elapsed) ? cleanIntervalSec - elapsed : 0;

    sprintf(msg, "%lu", remaining);
    mqttClient.publish("filtro/remaining_sec", msg, true);

    sprintf(msg, "%lu", millis() / 1000);
    mqttClient.publish("filtro/uptime_sec", msg, true);
}

// =====================================================
// WEB
// =====================================================
void handleRoot(){

    if(server.hasArg("interval")){
        int v = server.arg("interval").toInt();
        if(v >= 10 && v <= 3600){
            cleanIntervalSec = v;
            EEPROM.put(EEPROM_INTERVAL_ADDR, cleanIntervalSec);
            EEPROM.commit();
        }
    }

    if(server.hasArg("duration")){
        int v = server.arg("duration").toInt();
        if(v >= 1 && v <= 600){
            cleanDurationSec = v;
            EEPROM.put(EEPROM_DURATION_ADDR, cleanDurationSec);
            EEPROM.commit();
        }
    }

    String html = "<html><h1>Filtro Lago</h1>";
    html += "<p>Estado: " + String(cleanActive ? "CLEAN" : "WAIT") + "</p>";
    html += "<p>Intervalo: " + String(cleanIntervalSec) + " s</p>";
    html += "<p>Duracao: " + String(cleanDurationSec) + " s</p>";

    html += "<form>";
    html += "Intervalo: <input name='interval'><br>";
    html += "Duracao: <input name='duration'><br>";
    html += "<input type='submit'></form>";

    html += "</html>";

    server.send(200, "text/html", html);
}

void WiFiEvent(WiFiEvent_t event) {
  switch(event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("🔌 WiFi foi desligado");
      break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("🔗 Ligado ao AP");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("✅ IP obtido!");
      Serial.println(WiFi.localIP());
      break;

    default:
      break;
  }
}


// =====================================================
// WIFI (SAFE RECONNECT)
// =====================================================
void handleWiFi(){

    if(millis() - lastAttempt < 10000)
        return;

    lastAttempt = millis();

    if(WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\n✅ Ligado com sucesso!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("RSSI: ");
        Serial.println(WiFi.RSSI());
        Serial.println();
        return;
    }

    lastAttempt = millis();

    Serial.println("[WiFi] reconnect...");

    WiFi.disconnect(false);
    delay(200);


    if (!WiFi.config(localIP, gateway, subnet, dns))
    {
     Serial.println("❌ Falha ao configurar IP fixo");
    }


    #if USE_DEV_NETWORK
    WiFi.begin(ssid, password);
    #else
    //WiFi.onEvent(WiFiEvent);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    //WiFi.begin(ssid);
    WiFi.begin(ssid, password);
    #endif

    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("❌ SSID não encontrado");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("❌ Falha de autenticação");
        break;
      case WL_IDLE_STATUS:
        Serial.println("⏳ Idle...");
        break;
      case WL_DISCONNECTED:
        Serial.println("🔌 Desligado");
        break;
      default:
        Serial.println("ℹ️ Estado desconhecido");
    }
}

// =====================================================
// OTA Setup
// =====================================================
void setupOTA() {

  ArduinoOTA.setHostname("filtro-esp32");

  ArduinoOTA.onStart([]() {
    Serial.println("🚀 OTA Start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\n✅ OTA End");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress * 100) / total);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("❌ OTA Error[%u]\n", error);
  });

  ArduinoOTA.begin();

  Serial.println("📡 OTA Ready");
}

// =====================================================
// RELAYS
// =====================================================
void updateRelays(){

    if (cleanActive)
    {
        // limpeza ativa
        digitalWrite(relays[RELAY_CLEAN], LOW);
        digitalWrite(relays[RELAY_MOTOR], LOW);
    }
    else
    {
        digitalWrite(relays[RELAY_CLEAN], HIGH);
    
        digitalWrite(relays[RELAY_MOTOR], motorActive ? LOW: HIGH);
    }

    // bomba sempre ligada
    digitalWrite(relays[RELAY_PUMP], LOW);
}

// =====================================================
// DISPLAY
// =====================================================
void showDisplay(){

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    unsigned long now = millis();

    if (showCleaning)
    {
     showCleaning = false;

     unsigned long elapsed = (now - lastStateStartMs) / 1000;

     unsigned long remaining;
     char mode = cleanActive ? 'C' : 'W';

     if(cleanActive)
        remaining = (cleanDurationSec > elapsed) ? cleanDurationSec - elapsed : 0;
     else
        remaining = (cleanIntervalSec > elapsed) ? cleanIntervalSec - elapsed : 0;

     display.setTextSize(2);
     display.setCursor(10, 20);

     display.print(mode);
     display.print(" ");
     display.print(seconds2Time(remaining));

     display.display();
   }
  else
   {
     showCleaning = true;


    unsigned long motorElapsed;
    unsigned long motorRemaining;

    if(motorActive)
        motorElapsed = (millis() - lastMotorStartMs) / 1000;
    else
        motorElapsed = (millis() - lastMotorChangeMs) / 1000;

    if(motorActive)
        motorRemaining = (motorDurationSec > motorElapsed) ? motorDurationSec - motorElapsed : 0;
    else
        motorRemaining = (motorIntervalSec > motorElapsed) ? motorIntervalSec - motorElapsed : 0;

     display.setTextSize(2);
     display.setCursor(10, 20);

     display.print("R");
     display.print(" ");
     display.print(seconds2Time(motorRemaining));

     display.display();
   }
}

void scanNetworks() {
  Serial.println("\n🔍 A fazer scan de redes...");

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("Nenhuma rede encontrada.");
    return;
  }

  Serial.printf("Encontradas %d redes:\n\n", n);

  for (int i = 0; i < n; ++i) {
    Serial.printf("%2d: SSID: %-30s | RSSI: %4d dBm | Canal: %2d | Segurança: %s\n",
      i + 1,
      WiFi.SSID(i).c_str(),
      WiFi.RSSI(i),
      WiFi.channel(i),
      encryptionType(WiFi.encryptionType(i)).c_str()
    );
  }
}

String encryptionType(wifi_auth_mode_t type) {
  switch (type) {
    case WIFI_AUTH_OPEN: return "OPEN";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA_PSK";
    case WIFI_AUTH_WPA2_PSK: return "WPA2_PSK";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA_WPA2_PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2_ENTERPRISE";
    case WIFI_AUTH_WPA3_PSK: return "WPA3_PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2_WPA3_PSK";
    default: return "UNKNOWN";
  }
}


// =====================================================
// SETUP
// =====================================================
void setup(){

    Serial.begin(115200);

    Wire.begin(21, 22);

    EEPROM.begin(512);

    //EEPROM.get(EEPROM_INTERVAL_ADDR, cleanIntervalSec); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //EEPROM.get(EEPROM_DURATION_ADDR, cleanDurationSec); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if(cleanIntervalSec < 10 || cleanIntervalSec > 3600) cleanIntervalSec = 300;
    if(cleanDurationSec < 1 || cleanDurationSec > 600) cleanDurationSec = 60;

    for(int i=0;i<4;i++){
        pinMode(relays[i], OUTPUT);
        digitalWrite(relays[i], HIGH);
    }

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("Ver 1.0");
    display.display();

    delay(2000);

    setupSensors();

    //WiFi.config(localIP, dns, gateway, subnet);
   if (!WiFi.config(localIP, gateway, subnet, dns))
    {
     Serial.println("❌ Falha ao configurar IP fixo!");
    }
    #if USE_DEV_NETWORK
    WiFi.begin(ssid, password);
    #else
    //WiFi.onEvent(WiFiEvent);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(ssid, password);
    //WiFi.begin(ssid);
    #endif

    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(mqttCallback);

    server.on("/", handleRoot);
    server.begin();

    setupOTA();

  /* Limpeza inicial */

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("Cleaning");
    display.display();

    Serial.print(F("Limpeza inicial em curso..."));

    digitalWrite(relays[RELAY_PUMP], LOW);
    digitalWrite(relays[RELAY_CLEAN], LOW);
    digitalWrite(relays[RELAY_MOTOR], LOW);
    delay(60000);
    digitalWrite(relays[RELAY_CLEAN], HIGH);
    digitalWrite(relays[RELAY_CLEAN], HIGH);
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("Filtering");
    display.display();

    Serial.print(F("Limpeza inicial terminada"));

    lastCycleChangeMs = millis();
    lastStateStartMs = millis();

    lastMotorChangeMs = millis();
    lastMotorStartMs = millis();

    scanNetworks();

}


void printTime(unsigned long seconds)
{
    unsigned int m = seconds / 60;
    unsigned int s = seconds % 60;

    if(m < 10) Serial.print("0");
    Serial.print(m);
    Serial.print(":");
    if(s < 10) Serial.print("0");
    Serial.print(s);
}

void showStatus()
{
    // =================================================
    // LOGICA MOTOR
    // =================================================

    unsigned long motorElapsed;
    unsigned long motorRemaining;

    if(motorActive)
        motorElapsed = (millis() - lastMotorStartMs) / 1000;
    else
        motorElapsed = (millis() - lastMotorChangeMs) / 1000;

    if(motorActive)
        motorRemaining = (motorDurationSec > motorElapsed) ? motorDurationSec - motorElapsed : 0;
    else
        motorRemaining = (motorIntervalSec > motorElapsed) ? motorIntervalSec - motorElapsed : 0;

    // =========================
    // DEBUG MOTOR
    // =========================
    Serial.print(F("Motor: "));
    if(motorActive)
    {
        Serial.print(F("LIGADO | Tempo restante: "));
        printTime(motorRemaining);
        Serial.println();
    }
    else
    {
        Serial.print(F("DESLIGADO | Tempo para iniciar: "));
        printTime(motorRemaining);
        Serial.println();
    }


    // =================================================
    // LOGICA LIMPEZA
    // =================================================

    unsigned long cleanElapsed;
    unsigned long cleanRemaining;

    if(cleanActive)
        cleanElapsed = (millis() - lastStateStartMs) / 1000;
    else
        cleanElapsed = (millis() - lastCycleChangeMs) / 1000;

    if(cleanActive)
        cleanRemaining = (cleanDurationSec > cleanElapsed) ? cleanDurationSec - cleanElapsed : 0;
    else
        cleanRemaining = (cleanIntervalSec > cleanElapsed) ? cleanIntervalSec - cleanElapsed : 0;

    // =========================
    // DEBUG LIMPEZA
    // =========================
    Serial.print(F("Limpeza: "));
    if(cleanActive)
    {
        Serial.print(F("ATIVA | Tempo restante: "));
        printTime(cleanRemaining);
        Serial.println();
    }
    else
    {
        Serial.print(F("INATIVA | Tempo para iniciar: "));
        printTime(cleanRemaining);
        Serial.println();
    }


    Serial.println(F("-----------------------------"));
}

void printWiFiStatus(){

    wl_status_t st = WiFi.status();

    Serial.print("[WiFi] estado: ");

    switch(st){

        case WL_IDLE_STATUS:
            Serial.println("IDLE");
            break;

        case WL_NO_SSID_AVAIL:
            Serial.println("SSID nao encontrado");
            break;

        case WL_SCAN_COMPLETED:
            Serial.println("SCAN COMPLETO");
            break;

        case WL_CONNECTED:
            Serial.println("CONECTADO");
            Serial.print("[WiFi] IP: ");
            Serial.println(WiFi.localIP());
            Serial.print("[WiFi] RSSI: ");
            Serial.println(WiFi.RSSI());
            break;

        case WL_CONNECT_FAILED:
            Serial.println("FALHA DE LIGACAO");
            break;

        case WL_CONNECTION_LOST:
            Serial.println("LIGACAO PERDIDA");
            break;

        case WL_DISCONNECTED:
            Serial.println("DESCONECTADO");
            break;

        default:
            Serial.println("DESCONHECIDO");
            break;
    }
}

// =====================================================
// LOOP
// =====================================================
void loop(){

    ArduinoOTA.handle(); 

     loopSensors();

    unsigned long now = millis();

    if(now - lastDisplay > 2000){
        lastDisplay = now;
        showDisplay();
    }

    handleWiFi();

    if(WiFi.status() == WL_CONNECTED)
    {
        if(!wifiReady)
        {
         wifiConnectedAt = millis();
         wifiReady = true;
        }
    }
    else
    {
        wifiReady = false;
    }

    // MQTT
    if(wifiReady && (millis() - wifiConnectedAt > WIFI_STABILIZE_TIME))
    {

      if(!mqttClient.connected())
       {

        Serial.println("🔄 MQTT reconnect...");

        if(mqttClient.connect("filtroESP", mqttUser, mqttPass))
        {
            Serial.println("✅ MQTT ligado");

            mqttClient.subscribe("filtro/newCleanInterval");
            mqttClient.subscribe("filtro/newCleanDuration");

            mqttClient.publish("filtro/status", "online", true);
        }
    }

    mqttClient.loop();
    server.handleClient();
    }

    // =================================================
    // LOGICA Motor
    // =================================================

    unsigned long elapsed;
    unsigned long remaining;


    if(motorActive)
        elapsed = (millis() - lastMotorStartMs) / 1000;
    else
        elapsed = (millis() - lastMotorChangeMs) / 1000;

    if(motorActive)
        remaining = (motorDurationSec > elapsed) ? motorDurationSec - elapsed : 0;
    else
        remaining = (motorIntervalSec > elapsed) ? motorIntervalSec - elapsed : 0;

    // =====================================================
    // TRANSIÇÃO ÚNICA E LIMPA
    // =====================================================
    if(!motorActive && remaining == 0)
    {
        motorActive = true;
        lastMotorStartMs = millis();
        publishMQTT();
    }
     else
    if(motorActive && remaining == 0)
    {
        motorActive = false;
        lastMotorChangeMs = millis();
        publishMQTT();
    }

    // =================================================
    // LOGICA LIMPEZA
    // =================================================


    if(cleanActive)
        elapsed = (millis() - lastStateStartMs) / 1000;
    else
        elapsed = (millis() - lastCycleChangeMs) / 1000;

    if(cleanActive)
        remaining = (cleanDurationSec > elapsed) ? cleanDurationSec - elapsed : 0;
    else
        remaining = (cleanIntervalSec > elapsed) ? cleanIntervalSec - elapsed : 0;

    // =====================================================
    // TRANSIÇÃO ÚNICA E LIMPA
    // =====================================================
    if(!cleanActive && remaining == 0)
    {
        cleanActive = true;
        lastStateStartMs = millis();
        publishMQTT();
    }
     else
    if(cleanActive && remaining == 0)
    {
        cleanActive = false;
        lastCycleChangeMs = millis();
        publishMQTT();
    }

    // =====================================================
    // RELAYS
    // =====================================================
    
    updateRelays();
    
    // ================= MQTT PUBLISH 1 MIN =================
    
    if(millis() > lastMQTTPublish + 10000){
        lastMQTTPublish = millis();
        showStatus();
        publishMQTT();
    }

}

