#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <WebServer.h>

// =====================================================
// WIFI CONFIG
// =====================================================
#define USE_DEV_NETWORK 0

static unsigned long lastAttempt = 0;

// =====================================================
// REDE
// =====================================================
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
const char* password = "Lirios!2025#";
IPAddress localIP(192,168,1,33);
IPAddress gateway(192,168,1,254);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,1,254);
const char* mqttServer = "192.168.1.50";
#endif



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
const int relays[] = {16, 17, 25, 26};

#define RELAY_CLEAN 3
#define RELAY_MOTOR 2
#define RELAY_PUMP 0

// =====================================================
// EEPROM
// =====================================================
#define EEPROM_INTERVAL_ADDR 0
#define EEPROM_DURATION_ADDR 10

int cleanIntervalSec = 300;
int cleanDurationSec  = 60;

// =====================================================
// STATE / TIMERS
// =====================================================
unsigned long lastCycleChangeMs = 0;
unsigned long lastStateStartMs  = 0;
unsigned long lastDisplay = 0;

bool cleanActive = false;

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

// =====================================================
// WIFI (SAFE RECONNECT)
// =====================================================
void handleWiFi(){

    if(WiFi.status() == WL_CONNECTED)
        return;

    if(millis() - lastAttempt < 10000)
        return;

    lastAttempt = millis();

    Serial.println("[WiFi] reconnect...");

    WiFi.disconnect(true);
    delay(200);
    WiFi.begin(ssid, password);
}

// =====================================================
// RELAYS
// =====================================================
void updateRelays(){

    // limpeza ativa
    digitalWrite(relays[RELAY_CLEAN], cleanActive ? LOW : HIGH);

    // motor sempre ligado
    digitalWrite(relays[RELAY_MOTOR], LOW);

    // bomba sempre ligada
    digitalWrite(relays[RELAY_PUMP], LOW);

    // relay 3 invertido como pedido
    digitalWrite(relays[3], cleanActive ? LOW : HIGH);
}

// =====================================================
// DISPLAY
// =====================================================
void showDisplay(){

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    unsigned long now = millis();
    unsigned long elapsed = (now - lastStateStartMs) / 1000;

    unsigned long remaining;
    char mode = cleanActive ? 'C' : 'W';

    if(cleanActive)
        remaining = (cleanDurationSec > elapsed) ? cleanDurationSec - elapsed : 0;
    else
        remaining = (cleanIntervalSec > elapsed) ? cleanIntervalSec - elapsed : 0;

    display.setTextSize(3);
    display.setCursor(10, 20);

    display.print(mode);
    display.print(" ");
    display.print(remaining);
    display.print(" s");   // espaço antes do s

    display.display();
}

// =====================================================
// SETUP
// =====================================================
void setup(){

    Serial.begin(115200);

    EEPROM.begin(512);

    EEPROM.get(EEPROM_INTERVAL_ADDR, cleanIntervalSec);
    EEPROM.get(EEPROM_DURATION_ADDR, cleanDurationSec);

    if(cleanIntervalSec < 10 || cleanIntervalSec > 3600) cleanIntervalSec = 300;
    if(cleanDurationSec < 1 || cleanDurationSec > 600) cleanDurationSec = 60;

    for(int i=0;i<4;i++){
        pinMode(relays[i], OUTPUT);
        digitalWrite(relays[i], HIGH);
    }

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    WiFi.config(localIP, dns, gateway, subnet);
    WiFi.begin(ssid, password);

    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(mqttCallback);

    server.on("/", handleRoot);
    server.begin();

    lastCycleChangeMs = millis();
    lastStateStartMs = millis();
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

    unsigned long now = millis();

    if(now - lastDisplay > 1000){
        lastDisplay = now;
        showDisplay();
    }

    handleWiFi();

    // MQTT
    if(WiFi.status() == WL_CONNECTED){

        if(!mqttClient.connected()){

            if(mqttClient.connect("filtroESP", mqttUser, mqttPass)){

                mqttClient.subscribe("filtro/newCleanInterval");
                mqttClient.subscribe("filtro/newCleanDuration");

                mqttClient.publish("filtro/status", "online", true);
            }
        }

        mqttClient.loop();
        server.handleClient();
    }

    // =================================================
    // LOGICA LIMPEZA
    // =================================================

    unsigned long elapsed;
    unsigned long remaining;


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
    
    if(millis() - lastMQTTPublish >= 10000){
        lastMQTTPublish = millis();
        publishMQTT();
    }

}