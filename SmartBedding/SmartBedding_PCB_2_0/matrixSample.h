/* *****************************************************************************
 * File:     matrixSample.h                                                    *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on April 4, 2021, 20:45 PM                                          *
 * *****************************************************************************/

#ifndef MATRIXSAMPLE_H
#define MATRIXSAMPLE_H

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "Arduino.h"
#include <Esp.h>

#include "timer.h"
#include "initializer.h"
#include "mcp23017.h"
#include "ads1015.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/
#define MCP_ADDR 0x21                // pin expander address
#define ADS_ADDR ADS1015_ADDRESS_GND // ADC address = 0x48

// Matrix size
#define ROW_SIZE 16
#define COL_SIZE 12

#define MATRIX_SIZE 1200

// Row map to control the activation of rows
#define ROW01 0b001000000000000000
#define ROW02 0b000100000000000000
#define ROW03 0b000010000000000000
#define ROW04 0b000001000000000000
#define ROW05 0b000000100000000000
#define ROW06 0b000000010000000000
#define ROW07 0b000000001000000000
#define ROW08 0b000000000100000000
#define ROW09 0b000000000010000000
#define ROW10 0b000000000001000000
#define ROW11 0b000000000000100000
#define ROW12 0b000000000000010000
#define ROW13 0b000000000000001000
#define ROW14 0b000000000000000100
#define ROW15 0b000000000000000010
#define ROW16 0b000000000000000001

// Column map to control the activation of columns
#define COL01 0b00010000 // 0b0001XXXX   /// Map explanation
#define COL02 0b00010001 //      ||||+-----> ADDR0
#define COL03 0b00010010 //      |||+------> ADDR1
#define COL04 0b00010011 //      ||+-------> ADDR2
#define COL05 0b00010100 //      |+--------> ADDR3
#define COL06 0b00010101 //      +---------> ENABLE (Always 1)
#define COL07 0b00010110
#define COL08 0b00010111
#define COL09 0b00011000
#define COL10 0b00011001
#define COL11 0b00011010
#define COL12 0b00011011

// enum to control the states of pressure sample
enum tag_pressState
{
    nonePress = 0,
    updateRows = 1,
    sampleSensor = 2,
    composeData = 3
};

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// variable to control Rows
const uint32_t rowArray[ROW_SIZE] = {
    ROW01,
    ROW02,
    ROW03,
    ROW04,
    ROW05,
    ROW06,
    ROW07,
    ROW08,
    ROW09,
    ROW10,
    ROW11,
    ROW12,
    ROW13,
    ROW14,
    ROW15,
    ROW16
};

// variable to control Columns
const uint32_t colArray[COL_SIZE] = {
    COL01,
    COL02,
    COL03,
    COL04,
    COL05,
    COL06,
    COL07,
    COL08,
    COL09,
    COL10,
    COL11,
    COL12
};

/** ****************************************************************************
 ** ************ PROTOTYPES ****************************************************
 ** ****************************************************************************/
// matrixSetup()
// @param none: void
// @return err: esp_err_t
esp_err_t matrixSetup(void);

// shiftRegisterOut()
// @param dataPin: uint8_t
// @param clockPin: uint8_t
// @param val: uint32_t
// @return none: void
void shiftRegisterOut(uint32_t val);

/* *****************************************************************************
 * **** TASK TO RECORD DATA PRESSURE *******************************************
 * *****************************************************************************/
void task_PressAdq(void *arg);

#endif /* MATRIXSAMPLE_H */

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/