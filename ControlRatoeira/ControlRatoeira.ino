const int PinRele = 2; //Sentido de rotação

const int InputSensor = 7;

const int OutputLed = 3;

const int ContactSwitch = 4;

const int ledPin = LED_BUILTIN;

int sensorValue = 0;
int lastSensorValue = 0;

bool sensorState = LOW;
bool lastSensorState = LOW;

double activateTime = millis();

int contactSwitch = 0; 

bool lastValue  = 0;

void setup() {

  // initialize the serial port:
  Serial.begin(115200);

  pinMode(ContactSwitch, INPUT_PULLUP);


  pinMode(OutputLed, OUTPUT);
  digitalWrite(OutputLed, LOW);

  pinMode(PinRele, OUTPUT);
  digitalWrite(PinRele, LOW);

  pinMode(InputSensor, INPUT);

  Serial.println("--------- Setup done ------------");

  delay(2000);

  Serial.println("--------- Wait done ------------");
  
  Wait_lock();

  Serial.println("--------- Lock done ------------");
}

void loop() {

  Serial.println("--------- Loop ------------");

  contactSwitch = digitalRead(ContactSwitch);
  
  sensorValue = analogRead(InputSensor); 
  if (lastSensorValue == 0) { lastSensorValue = sensorValue;}

  int delta =   sensorValue - lastSensorValue;
  if (delta < 0) { delta = - delta; }
  
  Serial.print(sensorValue);
  Serial.print(" - ");
  Serial.print(contactSwitch);
  Serial.print(" - ");
  Serial.print(delta);


 lastSensorValue = sensorValue;
  if (delta > 5)
     sensorState = HIGH;
  else
   {
     sensorState = LOW;
     activateTime = millis();
     lastSensorState = sensorState;
     digitalWrite(ledPin, sensorState);
   }




  //if (sensorState && sensorState != lastSensorState && (millis() - activateTime > 500))
  if (delta > 5)
   {
    Serial.println("--------- Activate ------------");
    lastSensorState = sensorState;

    digitalWrite(PinRele, HIGH);
    delay(3500);
    Serial.println("--------- Activate done ------------");
    digitalWrite(PinRele, LOW);

    while(true)
       delay(5000);

    Serial.println("--------- Lock Done ------------");
   }

  
  //delay(100);
}

void Wait_lock()
{
    digitalWrite(PinRele, HIGH);

    contactSwitch = readContact();
    while(contactSwitch != 0)
    {
      contactSwitch = readContact();
      //Serial.print("wait for 0, ");
      //Serial.println(contactSwitch);
    }
    
    contactSwitch = readContact();
    while(contactSwitch != 1)
    {
      contactSwitch = readContact();
      //Serial.print("wait for 1, ");
      //Serial.println(contactSwitch);
    }

    digitalWrite(PinRele, LOW);
    Serial.println("Done");
}


int readContact()
{
  int leituras[4];
  int cnt=0;
  int sum;
  int idx=0;

 // return digitalRead(ContactSwitch);

 while(true)
  {
   leituras[idx] = digitalRead(ContactSwitch);
   idx++;
   if (idx > 3) idx = 0;
   
   delay(5);
   cnt ++;

   if (cnt > 4)
    {
      sum=leituras[0] + leituras[1] +leituras[2] + leituras[3];

      if (sum == 0) return 0;
      if (sum == 4) return 1;
    }
  }
}