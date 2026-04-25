#include <WiFi.h>

const char* ssid = "QuintinhaDosLirios";
const char* password = "Lirios!2025#";

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

void connectWiFi() {
  Serial.println("\n🔌 A tentar ligar ao WiFi...");
  Serial.printf("SSID: %s\n", ssid);

  WiFi.disconnect(true);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempt = 0;

  while (WiFi.status() != WL_CONNECTED) {
    attempt++;

    Serial.printf("Tentativa #%d - Estado: %d\n", attempt, WiFi.status());

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

    delay(2000);

    if (attempt >= 20) {
      Serial.println("\n⚠️ Muitas tentativas falhadas. Reiniciar tentativa...\n");
      WiFi.disconnect();
      delay(2000);
      WiFi.begin(ssid, password);
      attempt = 0;
    }
  }

  Serial.println("\n✅ Ligado com sucesso!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n🚀 ESP32 WiFi Debug Tool");

  scanNetworks();
}

void loop() {
  connectWiFi();

  Serial.println("\n🔁 Nova tentativa em 10 segundos...\n");
  delay(10000);
}