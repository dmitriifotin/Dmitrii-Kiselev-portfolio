#include <Arduino.h>
#include <driver/ledc.h> 

const int servoPins[] = {13, 12, 27, 33, 15, 32};  // GPIO pins for servos
const int potPin1 = A0;  // Analog pin for potentiometer 1 (controls 4 servos)
const int potPin2 = A1;  // Analog pin for potentiometer 2 (controls 2 servos)

void setup() {
  for (int i = 0; i < 6; i++) {
    ledcSetup(i, 50, 16);  // Configure LEDC PWM for servo control
    ledcAttachPin(servoPins[i], i);  // Attach the servo pin to the LEDC channel
  }
  Serial.begin(9600);
}

void loop() {
  int potValue1 = analogRead(potPin1);
  int potValue2 = analogRead(potPin2);

  // Control the first four servos with potentiometer 1
  for (int i = 0; i < 4; i++) {
    int servoAngle1 = map(potValue1, 0, 4095, 0, 180);
    int pwmValue1 = map(servoAngle1, 0, 180, 57000, 65000);
    ledcWrite(i, pwmValue1);
  }

  // Control the last two servos with potentiometer 2
  for (int i = 4; i < 6; i++) {
    int servoAngle2 = map(potValue2, 0, 4095, 0, 180);
    int pwmValue2 = map(servoAngle2, 0, 180, 57000, 65000);
    ledcWrite(i, pwmValue2);
  }
  Serial.print("Potentiometer 1: ");
  Serial.print(potValue1);
  Serial.print("\t Potentiometer 2: ");
  Serial.println(potValue2);
  
  delay(20);  // Adjust delay as needed for smoother motion
}
