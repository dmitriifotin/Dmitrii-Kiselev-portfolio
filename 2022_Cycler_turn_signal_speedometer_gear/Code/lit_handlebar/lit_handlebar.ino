//math library
#include "math.h"
//******************************************************************************
//BT library
#include "BLEDevice.h"
//******************************************************************************
//screen libraries
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14
//******************************************************************************
//accelerometer libraries
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
Adafruit_MPU6050 mpu;
#include <Preferences.h>
Preferences preferences;
//******************************************************************************
//variables
float accel_x0 = 0;
float accel_y0 = 0;
float accel_z0 = 0;
float accel_x = 0;
float accel_y = 0;
float accel_z = 0;
float xTheta0 = 0;
float yTheta0 = 0;
float zTheta0 = 0;
float xTheta = 0;
float yTheta = 0;
float zTheta = 0;
float xThetaIC = 0;
float yThetaIC = 0;
float zThetaIC = 0;
float xRadS0 = 0;
float yRadS0 = 0;
float zRadS0 = 0;
float xRadS = 0;
float yRadS = 0;
float zRadS = 0;
float velocity_x = 0;
float velocity_y = 0;
float velocity_x0 = 0;
float velocity_y0 = 0;
float speedometer = 0;
int initial_condition = 1;
int speed_units = 0;

//******************************************************************************
//battery analog
const int batteryPin = 26;
int batteryValue = 0;
int batteryCharge = 4;
//buttons/LEDs
const int leftTurn = 21;
const int rightTurn =  13;
const int brake = 12;
const int leftLED =  14;
const int rightLED =  32;
int leftState = 0;
int rightState = 0;
int brakeState = 0;
//******************************************************************************
//server IDs
static BLEUUID serviceUUID("358d694a-79c2-11ec-90d6-0242ac120003");
static BLEUUID charUUID("6f9a8c4e-79c2-11ec-90d6-0242ac120003");

static BLEAddress *pServerAddress;
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
//******************************************************************************
//accelerometer default functions

//******************************************************************************
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
}
//******************************************************************************
//BT connect to device
bool connectToServer(BLEAddress pAddress) {
    Serial.print("Forming a connection to ");
    Serial.println(pAddress.toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->connect(pAddress);
    Serial.println(" - Connected to server");

    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our service");

    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our characteristic");

    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
}
//******************************************************************************
//BT get ID'ed device address
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {
      Serial.print("Found our device!  address: "); 
      advertisedDevice.getScan()->stop();

      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;
    }
  }
};

//******************************************************************************
void setup() {
  Serial.begin(115200);
  //pins setup
  pinMode(leftTurn, INPUT);
  pinMode(rightTurn, INPUT);
  pinMode(brake, INPUT);
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
//******************************************************************************
  //display setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5,10);
  display.println("get lit yo");
  yield();
  display.display();
//******************************************************************************
  //accelerometer setup

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
//******************************************************************************
  //BT setup
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);

}

void loop() {
  String value;
  leftState = digitalRead(leftTurn);
  rightState = digitalRead(rightTurn);
  brakeState = digitalRead(brake);
  
  //Adjust this! - what happens if BT connection fails
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      connected = true;
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }
//******************************************************************************
  //read and process battery value
  batteryValue = analogRead(batteryPin);
  if (batteryValue > 1800) {
    batteryCharge = 4;
  }
  else if (batteryValue > 1700) {
    batteryCharge = 3;
  }
  else if (batteryValue > 1600) {
    batteryCharge = 2;
  }
  else {
    batteryCharge = 1;
  }
  
//******************************************************************************
  //Turn/brake over BT code
  if (connected) {
    if(leftState==HIGH && brakeState==HIGH) {
      value = "Q";
      Serial.println("left/brake");
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());
      digitalWrite(leftLED, HIGH);
    }
    else if(rightState==HIGH && brakeState==HIGH) {
      value = "W";
      Serial.println("right/brake");
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());
      digitalWrite(rightLED, HIGH);
    }
    else if(leftState==HIGH) {
      value = "L";
      Serial.println("left");
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());
      digitalWrite(leftLED, HIGH);
    }
    else if(rightState==HIGH) {
      value = "R";
      Serial.println("right");
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());
      digitalWrite(rightLED, HIGH);
    }
    else if(brakeState==HIGH) {
      value = "B";
      Serial.println("brake");
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());
    }
    else {
      value = "get lit yo!";
      Serial.println("no input");
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());
    }
  }
  else {
//    display.println("Vest not");
//    display.println("connected");
//    yield();
//    display.display();
  }
//******************************************************************************
  //Accelerometer code
  //Todo:
  //round up values
  preferences.begin("mileage", false);
  float mileage = preferences.getFloat("mileage", 0);
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  accel_x0 = a.acceleration.x;
  accel_y0 = a.acceleration.y;
  accel_z0 = a.acceleration.z;
  if (initial_condition == 1) {
    xRadS0 = g.gyro.x;
    yRadS0 = g.gyro.y;
    zRadS0 = g.gyro.z;
    xThetaIC = acos(accel_x0/9.81);
    yThetaIC = acos(accel_y0/9.81);
    zThetaIC = acos(accel_z0/9.81);
    initial_condition = 0;
  }
  //display code
  display.setCursor(100,0);
  display.setTextSize(1);
  display.print(batteryCharge * 25);
  display.print("%");
  display.fillRect(82, 2, 1, 2, WHITE);
  display.drawRoundRect(83, 0, 16, 6, 1, WHITE);
  if(batteryCharge == 4){
    display.fillRect(84, 0, 3, 6, WHITE);
    display.fillRect(88, 0, 3, 6, WHITE);
    display.fillRect(92, 0, 3, 6, WHITE);
    display.fillRect(96, 0, 3, 6, WHITE);
  }
  else if(batteryCharge == 3){
    display.fillRect(88, 0, 3, 6, WHITE);
    display.fillRect(92, 0, 3, 6, WHITE);
    display.fillRect(96, 0, 3, 6, WHITE);
  }
  else if(batteryCharge == 2){
    display.fillRect(92, 0, 3, 6, WHITE);
    display.fillRect(96, 0, 3, 6, WHITE);
  }
  else if(batteryCharge == 1){
    display.fillRect(96, 0, 3, 6, INVERSE);
  }
  display.setCursor(0,7);
  display.setTextSize(2);
  display.print((speedometer * 3600 / 1000)-fmod(speedometer * 3600 / 1000,1.0));
  display.println(" km/h");
  display.setTextSize(1);
  display.print("ODO: ");
  display.print(mileage);
  display.print(" km");

  yield();
  display.display();

  delay(200);
  mpu.getEvent(&a, &g, &temp);
  xRadS = g.gyro.x;
  yRadS = g.gyro.y;
  zRadS = g.gyro.z;

  xTheta = (xRadS - xRadS0) * 0.2 * 1.8;
  yTheta = (yRadS - yRadS0) * 0.2 * 1.8;
  zTheta = (zRadS - zRadS0) * 0.2 * 1.8;


  xTheta0 = xTheta0 + xTheta;
  yTheta0 = yTheta0 + yTheta;
  zTheta0 = zTheta0 + zTheta;

  accel_x = a.acceleration.x - (9.81 * cos(xThetaIC + xTheta0));
  accel_y = a.acceleration.y - (9.81 * cos(yThetaIC + yTheta0));
  accel_z = a.acceleration.z - (9.81 * cos(zThetaIC + zTheta0));
  accel_x0 = accel_x0 - (9.81 * cos(xThetaIC + xTheta0));
  accel_y0 = accel_y0 - (9.81 * cos(yThetaIC + yTheta0));
  accel_z0 = accel_z0 - (9.81 * cos(zThetaIC + zTheta0));
  velocity_x = (accel_x - accel_x0) * 0.2 + velocity_x0;
  velocity_y = (accel_y - accel_y0) * 0.2 + velocity_y0;
  speedometer = sqrt(sq(velocity_x)+sq(velocity_y));
  velocity_x0 = velocity_x;
  velocity_y0 = velocity_y;
  mileage = mileage + speedometer * 0.2 / 3600;
  preferences.putFloat("mileage", mileage);
  preferences.end();

//******************************************************************************
  //test accelerometer outputs/calculations
  Serial.print("Default Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");
  Serial.print("Acceleration X: ");
  Serial.print(accel_x);
  Serial.print(", Y: ");
  Serial.print(accel_y);
  Serial.print(", Z: ");
  Serial.print(accel_z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.println("Derived theta: ");
  Serial.println(xThetaIC);
  Serial.println(yThetaIC);
  Serial.println(zThetaIC);
  Serial.println("Theta: ");
  Serial.println(xTheta0);
  Serial.println(yTheta0);
  Serial.println(zTheta0);
  Serial.print("Velocity X: ");
  Serial.print(velocity_x);
  Serial.print(", Y: ");
  Serial.print(velocity_y);
  Serial.print(", Speed: ");
  Serial.println(speedometer);
  Serial.print("Mileage: ");
  Serial.println(mileage);
  Serial.print("Battery: ");
  Serial.println(batteryValue);
//******************************************************************************
//clear display
  display.clearDisplay();
  display.setCursor(0,0);
//turn off LEDs
  digitalWrite(leftLED, LOW);
  digitalWrite(rightLED, LOW);
}
