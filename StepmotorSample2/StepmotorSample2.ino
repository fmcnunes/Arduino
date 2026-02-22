#include <Stepper.h>

const int stepsPerRevolution = 200;

const int A1A = 2; //Sentido de rotação
const int A1B = 3; //Velocidade

const int B1A = 5; //Sentido de rotação
const int B2A = 4; //Velocidade

const int OutputSensor = 6; 

const int InputSensor = 7; 

const int ledPin = LED_BUILTIN;

int sensorValue = 0;

bool sensorState = LOW;
bool lastSensorState = LOW;

double activateTime = millis();

Stepper myStepper(stepsPerRevolution, B1A, B2A, A1A, A1B);

void setup() {
 // set the speed at 60 rpm:
 myStepper.setSpeed(120);
 // initialize the serial port:
 Serial.begin(9600);

 pinMode(OutputSensor, OUTPUT);
 digitalWrite(OutputSensor, HIGH);

  pinMode(InputSensor, INPUT);


}

void loop() {
// step one revolution in one direction:
/*
Serial.println("clockwise");
Serial.println(stepsPerRevolution);
myStepper.step(stepsPerRevolution*2);
Serial.println("Done...");
delay(500);

// step one revolution in the other direction:
Serial.println("counterclockwise");
myStepper.step(-stepsPerRevolution/2);
*/
/*
Serial.println("Done...");
delay(500);*/
  sensorValue = analogRead(InputSensor); 
  Serial.println(sensorValue);
  if (sensorValue > 1000)
     sensorState = HIGH;
  else
   {
     sensorState = LOW;
     activateTime = millis();
     lastSensorState = sensorState;
     digitalWrite(ledPin, sensorState);
   }




  if (sensorState && sensorState != lastSensorState && (millis() - activateTime > 500))
   {
    Serial.println("--------- Activate ------------");
    digitalWrite(ledPin, HIGH);
    //myStepper.step(stepsPerRevolution*2);
    lastSensorState = sensorState;
    delay(2000);
   }

  
  delay(100);
  
  //val = digitalRead(InputSensor);   // lê o pino de entrada
  //digitalWrite(ledPin, val);  // aciona o LED com o valor lido do botão
}