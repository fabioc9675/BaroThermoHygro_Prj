/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TXID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RXID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_NOID "6E400004-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *pCharacteristic_tx;
BLECharacteristic *pCharacteristic_rx;
BLECharacteristic *pCharacteristic_no;

int cont = 0;
String data_no;
String data_tx;
char buf_tx[500];
char buf_no[500];

String prueba = "[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1;-1,-1,0,4,2,-1,-1,-1,-1,-1,-1,-1;-1,-1,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1;-1,-1,-2,-2,-2,3,1,-1,-1,-1,-1,-1;-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1;-1,-1,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1;-1,-1,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1;-1,-1,0,0,-1,-1,-1,-1,-1,-1,-1,-1;-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1;0,-1,-2,-2,-2,-1,-1,-1,-1,-1,-1,1;0,-1,-2,-2,-2,0,28,-1,-1,11,39,14;-1,-1,89,97,46,68,33,5,3,10,-1,8;23,13,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1;-1,-1,-2,-3,-2,-1,-1,-1,-1,-1,-1,-1;7,-1,-2,-2,-2,-1,-2,-1,-1,-1,-1,-1;-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1]";

int dato [192] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,4,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-2,-2,-2,3,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-2,-2,-2,-1,-1,-1,-1,-1,-1,1,0,-1,-2,-2,-2,0,28,-1,-1,11,39,14,-1,-1,89,97,46,68,33,5,3,10,-1,8,23,13,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-2,-3,-2,-1,-1,-1,-1,-1,-1,-1,7,-1,-2,-2,-2,-1,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0)
    {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++)
        Serial.print(rxValue[i]);

      Serial.println();
      Serial.println("*********");
    }
  }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("TAPP_Fab");
  BLEDevice::setMTU(50);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic_tx = pService->createCharacteristic(
      CHARACTERISTIC_TXID,
      BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_WRITE_NR |
          BLECharacteristic::PROPERTY_INDICATE);

  pCharacteristic_rx = pService->createCharacteristic(
      CHARACTERISTIC_RXID,
      BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_WRITE_NR |
          BLECharacteristic::PROPERTY_INDICATE);

  pCharacteristic_no = pService->createCharacteristic(
      CHARACTERISTIC_NOID,
      BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_WRITE_NR |
          BLECharacteristic::PROPERTY_INDICATE);

  pCharacteristic_tx->setCallbacks(new MyCallbacks());
  pCharacteristic_rx->setValue("Hello World says Neil");

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic_tx->addDescriptor(new BLE2902());
  pCharacteristic_rx->addDescriptor(new BLE2902());
  pCharacteristic_no->addDescriptor(new BLE2902());

  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(10000);

  cont = cont + 1;
  // data_no = prueba + String(cont);
  data_tx = "Hello World says Neil: " + String(cont);
  
  // data_no.toCharArray(buf_no, data_no.length() + 1);
  data_tx.toCharArray(buf_tx, data_tx.length() + 1);

  pCharacteristic_rx->setValue(buf_tx);

  data_no = "Ini";
  data_no.toCharArray(buf_no, data_no.length() + 1);
  pCharacteristic_no->setValue(buf_no);
  pCharacteristic_no->notify();
  Serial.println("fin");

  for (int i = 0; i < 48; i++){
    data_no = String(dato[4*i]) +',' + String(dato[4*i+1])+',' + String(dato[4*i+2])+',' + String(dato[4*i+3]);
    if (i < 47){
      data_no = data_no + ',';
    }
    Serial.print(data_no);
    data_no.toCharArray(buf_no, data_no.length() + 1);
    pCharacteristic_no->setValue(buf_no);
    pCharacteristic_no->notify();
    delay(10);
  }
  data_no = "Fin";
  data_no.toCharArray(buf_no, data_no.length() + 1);
  pCharacteristic_no->setValue(buf_no);
  pCharacteristic_no->notify();
  Serial.println("fin");
}
