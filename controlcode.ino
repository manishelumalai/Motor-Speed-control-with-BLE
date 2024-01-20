#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID  "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

int enbA=5;
int in1=7;
int in2=8;
int receivedvalue;
BLEServer*pserver=NULL;
BLECharacteristic*pcharacteristic=NULL;
bool deviceconnected=false;
bool olddeviceconnected=false;
uint32_t value = 0;


class myservercallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pserver) {
      deviceconnected = true;
    };

    void onDisconnect(BLEServer* pserver) {
      deviceconnected = false;
    };
};

class mycallbacks:public BLECharacteristicCallbacks{
  void onWrite(BLECharacteristic* pCharacteristic) {
        std::string value = pcharacteristic->getValue();
          if (value.length() > 0){
             receivedvalue = std::stoi(value);
            analogWrite(enbA,receivedvalue);
          }
          else{
            
            receivedvalue = static_cast<int>(value[0]);
            if (receivedvalue == 1) {
                digitalWrite(in1, HIGH);
                digitalWrite(in2,LOW);

            } else if (receivedvalue == 0) {
                digitalWrite(in1, LOW);
                digitalWrite(in2,LOW);
            }
          }
}};

void setup()
{Serial.begin(115200);
pinMode(enbA,OUTPUT);
pinMode(in1,OUTPUT);
pinMode(in2,OUTPUT);
  BLEDevice::init("MOTOR_CONTROL");

  pserver=BLEDevice::createServer();
  pserver->setCallbacks(new myservercallbacks());

  BLEService*pservice=pserver->createService(SERVICE_UUID);
  pcharacteristic=pservice->createCharacteristic(CHARACTERISTIC_UUID,
                                               BLECharacteristic::PROPERTY_READ|
                                               BLECharacteristic::PROPERTY_WRITE|
                                               BLECharacteristic::PROPERTY_NOTIFY|
                                               BLECharacteristic::PROPERTY_INDICATE );

 pcharacteristic->addDescriptor(new BLE2902());
 pcharacteristic->setCallbacks(new mycallbacks());

 pservice->start();
 BLEAdvertising*pAdvertising=BLEDevice::getAdvertising();
 pAdvertising->addServiceUUID(SERVICE_UUID);
 pAdvertising->setScanResponse(false);
 pAdvertising->setMinPreferred(0x0);
 BLEDevice::startAdvertising();
 Serial.println("waiting for a clinent connection to notify");
}



void loop(){

if(!deviceconnected&&olddeviceconnected){
  delay(500);
  pserver->startAdvertising();
  Serial.println("waiting for connection");
  olddeviceconnected=deviceconnected;
}

if(deviceconnected && !olddeviceconnected){
  olddeviceconnected=deviceconnected;
}

}