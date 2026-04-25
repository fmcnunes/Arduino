#include <Servo.h>

Servo meuServo;
String entrada = "";

const int trigPin = 10;
const int echoPin = 9;
float duration, distance;

float valores[4];     // guarda os últimos 4 valores
int indice = 0;       // posição atual
bool cheio = false;   // indica se já temos 4 valores

unsigned long ultimaAmostragem = 0;
unsigned long ultimoDisplay = 0;
unsigned long ultimoSpray = 0;


void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  meuServo.attach(12);

  meuServo.write(0);

  Serial.println("Teste do Servo");
}


void loop() {
  
  unsigned long agora = millis();

  if (agora - ultimaAmostragem >= 100)
  {
    ultimaAmostragem = agora;

    getDistance();   // chama a função a cada 100 ms
  }

  if (agora - ultimoDisplay >= 1000)
  {
    ultimoDisplay = agora;

    Serial.print("Distance: ");
    Serial.println(distanciaMedia());
  }

  if (distanciaMedia() < 20 && (agora - ultimoSpray >= 10000))
  {
    ultimoSpray = agora;
    spray();
  }
}

void adicionarValor(float v) {
  valores[indice] = v;
  indice++;

  if (indice >= 4) {
    indice = 0;
    cheio = true;
  }
}


void spray()
{
    Serial.print(millis());
    Serial.println("Movendo para o Topo");
    meuServo.write(0);
    delay(100);
    Serial.print(millis());
    Serial.println("Movendo para baixo");
    meuServo.write(180);
    delay(1000);
    Serial.println("Movendo para o Topo");
    meuServo.write(0);

}

float distanciaMedia() {
  int n = cheio ? 4 : indice;
  if (n == 0) return 0;

  float soma = 0;
  for (int i = 0; i < n; i++) {
    soma += valores[i];
  }

  return soma / n;
}

void getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;

  adicionarValor(distance);
}