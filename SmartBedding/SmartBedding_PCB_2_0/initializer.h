/* *****************************************************************************
 * File:     initializer.h                                                     *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on March 29, 2021, 12:31 PM                                         *
 * *****************************************************************************/

#ifndef INITIALIZER_H
#define INITIALIZER_H

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "Arduino.h"
#include <Esp.h>
#include <stdio.h>

#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include "ws2812b.h"
#include "recordSound.h"
#include "sdFunctions.h"

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TXID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RXID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_NOID "6E400004-B5A3-F393-E0A9-E50E24DCCA9E"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/
#define HSPI_SCLK_P 14 // sclk pin
#define HSPI_MISO_P 12 // miso pin
#define HSPI_MOSI_P 13 // mosi pin
#define HSPI_SS_P 15   // ss pin

#define HSPI_SPEED 10000000 // 10 MHz

#define I2C_SDA 21 // sda pin
#define I2C_SCL 22 // scl pin

#define I2C_SPEED 400000 // 400 kHz

#define VSPI_SCLK_P 18 // sclk pin
#define VSPI_MISO_P 19 // miso pin
#define VSPI_MOSI_P 23 // mosi pin
#define VSPI_SS_P 5    // ss pin

#define VSPI_SPEED 4000000 // 4 MHz

// Pin defines
#define SR_CLOCK_PIN 27 // Clock shift register pin
#define SR_DATAW_PIN 32 // Data shift register pin
#define SR_LATCH_PIN 4  // Latch-clock shift register

// #define BUZZER_PIN 4 // Buzzer pin

// Pin to control the Light indicator
#define LED_COUNT 1 // quantity of LEDs in the line
#define LED_PIN 17  // pin connection

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
char mainFileName[] = "/Registros.txt";

/** ****************************************************************************
 ** ************ PROTOTYPES ****************************************************
 ** ****************************************************************************/
// initPinPeripheral()
// @param none: void
// @return none: void
void initPinPeripheral(void);

// initSDPeripheral()
// @param none: void
// @return none: void
void initSDPeripheral(void);

// initCharString()
// @param none: void
// @return none: void
void initCharString(void);

// initLedIndicator()
// @param none: void
// @return none: void
void initLedIndicator(void);

// initMemory()
// @param none: void
// @return none: void
void initMemory(void);

// indicatorSetColor()
// @param color: uint32_t
// @return none: void
void indicatorSetColor(uint32_t color);

/* *****************************************************************************
 * **** TASK TO RECORD DATA SOUND **********************************************
 * *****************************************************************************/

#endif /* INITIALIZER_H */

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/
