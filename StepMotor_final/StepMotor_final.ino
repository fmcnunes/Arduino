// L9110 pins
const int A1A = 9;
const int A1B = 10;
const int B1A = 6;
const int B1B = 5;

// Full-step sequence
int steps[4][4] = {
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 1},
  {1, 0, 0, 1}
};

void setup() {
   Serial.begin(115200);
  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);
  pinMode(B1A, OUTPUT);
  pinMode(B1B, OUTPUT);
}

void loop() {

int speed = 10;

  while(1)
  {
     Serial.println("Forward()!");
  int totalSteps = 200; // 1 full rotation
  for (int stepCount = 0; stepCount < totalSteps; stepCount++) {
    int i = stepCount % 4; // loop through the 4-step sequence
    digitalWrite(A1A, steps[i][0]);
    digitalWrite(A1B, steps[i][1]);
    digitalWrite(B1A, steps[i][2]);
    digitalWrite(B1B, steps[i][3]);
    delay(speed); // adjust speed
  }

  //releaseMotor();
  //delay(3000);
  //Serial.println("Back()!");

  for (int stepCount = 0; stepCount < totalSteps; stepCount++) {
    int i = stepCount % 4; // loop through the 4-step sequence
    digitalWrite(A1A, steps[i][3]);
    digitalWrite(A1B, steps[i][2]);
    digitalWrite(B1A, steps[i][1]);
    digitalWrite(B1B, steps[i][0]);
    delay(speed); // adjust speed
  }
   //releaseMotor();
   //delay(3000);
  }

}

void releaseMotor()
{
  Serial.println("ReleaseMotor()!");
  digitalWrite(A1A, LOW);
  digitalWrite(A1B, LOW);
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, LOW);
}
