// use libraries for the scale and LCD
#include "HX711.h"
#include <LiquidCrystal.h>

//the linear calibration factor that converts the measured voltage drop into weight
#define calibration_factor 49000

// setting the pins for the load cell
#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2

// setting the pins for the LCD
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

// setting the pins for the tare button, the kg/lb switch and the LED and piezo for out of range singal
const int buttonPin = 7;
const int switchPin =  8;
const int outOfRangePin = 1;

// vars for weight, tare button state, kg/lb switch state and out of range state
float weight = 0;
int buttonState = 0;
int switchState = 0;
int outOfRange = 0;

// decalre the scale
HX711 scale;

void setup() {
  // initiate scale with the determined calibration factor and set the initial weight to 0
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();

  // initiate LCD
  lcd.begin(16, 2);

  // declare inputs and outputs for buttons and switches
  pinMode(buttonPin, INPUT);
  pinMode(switchPin, INPUT);
  pinMode(outOfRangePin, OUTPUT);
}

void loop() {
  weight = scale.get_units(); //get the weight from the scale
  buttonState = digitalRead(buttonPin); //read if tare is pressed
  switchState = digitalRead(switchPin); //read if switch is for kg or lb
  
  // round down values within +/-0.01
  if (weight > -0.01 && weight < 0.01) {
    weight = 0.000;
  }

  // check if the weight is out of range of 5 kgs, set out of range IO to high so the LED lights up and piezo gives a signal
  if (weight * 0.45359237 > 5 || weight * 0.45359237 < -5) {
    lcd.setCursor(0,0);
    lcd.print("Out of range");
    lcd.setCursor(0,1);
    lcd.print("Max 5 kgs/10 lbs");
    digitalWrite(outOfRangePin, HIGH);
    delay(250);
    digitalWrite(outOfRangePin, LOW);
    delay(250);
    lcd.clear();
  }

  // if not out of range and the switch is set to lbs, print the weight in lbs on LCD
  else if (switchState == HIGH){
    lcd.setCursor(0,0);
    lcd.print(weight);
    lcd.setCursor(0,1);
    lcd.print("lbs");
  }

  // if not out of range and the switch is set to kgs, print the weight in kgs on LCD
  else {
    lcd.setCursor(0,0);
    lcd.print(weight * 0.45359237);
    lcd.setCursor(0,1);
    lcd.print("kgs");
  }

  // if tare is pressed, zero out the current weight on the scale
  if (buttonState == HIGH) {
    scale.tare();
  }
  delay(500);
  
}
