#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "358d694a-79c2-11ec-90d6-0242ac120003"
#define CHARACTERISTIC_UUID "6f9a8c4e-79c2-11ec-90d6-0242ac120003"

const int leftTurn = 12;
const int rightTurn =  33;
const int brake = 27;
int leftState = 0;
int rightState = 0;
int brakeState = 0;

class MyCallbackHandler: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic){
    std::string value = pCharacteristic->getValue();
    if (value == "Q") {
      digitalWrite(leftTurn, HIGH);
      digitalWrite(brake, HIGH);
      Serial.println("left/brake");
    }
    else if (value == "W") {
      digitalWrite(rightTurn, HIGH);
      digitalWrite(brake, HIGH);
      Serial.println("right/brake");
    }
    else if (value == "L") {
      digitalWrite(leftTurn, HIGH);
      Serial.println("left");
    }
    else if (value == "R") {
      digitalWrite(rightTurn, HIGH);
      Serial.println("right");
    }
    else if (value == "B") {
      digitalWrite(brake, HIGH);
      Serial.println("brake");
    }
    if(value == "B" && value == "LB" && value == "RB"){
      digitalWrite(brake, LOW);
    }
    delay(200);
    digitalWrite(leftTurn, LOW);
    digitalWrite(rightTurn,LOW);
    delay(200);
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("lit_mainframe");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("get lit yo");
  pCharacteristic->setCallbacks(new MyCallbackHandler());
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  pinMode(leftTurn, OUTPUT);
  pinMode(rightTurn, OUTPUT);
  pinMode(brake, OUTPUT);
}

void loop() {
  delay(50);
}
