/* *****************************************************************************
 * File:     recordSound.ino                                                   *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on March 29, 2021, 12:31 PM                                         *
 * *****************************************************************************/

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "initializer.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// core definitions for program execution
extern uint8_t taskCoreZero;
extern uint8_t taskCoreOne;

// Tasks handlers
extern xTaskHandle tInitializerHandle;
extern xTaskHandle tSoundHandle;

// Objects to control SD Card
extern SPIClass hspi; // SD card SPI controller
extern SPIClass vspi; // IMU SPI controller

// Object to control pin expander chip
extern TwoWire i2c_gp; // I2C to control mcp peripheral

// variables to control the register in proccess
extern uint16_t regNumber;

extern String sdDir;       // Variable String to save root dir
extern char rootDir[50];   // Variable char to save root dir
extern char pressFileName[50]; // Variable char to save press file name
extern char moveFileName[50];  // variable char to save move file name

// flags to execute program
extern bool flSystemReady;

// Object initialization for LED control
extern WS2812FX ws2812fx;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/* ***************************************************************************
 * **** FUNCTIONS ************************************************************
 * ***************************************************************************/

// initPinPeripheral()
// @param none: void
// @return none: void
void initPinPeripheral(void)
{
    // Initialization of Shif Register Pin as output
    pinMode(SR_CLOCK_PIN, OUTPUT);
    pinMode(SR_DATAW_PIN, OUTPUT);
    pinMode(SR_LATCH_PIN, OUTPUT);

    // initialization of buzzer pin as output
//    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    // HSPI pin initialization
    pinMode(HSPI_MISO_P, INPUT_PULLUP);
    pinMode(HSPI_SCLK_P, OUTPUT);
    pinMode(HSPI_MOSI_P, OUTPUT);
    pinMode(HSPI_SS_P, OUTPUT);

    // VSPI pin initialization
    pinMode(VSPI_MISO_P, INPUT_PULLUP);
    pinMode(VSPI_SCLK_P, OUTPUT);
    pinMode(VSPI_MOSI_P, OUTPUT);
    pinMode(VSPI_SS_P, OUTPUT);

    // SPI to control SD Card initialization
    hspi.begin(HSPI_SCLK_P, HSPI_MISO_P, HSPI_MOSI_P, HSPI_SS_P);

    // SPI to control movement initialization
    vspi.begin(VSPI_SCLK_P, VSPI_MISO_P, VSPI_MOSI_P, VSPI_SS_P);

    // I2C initialization
    i2c_gp.begin(I2C_SDA, I2C_SCL, I2C_SPEED);
}

// initSDPeripheral()
// @param none: void
// @return none: void
void initSDPeripheral(void)
{
    uint8_t cardType = CARD_NONE;

    // SD comunication init
    if (!SD.begin(HSPI_SS_P, hspi, HSPI_SPEED))
    {
        // Error notification about non memory connection
        // Serial.println("Card Mount Failed");
        return;
    }

    cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        // Error notification about non memory connection
        //Serial.println("No SD card attached");
        return;
    }

    // memory initialization
    initMemory();
}

// initCharString()
// @param none: void
// @return none: void
void initCharString(void)
{
    regNumber = 0; // initialization od register number
    sdDir.reserve(50);
    sdDir = String("");
}

// initLedIndicator()
// @param none: void
// @return none: void
void initLedIndicator(void)
{
    // initialize Led Indicator
    ws2812fx.init();
    ws2812fx.setBrightness(50);
    ws2812fx.setSpeed(100);

    // set initialization color
    indicatorSetColor(GREEN);
}

// initMemory()
// @param none: void
// @return none: void
void initMemory(void)
{

    // local variables definition
    esp_err_t err = ESP_OK;

    //char *lineReg = (char *)malloc(sizeof(char) * 50);
    char lineReg[50];
    String slineR;
//    String sPressF;
//    String sImuF;

    slineR.reserve(50);
    slineR = "";
//    sPressF.reserve(50);
//    sPressF = "";
//    sImuF.reserve(50);
//    sImuF = "";
    initCharString();

    // open registers file counter
    File fileRegisters = SD.open(mainFileName, FILE_READ); // try to open file
    // verify it file exist or doesnt exist
    if (!fileRegisters)
    { // if file doesnt exist
        writeFile(SD, mainFileName, "Registro de recorridos del sensor\r\n");
        regNumber = 1; // register number 1 in proccess
        fileRegisters.close();
    }
    else
    { // if file exists
        while (fileRegisters.available())
        {
            char dReg = fileRegisters.read();
            if (dReg == '\n')
            {
                regNumber++;
            }
        }
        fileRegisters.close();
    }

    // Semaphore freedoom

    // creation of line to control the register backup
    slineR = "Register_ " + String(regNumber) + '\n';
    slineR.toCharArray(lineReg, slineR.length() + 1);
    if (appendFile(SD, mainFileName, lineReg) != ESP_OK)
    {
        Serial.println("Update of file Registros failed");
        err = ESP_FAIL;
    }

    // creation of folder container
    sdDir = "/Register_" + String(regNumber);
    sdDir.toCharArray(rootDir, sdDir.length() + 1);
    if (createDir(SD, rootDir) != ESP_OK)
    {
        Serial.println("Dir creation Failed");
        err = ESP_FAIL;
    }

    // creation of files to save data of IMU and Pressure
//    sPressF = sdDir + "/RegPress_" + String(regNumber) + ".txt";
//    sPressF.toCharArray(pressFile, sPressF.length() + 1);
//    sImuF = sdDir + "/RegMove_" + String(regNumber) + ".txt";
//    sImuF.toCharArray(moveFile, sImuF.length() + 1);
//    if (writeFile(SD, pressFile, "Pressure Data Register\r\n") != ESP_OK)
//    {
//        Serial.println("Pressure file creation Failed");
//        err = ESP_FAIL;
//    }
//    if (writeFile(SD, moveFile, "Movement Data Register\r\n") != ESP_OK)
//    {
//        Serial.println("Movement file creation Failed");
//        err = ESP_FAIL;
//    }

    // creation of files already done
    if (err == ESP_OK)
    {
        Serial.println("System initialized successfully");
        flSystemReady = true;
    }

    // Led indicator ready
    indicatorSetColor(BLUE);
}

// indicatorSetColor()
// @param color: uint32_t
// @return none: void
void indicatorSetColor(uint32_t color)
{
    ws2812fx.start();
    ws2812fx.setMode(1);
    ws2812fx.setColor(color);
    ws2812fx.service();
}

/* *****************************************************************************
 * **** TASK TO RECORD INITILIZE EVERYTHING ************************************
 * *****************************************************************************/

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/
