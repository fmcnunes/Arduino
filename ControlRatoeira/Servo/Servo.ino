#define SERVO_PIN 9

void setup() {
  pinMode(SERVO_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // Move servo to 0 degrees
   Serial.println("--------- 0 ------------");
  setServoAngle(0);
  delay(3000); // Hold for 3 seconds

  // Move servo to 90 degrees
   Serial.println("--------- 90 ------------");
  setServoAngle(90);
  delay(3000); // Hold for 3 seconds

  // Move servo to 180 degrees
   Serial.println("--------- 180 ------------");
  setServoAngle(180);
  delay(3000); // Hold for 3 seconds
}

void setServoAngle(int angle) {
  // Adjust the pulse width range if necessary
  int pulseWidth = map(angle, 0, 180, 540, 2400); // Try narrowing the range

  for (int i = 0; i < 200; i++) { // Send the pulse 50 times
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(SERVO_PIN, LOW);
    delayMicroseconds(20000 - pulseWidth); // Complete the 20ms period (50Hz)
  }
}
