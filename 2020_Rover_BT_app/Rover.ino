#include "BluetoothSerial.h"
#include <ESP32_Servo.h>
#include "esp32-hal-ledc.h"
#include "Arduino.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int controlPin1 = 27;
int controlPin2 = 26;
int enablePin = 14;
int motorSpeed = 1500;

const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;

Servo Base;
Servo Shoulder;
Servo Elbow;
Servo Lens;
Servo Steer;
Servo Claw;
String message = "";
char incomingChar;

void setup() {
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(enablePin, pwmChannel);
  Serial.begin(115200);
  SerialBT.begin("Please"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  pinMode(controlPin1, OUTPUT);
  pinMode(controlPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  Base.attach(13, 500, 2400);
  Shoulder.attach(25, 500, 2400);
  Elbow.attach(33, 500, 2400);
  Lens.attach(32, 500, 2400);
  Steer.attach(21, 500, 1500);
  Claw.attach(23, 500, 2400);
  
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n') {
      message += String(incomingChar);
    }
    else {
      //arm navigation
      if (message.startsWith("F")) {
        message.remove(0, 1);
        message.trim();
        Base.write(message.toInt());
      }
      if (message.startsWith("H")) {
        message.remove(0, 1);
        message.trim();
        Shoulder.write(message.toInt());
      }
      if (message.startsWith("E")) {
        message.remove(0, 1);
        message.trim();
        Elbow.write(message.toInt());
      }
      //motor speed
      if (message.startsWith("M")) {
        message.remove(0, 1);
        message.trim();
        motorSpeed = message.toInt()*70;
      }
      //claw position
      if (message.startsWith("V")) {
        message.remove(0, 1);
        message.trim();
        if (message.toInt()<90) {
          Claw.write(0);
        }
        if (message.toInt()>90) {
          Claw.write(180);
        }
      }
      message = "";
    }

    //Serial.setTimeout(50);
    Serial.write(incomingChar);
    
    //car navigation*******************************************************
    if (message == "Go") {
      digitalWrite(controlPin1, HIGH);
      digitalWrite(controlPin2, LOW);
      ledcWrite(pwmChannel, motorSpeed);
    }
    if (message == "Back") {
      digitalWrite(controlPin1, LOW);
      digitalWrite(controlPin2, HIGH);
      ledcWrite(pwmChannel, motorSpeed);
    }
    if (message == "Stop") {
      digitalWrite(enablePin, 0);
      digitalWrite(controlPin1, LOW);
      digitalWrite(controlPin2, LOW);
    }

    if (message == "Left") {
      Steer.write(250);
    }
    if (message == "Right") {
      Steer.write(0);
    }
    if (message == "Center") {
      Steer.write(95);
    }

    //lens navigation****************************************************
    if (message == "LensX0") {
      Lens.write(0);
    }
    if (message == "LensX10") {
      Lens.write(45);
    }
    if (message == "LensX100") {
      Lens.write(90);
    }
  }
  delay(20);
}