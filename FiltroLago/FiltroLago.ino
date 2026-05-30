#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <WebServer.h>

//
// =====================================================
// WIFI CONFIG
// =====================================================
#define USE_DEV_NETWORK 0

static unsigned long lastAttempt = 0;


static unsigned long debugTime = 0;

// =====================================================
// REDE
// =====================================================
#if USE_DEV_NETWORK
const char* ssid = "Quintinha_Dos_Lirios_Sala";
const char* password = "!fsnunes2020!";
IPAddress localIP(192, 168, 0, 33);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 0, 1);
const char* mqttServer = "192.168.0.7";
#else
const char* ssid = "QuintinhaDosLirios";
const char* password = "Lirios!2025#";
IPAddress localIP(192, 168, 1, 33);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 254);
const char* mqttServer = "quintinhadoslirios.ddns.net";
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
const int relays[] = { 25, 14, 27, 26 };

#define RELAY_CLEAN 3
#define RELAY_MOTOR 1
#define RELAY_PUMP 0

// =====================================================
// SENSOR NIVEL
// =====================================================
#define LEVEL_SENSOR_PIN 17

const unsigned long SENSOR_IGNORE_TIME = 5000;
unsigned long ignoreSensorUntil = 0;

  bool rawLevel = true;

// =====================================================
// TEMPOS
// =====================================================

const unsigned long FULL_CLEAN_TIME = 45000;
const unsigned long QUARTER_ROTATION_TIME = FULL_CLEAN_TIME / 4;

// =====================================================
// ESTADOS
// =====================================================
enum FilterState {
  STATE_NORMAL,
  STATE_QUARTER_ROTATION,
  STATE_CLEAN
};

FilterState state = STATE_NORMAL;

unsigned long stateStartMs = 0;

uint8_t quarterRotations = 0;

static bool previousLevelHigh = false;

const unsigned long CLEAN_COOLDOWN_TIME = 60000;  // 60 segundos
unsigned long lastCleanTriggerMs = 0;
unsigned long lastCleanDurationMs = 0;


// =====================================================
// EEPROM
// =====================================================
#define EEPROM_INTERVAL_ADDR 0
#define EEPROM_DURATION_ADDR 10

//int cleanIntervalSec = 300;
//int cleanDurationSec  = 60;

// =====================================================
// STATE / TIMERS
// =====================================================
//unsigned long lastCycleChangeMs = 0;
//unsigned long lastStateStartMs  = 0;
unsigned long lastDisplay = 0;

//bool cleanActive = false;

// =====================================================
// MQTT
// =====================================================
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char* mqttUser = "termosifao";
const char* mqttPass = "Kusku2026";
const int mqttPort = 1883;

static unsigned long lastMQTTPublish = 0;
const unsigned long MQTT_PUBLISH_INTERVAL = 10000;  // 1 minuto

static unsigned long lastMQTTAttempt = 0;
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
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  char buf[32];
  if (length >= sizeof(buf)) length = sizeof(buf) - 1;

  memcpy(buf, payload, length);
  buf[length] = 0;

  int v = atoi(buf);

  /*
    if(String(topic) == "filtro/newCleanInterval"){
        if(v >= 10 && v <= 3600){
            cleanIntervalSec = v;
            EEPROM.put(EEPROM_INTERVAL_ADDR, cleanIntervalSec);
            EEPROM.commit();
        }
    }*/
}

// =====================================================
// MQTT PUBLISH
// =====================================================
void publishMQTT() {
  if (!mqttClient.connected())
    return;

  char buf[32];

  // uptime
  sprintf(buf, "%lu", millis() / 1000);
  mqttClient.publish("filtro/uptime_sec", buf, true);

  // heap livre
  sprintf(buf, "%u", ESP.getFreeHeap());
  mqttClient.publish("filtro/free_heap", buf, true);

  // wifi rssi
  sprintf(buf, "%d", WiFi.RSSI());
  mqttClient.publish("filtro/wifi_rssi", buf, true);

  // estado
  switch (state) {
    case STATE_NORMAL:
      mqttClient.publish("filtro/state", "NORMAL", true);
      break;

    case STATE_QUARTER_ROTATION:
      mqttClient.publish("filtro/state", "ROTATING", true);
      break;

    case STATE_CLEAN:
      mqttClient.publish("filtro/state", "CLEANING", true);
      break;
  }

  unsigned long lastCleanIntervalSec =
    lastCleanDurationMs / 1000;

  sprintf(buf, "%lu", lastCleanIntervalSec);

  mqttClient.publish(
    "filtro/last_clean_interval_sec",
    buf,
    true);

  mqttClient.publish(
    "filtro/levelSensor",
    rawLevel ? "1" : "0",
    true);


  // quarters
  sprintf(buf, "%u", quarterRotations);
  mqttClient.publish("filtro/quarters", buf, true);

  // sensor nivel
  mqttClient.publish(
    "filtro/level_high",
    digitalRead(LEVEL_SENSOR_PIN) == LOW ? "1" : "0",
    true);
}

// =====================================================
// WEB
// =====================================================
void handleRoot() {
  String html;

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";

  html += "<meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";

  html += "<title>Filtro Lago</title>";

  html += "<style>";
  html += "body{font-family:Arial;background:#111;color:#eee;padding:20px;}";
  html += "h1{color:#4fc3f7;}";
  html += ".card{background:#1e1e1e;padding:15px;margin-bottom:15px;border-radius:10px;}";
  html += ".ok{color:#4caf50;}";
  html += ".warn{color:#ff9800;}";
  html += ".bad{color:#f44336;}";
  html += "table{width:100%;border-collapse:collapse;}";
  html += "td{padding:6px;border-bottom:1px solid #333;}";
  html += "</style>";

  html += "<meta http-equiv='refresh' content='5'>";

  html += "</head>";
  html += "<body>";

  html += "<h1>Filtro Lago</h1>";

  // =====================================================
  // ESTADO
  // =====================================================

  html += "<div class='card'>";
  html += "<h2>Estado</h2>";

  html += "<table>";

  html += "<tr><td>Estado</td><td>";

  switch (state) {
    case STATE_NORMAL:
      html += "<span class='ok'>NORMAL</span>";
      break;

    case STATE_QUARTER_ROTATION:
      html += "<span class='warn'>ROTATING</span>";
      break;

    case STATE_CLEAN:
      html += "<span class='bad'>CLEANING</span>";
      break;
  }

  html += "</td></tr>";

  html += "<tr><td>Level Sensor</td><td>";
  html += String(rawLevel);
  html += "</td></tr>";

  html += "<tr><td>Quarter Rotations</td><td>";
  html += String(quarterRotations);
  html += "</td></tr>";

  html += "<tr><td>Sensor Nivel</td><td>";

  if (digitalRead(LEVEL_SENSOR_PIN) == LOW)
    html += "<span class='bad'>HIGH</span>";
  else
    html += "<span class='ok'>NORMAL</span>";

  html += "</td></tr>";

  html += "</table>";
  html += "</div>";

  // =====================================================
  // TEMPOS
  // =====================================================

  html += "<div class='card'>";
  html += "<h2>Tempos</h2>";

  html += "<table>";

  html += "<tr><td>Ultimo Intervalo</td><td>";
  html += String(lastCleanDurationMs / 1000);
  html += " s</td></tr>";

  html += "<tr><td>Cooldown</td><td>";

  unsigned long cooldownRemaining = 0;

  if (millis() - lastCleanTriggerMs < CLEAN_COOLDOWN_TIME) {
    cooldownRemaining =
      (CLEAN_COOLDOWN_TIME - (millis() - lastCleanTriggerMs)) / 1000;
  }

  html += String(cooldownRemaining);
  html += " s</td></tr>";

  html += "<tr><td>Uptime</td><td>";
  html += String(millis() / 1000);
  html += " s</td></tr>";

  html += "</table>";
  html += "</div>";

  // =====================================================
  // WIFI
  // =====================================================

  html += "<div class='card'>";
  html += "<h2>WiFi</h2>";

  html += "<table>";

  html += "<tr><td>SSID</td><td>";
  html += WiFi.SSID();
  html += "</td></tr>";

  html += "<tr><td>IP</td><td>";
  html += WiFi.localIP().toString();
  html += "</td></tr>";

  html += "<tr><td>RSSI</td><td>";
  html += String(WiFi.RSSI());
  html += " dBm</td></tr>";

  html += "</table>";
  html += "</div>";

  // =====================================================
  // ESP32
  // =====================================================

  html += "<div class='card'>";
  html += "<h2>ESP32</h2>";

  html += "<table>";

  html += "<tr><td>Free Heap</td><td>";
  html += String(ESP.getFreeHeap());
  html += "</td></tr>";

  html += "<tr><td>CPU Freq</td><td>";
  html += String(ESP.getCpuFreqMHz());
  html += " MHz</td></tr>";

  html += "<tr><td>Flash Size</td><td>";
  html += String(ESP.getFlashChipSize() / 1024 / 1024);
  html += " MB</td></tr>";

  html += "</table>";
  html += "</div>";

  // =====================================================
  // RELAYS
  // =====================================================

  html += "<div class='card'>";
  html += "<h2>Relays</h2>";

  html += "<table>";

  html += "<tr><td>Bomba</td><td>";
  html += digitalRead(relays[RELAY_PUMP]) == LOW ? "ON" : "OFF";
  html += "</td></tr>";

  html += "<tr><td>Motor</td><td>";
  html += digitalRead(relays[RELAY_MOTOR]) == LOW ? "ON" : "OFF";
  html += "</td></tr>";

  html += "<tr><td>Spray</td><td>";
  html += digitalRead(relays[RELAY_CLEAN]) == LOW ? "ON" : "OFF";
  html += "</td></tr>";

  html += "</table>";
  html += "</div>";

  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}
// =====================================================
// WIFI (SAFE RECONNECT)
// =====================================================
void handleWiFi() {

  if (WiFi.status() == WL_CONNECTED)
    return;

  if (millis() - lastAttempt < 10000)
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
void updateRelays() {
  switch (state) {
    // =================================================
    // NORMAL
    // =================================================
    case STATE_NORMAL:

      // bomba ON
      digitalWrite(relays[RELAY_PUMP], LOW);

      // motor OFF
      digitalWrite(relays[RELAY_MOTOR], HIGH);

      // spray OFF
      digitalWrite(relays[RELAY_CLEAN], HIGH);

      break;


    // =================================================
    // ROTACAO 1/4
    // =================================================
    case STATE_QUARTER_ROTATION:


      // bomba ON
      digitalWrite(relays[RELAY_PUMP], LOW);

      // motor ON
      digitalWrite(relays[RELAY_MOTOR], LOW);

      // spray OFF
      digitalWrite(relays[RELAY_CLEAN], HIGH);

      break;

    // =================================================
    // LIMPEZA TOTAL
    // =================================================
    case STATE_CLEAN:

      // bomba OFF
      digitalWrite(relays[RELAY_PUMP], LOW);

      // motor ON
      digitalWrite(relays[RELAY_MOTOR], LOW);

      // spray ON
      digitalWrite(relays[RELAY_CLEAN], LOW);

      break;
  }
}

// =====================================================
// DISPLAY
// =====================================================
void showDisplay() {


  /*
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);


  display.setTextSize(3);
  display.setCursor(10, 20);

  //display.print(mode);
  display.print(" ");
  //display.print(remaining);
  display.print(" s");  // espaço antes do s

  display.display();
  */
}

// =====================================================
// SETUP
// =====================================================
void setup() {


  pinMode(LEVEL_SENSOR_PIN, INPUT_PULLUP);

  Serial.begin(115200);

  EEPROM.begin(512);

  /*
    EEPROM.get(EEPROM_INTERVAL_ADDR, cleanIntervalSec);
    EEPROM.get(EEPROM_DURATION_ADDR, cleanDurationSec);

    if(cleanIntervalSec < 10 || cleanIntervalSec > 3600) cleanIntervalSec = 300;
    if(cleanDurationSec < 1 || cleanDurationSec > 600) cleanDurationSec = 60;
*/
   lastCleanTriggerMs = millis();

  for (int i = 0; i < 4; i++) {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], HIGH);
  }

  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  WiFi.config(localIP, dns, gateway, subnet);
  WiFi.begin(ssid, password);

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);

  server.on("/", handleRoot);
  server.begin();

  //lastCycleChangeMs = millis();
  //lastStateStartMs = millis();
}

void printWiFiStatus() {

  wl_status_t st = WiFi.status();

  Serial.print("[WiFi] estado: ");

  switch (st) {

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
void loop() {

  unsigned long now = millis();

  if (now - lastDisplay > 1000) {
    lastDisplay = now;
    showDisplay();
  }

  handleWiFi();

  // MQTT
  if (WiFi.status() == WL_CONNECTED) {

    if (!mqttClient.connected()) {
      if (now - lastMQTTAttempt > 5000) {
        lastMQTTAttempt = now;
        if (mqttClient.connect("filtroESP", mqttUser, mqttPass)) {

          mqttClient.subscribe("filtro/newCleanInterval");
          mqttClient.subscribe("filtro/newCleanDuration");

          mqttClient.publish("filtro/status", "online", true);
        }
      }
    }

    mqttClient.loop();
    server.handleClient();
  }

  // =================================================
  // LOGICA LIMPEZA
  // =================================================

 rawLevel = digitalRead(LEVEL_SENSOR_PIN) == LOW;


  static unsigned long levelStart = 0;
  static bool levelHigh = false;

  if (rawLevel) {
    if (levelStart == 0)
      levelStart = millis();

    if (millis() - levelStart > 3000)
      levelHigh = true;
  } else {
    levelStart = 0;
    levelHigh = false;
  }

  bool levelTriggered = levelHigh && !previousLevelHigh;

  previousLevelHigh = levelHigh;

  //levelTriggered = rawLevel;




  levelTriggered =  rawLevel;

  if (millis() < ignoreSensorUntil) {
    levelTriggered = false;}

    if (now - debugTime > 1000) {
    Serial.print("GPIO17 = ");
    Serial.print(rawLevel);
    Serial.print(",");
    Serial.print(levelTriggered);
    Serial.print(",");
    Serial.println(millis() - ignoreSensorUntil);
    debugTime = now;
  }

  switch (state) {
    case STATE_NORMAL:


      if (levelTriggered) {

        if (quarterRotations >= 3) {
          quarterRotations = 0;
          state = STATE_CLEAN;
          stateStartMs = now;
          lastCleanDurationMs = now - lastCleanTriggerMs;
          lastCleanTriggerMs = now;
          Serial.println("New state STATE_CLEAN");
        } else {
          state = STATE_QUARTER_ROTATION;
          quarterRotations++;
          stateStartMs = now;
          Serial.println("New state STATE_QUARTER_ROTATION");
        }
      }

      break;

    case STATE_QUARTER_ROTATION:

      if (millis() - stateStartMs > QUARTER_ROTATION_TIME)  // -> tempo para rodar 1/4 de volta
      {
        state = STATE_NORMAL;
        stateStartMs = millis();
        ignoreSensorUntil = millis() + SENSOR_IGNORE_TIME;
        Serial.println("New state STATE_NORMAL");
      }

      break;

    case STATE_CLEAN:

      // segurança
      if (millis() - stateStartMs > FULL_CLEAN_TIME)  // Tempo necessario para limpar tudo
      {
        state = STATE_NORMAL;
        stateStartMs = millis();
        Serial.println("New state STATE_NORMAL");
      }

      break;
  }

  // =====================================================
  // RELAYS
  // =====================================================

  updateRelays();

  // ================= MQTT PUBLISH 1 MIN =================

  if (millis() - lastMQTTPublish >= 10000) {
    lastMQTTPublish = millis();
    publishMQTT();
  }
}