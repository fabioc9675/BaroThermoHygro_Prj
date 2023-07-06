/* *****************************************************************************
 * File:     matrixSample.ino                                                  *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on April 4, 2021, 20:45 PM                                          *
 * *****************************************************************************/

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "matrixSample.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// Task handlers
extern xTaskHandle tPressAdqHandle;

// Semaphores definition
extern SemaphoreHandle_t xSemaphorePress;

// Object to control pin expander chip
extern TwoWire i2c_gp; // I2C to control mcp peripheral

// variable to save the pressure data into a matrix
extern int16_t matrix[ROW_SIZE][COL_SIZE];
extern String dataMatrix;

// flags to execute program
extern bool flMatrixReady; // flag to indicate that matrix measurement was performed
extern volatile uint8_t flNewMatrixSample;

// counter to sample data without memory
extern volatile uint16_t countNewMatrixSample;

// number of sound assosited with matrix sample
extern uint16_t soundNumber;

// Counter to know the data cuantity
extern int counterPress;

// variables to make a delay to finis some delayed task
extern volatile uint8_t fl_newSoundPre;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// object to control mcp23017 chip
MCP23017 mcp = MCP23017(MCP_ADDR, i2c_gp);

// object to control ads1015 chip
ADS1015 ads;
uint16_t pressADC = 0;
int16_t pressValue = 0;

// state machine control variable
uint8_t pressState = nonePress;

// Local variable to save data Matrix
String lDataMatrix;

/* ***************************************************************************
 * **** FUNCTIONS ************************************************************
 * ***************************************************************************/
// matrixSetup()
// @param none: void
// @return err: esp_err_t
esp_err_t matrixSetup(void)
{
    esp_err_t err = ESP_OK;

    // mcp initialization
    mcp.init();
    mcp.portMode(MCP23017Port::A, 0);                  // Port A as output
    mcp.writeRegister(MCP23017Register::GPIO_A, 0x00); // Reset port A, output state = 0

    // ads initialization
    if (!ads.begin(ADS_ADDR, i2c_gp))
    {
        err = ESP_FAIL;
    }
    ads.setGain(ADS1015_CONFIG_PGA_1); // Gain configuration

    return err;
}

// shiftRegisterOut()
// @param dataPin: uint8_t
// @param clockPin: uint8_t
// @param val: uint32_t
// @return none: void
void shiftRegisterOut(uint32_t val)
{
    uint8_t shCount;
    uint8_t dataSR = 0; // data to shift out

    for (shCount = 0; shCount < 34; shCount++)
    {
        if (shCount % 2)
        {
            digitalWrite(SR_CLOCK_PIN, LOW);
            digitalWrite(SR_DATAW_PIN, dataSR);
        }
        else
        {
            digitalWrite(SR_CLOCK_PIN, HIGH);
            dataSR = (val >> shCount / 2) & 0x01;
        }
    }
}

/* *****************************************************************************
 * **** TASK TO RECORD DATA PRESSURE *******************************************
 * *****************************************************************************/
void task_PressAdq(void *arg)
{
    esp_err_t err = ESP_OK;
    // Peripheral initialization
    err = matrixSetup();
    if (err == ESP_FAIL)
    {
        indicatorSetColor(MAGENTA);
    }

    // internal variables to control the pressure sample
    uint8_t rowPos = 0; // row position
    uint8_t colPos = 0; // column position

    // initializate data memory
    dataMatrix.reserve(MATRIX_SIZE);
    lDataMatrix.reserve(MATRIX_SIZE);
    dataMatrix = "";
    lDataMatrix = "";

    Serial.println("Task press adq initiated");

    // delay for initiate task
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // infinity loop
    while (1)
    {

        switch (pressState)
        {
        case updateRows:
            /* update rows to shif register */
            shiftRegisterOut(rowArray[rowPos]);
            pressState = sampleSensor;
            digitalWrite(SR_LATCH_PIN, HIGH);
            digitalWrite(SR_LATCH_PIN, LOW);

            break;

        case sampleSensor:
            /* sample sensor */
            if (colPos < 12)
            {
                // update selected column
                mcp.writePort(MCP23017Port::A, colArray[colPos]);
                // Acquire ADC value of pressure and cast for negative values
                pressADC = ads.getSingleEnded(ADS_CHANNEL_0);
                if (pressADC > 2047)
                {
                    pressValue = -1 * ((int16_t)(~pressADC) & 0X0FFF); // negative values
                }
                else
                {
                    pressValue = (int16_t)pressADC; // positive values
                }
                // save measured data into matrix
                matrix[rowPos][colPos] = pressValue;

                colPos += 1; // increae column position
            }
            else
            {

                rowPos += 1; // increase row position
                colPos = 0;  // initiate column position
                // verify if already sample all the rows

                //                delay(5000);
                //                Serial.print(rowPos);
                if (rowPos == 16)
                {
                    rowPos = 0;
                    pressState = composeData;
                }
                else
                {
                    pressState = updateRows;
                }
            }
            break;

        case composeData:
            /* Compose string data to store in memory */
            pressState = nonePress;

            lDataMatrix = "S" + String(soundNumber) + " map = [";
            for (int i = 0; i < ROW_SIZE; i++)
            {
                for (int j = 0; j < COL_SIZE; j++)
                {
                    // Serial.print(matrix[i][j]);
                    lDataMatrix += String(matrix[i][j]);
                    if (j != (COL_SIZE - 1))
                    {
                        // Serial.print(", ");
                        lDataMatrix += ",";
                    }
                }
                if (i != (ROW_SIZE - 1))
                {
                    // Serial.print("; ");
                    lDataMatrix += ";";
                }
            }

            counterPress += 1;
            // Serial.print("\r\n");
            lDataMatrix += "]*";
            lDataMatrix += String(counterPress) + "\r\n";
            // Serial.println(dataMatrix);
            // Serial.println("*****");

            // create a copy of data Matrix
            dataMatrix = lDataMatrix;

            flMatrixReady = true; // data ready to send
            break;

        case nonePress:
            /* initiate sample */
            if (xSemaphoreTake(xSemaphorePress, portMAX_DELAY) == pdTRUE)
            {
                // if ((!flMatrixReady) && (flNewMatrixSample))
                // {
                flNewMatrixSample = false;
                countNewMatrixSample = 0;
                rowPos = 0; // initiate row position
                colPos = 0; // initiate column position
                pressState = updateRows;

                if (fl_newSoundPre == true)
                {
                    fl_newSoundPre = false;

                    vTaskDelay(5000);
                    lDataMatrix = "";
                    dataMatrix = "";
                }

                // vTaskDelay(1000 / portTICK_PERIOD_MS); // delay of 1 second before the next sample
                // }
                // else
                // {
                //     vTaskDelay(100 / portTICK_PERIOD_MS); // delay of 1 second before the next sample
                // }
            }

            break;

        default:
            break;
        }
    }
    vTaskDelete(tPressAdqHandle);
}

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/
