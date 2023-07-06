/* *****************************************************************************
 * File:     i2sConfig.h                                                       *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on March 29, 2021, 12:31 PM                                         *
 * *****************************************************************************/

#ifndef I2SCONFIG_H
#define I2SCONFIG_H

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "Arduino.h"
#include <Esp.h>

#include "driver/i2s.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_system.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/
// audio sample rate
#define SAMPLE_RATE 44100

// microphone pin definition
#define PIN_I2S_BCK 26
#define PIN_I2S_LRC 25
#define PIN_I2S_DIN 33
// #define PIN_I2S_DOUT 33

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ PROTOTYPES ****************************************************
 ** ****************************************************************************/
// @param mode : I2S_MODE_RX or I2S_MODE_TX
// @param port : pointer to port object
// @param BPS : I2S_BITS_PER_SAMPLE_16BIT or I2S_BITS_PER_SAMPLE_32BIT
// @return err : esp_err_t
esp_err_t I2S_Init(i2s_port_t i2s_port);

// I2S_ReadData() for I2S_MODE_RX
// @param i2s_port: port to read
// @param data: pointer to buffer
// @param numData: buffer size
// @param bytesRead: bytes already read
// @return err: esp_err_t
esp_err_t I2S_ReadData(i2s_port_t i2s_port, char *data, size_t numData, size_t *bytesRead);

// I2S_Write() for I2S_MODE_TX
// @param data: pointer to buffer
// @param numData: buffer size
// void I2S_Write(char *data, int numData);

// This I2S specification :
//  -   LRC high is channel 2 (right).
//  -   LRC signal transitions once each word.
//  -   DATA is valid on the CLOCK rising edge.
//  -   Data bits are MSB first.
//  -   DATA bits are left-aligned with respect to LRC edge.
//  -   DATA bits are right-shifted by one with respect to LRC edges.
//        It's valid for ADMP441 (microphone) and MAX98357A (speaker).
//        It's not valid for SPH0645LM4H(microphone) and WM8960(microphon/speaker).
//
//  -   44100Hz
//  -   stereo
// More information about configuration in the follow link
// https://github.com/miketeachman/micropython-esp32-i2s-examples

#endif /* I2SCONFIG_H */

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/