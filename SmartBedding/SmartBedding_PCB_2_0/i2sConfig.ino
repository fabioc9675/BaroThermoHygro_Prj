/* *****************************************************************************
 * File:     i2sConfig.ino                                                     *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on March 29, 2021, 12:31 PM                                         *
 * *****************************************************************************/

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "i2sConfig.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/* ***************************************************************************
 * **** FUNCTIONS ************************************************************
 * ***************************************************************************/

// @param mode : I2S_MODE_RX or I2S_MODE_TX
// @param port : pointer to port object
// @param BPS : I2S_BITS_PER_SAMPLE_16BIT or I2S_BITS_PER_SAMPLE_32BIT
// @return err : esp_err_t
esp_err_t I2S_Init(i2s_port_t i2s_port)
{
    esp_err_t err = ESP_OK; // error control variable

    // I2S configuration
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
        .sample_rate = SAMPLE_RATE,                        // 44100 Hz
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,      // work with 16 bits
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,      // use left and right channels
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Interrupt level 1
        .dma_buf_count = 32,                      // number of buffers
        .dma_buf_len = 120                        // 8 samples per buffer (minimum)
    };

    // Pin configuration fos I2S
    const i2s_pin_config_t pin_config = {
        .bck_io_num = PIN_I2S_BCK,         // I2S serial clock
        .ws_io_num = PIN_I2S_LRC,          // I2S word select
        .data_out_num = I2S_PIN_NO_CHANGE, // I2S pin no used (only for speakers)
        .data_in_num = PIN_I2S_DIN,        // I2S serial data input
    };

    // Configuring the I2S driver and pins, this function must be called before any I2S driver read/write operations.
    err = i2s_driver_install(i2s_port, &i2s_config, 0, NULL);
    if (err != ESP_OK)
    {
        return err;
    }
    // Set i2s pin configuration
    err = i2s_set_pin(i2s_port, &pin_config);
    if (err != ESP_OK)
    {
        return err;
    }
    // err = i2s_set_clk(i2s_port, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);

    return err;
}

// I2S_ReadData() for I2S_MODE_RX
// @param i2s_port: port to read
// @param data: pointer to buffer
// @param numData: buffer size
// @param bytesRead: bytes already read
// @return err: esp_err_t
esp_err_t I2S_ReadData(i2s_port_t i2s_port, char *data, size_t numData, size_t *bytesRead)
{
    // read operation in I2S protocol
    return i2s_read(i2s_port, (char *)data, numData, bytesRead, portMAX_DELAY);
}



// I2S_Write() for I2S_MODE_TX
// @param data: pointer to buffer
// @param numData: buffer size
// void I2S_Write(char *data, int numData);

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/