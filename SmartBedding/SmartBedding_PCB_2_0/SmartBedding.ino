/* *****************************************************************************
 * File:     main.ino                                                          *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on March 29, 2021, 12:31 PM                                         *
 * *****************************************************************************/

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "Arduino.h"
#include <Esp.h>
#include <nvs_flash.h>
#include <stdio.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "initializer.h"
#include "timer.h"
#include "matrixSample.h"
#include "movementSample.h"
#include "storeData.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

BLECharacteristic *pCharacteristic_tx;
BLECharacteristic *pCharacteristic_rx;
BLECharacteristic *pCharacteristic_no;

String data_no;
String data_tx;
char buf_tx[500];
char buf_no[500];
int datomat[192];

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/

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

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// core definitions for program execution
uint8_t taskCoreZero = 0;
uint8_t taskCoreOne = 1;

// Tasks handlers
xTaskHandle tInitializerHandle;
xTaskHandle tSoundHandle;
xTaskHandle tTimerHandle;
xTaskHandle tPressAdqHandle;
xTaskHandle tMovAdqHandle;
xTaskHandle tStoreHandle;

// Semaphores definition
SemaphoreHandle_t xSemaphoreTimer;
SemaphoreHandle_t xSemaphorePress;
SemaphoreHandle_t xSemaphoreMovem;

// Objects to control SD Card
SPIClass hspi = SPIClass(HSPI); // SD card SPI controller
SPIClass vspi = SPIClass(VSPI); // IMU SPI controller

// Object to control pin expander chip
TwoWire i2c_gp = TwoWire(0); // I2C to control general purpose peripheral

// variables to control the register in proccess
uint16_t regNumber = 0;
uint16_t soundNumber = 0;

String sdDir;           // Variable String to save root dir
char rootDir[50];       // Variable char to save root dir
char pressFileName[50]; // Variable char to save press file name
char moveFileName[50];  // variable char to save move file name

// Files variables to store the acquired data
File pressFile;
File movemFile;
volatile uint8_t fl_movFileOpen = false;
volatile uint8_t fl_preFileOpen = false;

// flags to execute program
bool flSystemReady = false;
bool flMatrixReady = false;   // flag to indicate that matrix measurement was performed
bool flMovementReady = false; // flag to indicate that movement measurement was performed

// flags to manage interruption
volatile uint8_t flagTimerInterrupt = false;
volatile uint8_t flNewMatrixSample = false;
volatile uint8_t flNewMovementSample = false;

// variables to make a delay to finis some delayed task
volatile uint8_t fl_newSoundMov = false;
volatile uint8_t fl_newSoundPre = false;

// counter to sample data without memory
volatile uint16_t countNewMatrixSample = 0;
// counter to sample data without memory
volatile uint16_t countNewMovementSample = 0;

// variable to save the pressure data into a matrix
int16_t matrix[ROW_SIZE][COL_SIZE];
String dataMatrix;

// variable to save the movement data into a string
String dataMovement;

// Object initialization for LED control
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RBG + NEO_KHZ800);

// Counter to know the data cuantity
int counterPress = 0;
int counterMove = 0;

/** ****************************************************************************
 ** ************ PROTOTYPES ****************************************************
 ** ****************************************************************************/

/* ***************************************************************************
 * **** CONFIGURATION ********************************************************
 * ***************************************************************************/
void setup()
{

    // flash initialization
    nvs_flash_init();

    // binary semaphore creation
    xSemaphoreTimer = xSemaphoreCreateBinary();
    xSemaphoreMovem = xSemaphoreCreateBinary();
    xSemaphorePress = xSemaphoreCreateBinary();

    // Serial initialization
    Serial.begin(115200);

    // Bluetooth configuration
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

    Serial.println("Welcome to SmartBedding Log");
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // peripheral initialization
    initTimer();
    initPinPeripheral();
    initLedIndicator();
    initSDPeripheral();

    // Tasks initialization
    xTaskCreatePinnedToCore(task_TimerHandler, "task_Timer_Handler", 2048, NULL, 4, &tTimerHandle, taskCoreOne);

    // if (flSystemReady)
    // {
    xTaskCreatePinnedToCore(task_RecordSound, "task_Record_Sound", 16384, NULL, 4, &tSoundHandle, taskCoreOne);
    xTaskCreatePinnedToCore(task_PressAdq, "task_Press_Adq", 2048, NULL, 3, &tPressAdqHandle, taskCoreOne);
    xTaskCreatePinnedToCore(task_MovAdq, "task_Mov_Adq", 2048, NULL, 3, &tMovAdqHandle, taskCoreOne);
    xTaskCreatePinnedToCore(task_storeData, "task_Store_Data", 4096, NULL, 3, &tStoreHandle, taskCoreOne);
    // }
    // else
    // {
    //     Serial.println("Restarting program because it is not ready");
    //     Serial.flush();
    //     indicatorSetColor(RED);
    //     vTaskDelay(20000 / portTICK_PERIOD_MS);
    //     ESP.restart(); // function to restart SmartBedding in case there is not memory connection
    // }

    // clean Serial Buffers
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

/* ***************************************************************************
 * **** LOOP MAIN_MENU *******************************************************
 * ***************************************************************************/
void loop()
{
}

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/