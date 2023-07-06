/* *****************************************************************************
 * File:     storeData.ino                                                     *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on April 8, 2021, 17:47  PM                                         *
 * *****************************************************************************/

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "storeData.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

extern BLECharacteristic *pCharacteristic_tx;
extern BLECharacteristic *pCharacteristic_rx;
extern BLECharacteristic *pCharacteristic_no;

extern String data_no;
extern String data_tx;
extern char buf_tx[500];
extern char buf_no[500];
extern int16_t matrix[ROW_SIZE][COL_SIZE];

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// Tasks handlers
extern xTaskHandle tStoreHandle;

// flags to execute program
extern bool flMatrixReady;   // flag to indicate that matrix measurement was performed
extern bool flMovementReady; // flag to indicate that movement measurement was performed

// variable to save the pressure data into a matrix
extern String dataMatrix;

// variable to save the movement data into a string
extern String dataMovement;

// variables to control the register in proccess
extern char rootDir[50];       // Variable char to save root dir
extern char pressFileName[50]; // Variable char to save press file name
extern char moveFileName[50];  // variable char to save move file name

// Files variables to store the acquired data
extern File pressFile;
extern File movemFile;
extern volatile uint8_t fl_movFileOpen;
extern volatile uint8_t fl_preFileOpen;

// variable to copy file name
char pressFileCopy[50];
char moveFileCopy[50];

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/* ***************************************************************************
 * **** FUNCTIONS ************************************************************
 * ***************************************************************************/

/* *****************************************************************************
 * **** TASK TO STORE DATA SENSORS *********************************************
 * *****************************************************************************/
void task_storeData(void *arg)
{
    esp_err_t err = ESP_OK; // variable to control error

    Serial.println("Task store sensor initiated");
    // delay for initiate task
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // infinity loop
    while (1)
    {
        // Write file with pressure matrix information
        if (flMatrixReady & fl_preFileOpen)
        {
            flMatrixReady = false;
            // pressFile.print(dataMatrix.c_str());

            // Serial.println(dataMatrix);
            // Serial.println("*****");

            data_no = "Ini";
            data_no.toCharArray(buf_no, data_no.length() + 1);
            pCharacteristic_no->setValue(buf_no);
            pCharacteristic_no->notify();
            Serial.println("fin");

            int matrix2[192];
            for (int h = 0; h < 12; h++)
            {
                for (int k = 0; k < 16; k++)
                {
                    matrix2[16 * h + k] = matrix[k][h];
                }
            }

            for (int i = 0; i < 48; i++)
            {
                data_no = String(matrix2[4 * i]) + ',' + String(matrix2[4 * i + 1]) + ',' + String(matrix2[4 * i + 2]) + ',' + String(matrix2[4 * i + 3]);
                if (i < 47)
                {
                    data_no = data_no + ',';
                }
                Serial.print(data_no);
                data_no.toCharArray(buf_no, data_no.length() + 1);
                pCharacteristic_no->setValue(buf_no);
                pCharacteristic_no->notify();
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            data_no = "Fin";
            data_no.toCharArray(buf_no, data_no.length() + 1);
            pCharacteristic_no->setValue(buf_no);
            pCharacteristic_no->notify();
            Serial.println("fin");

            // vTaskDelay(15000 / portTICK_PERIOD_MS);

            // strcpy(pressFileCopy, pressFileName);
            // err = appendFile(SD, pressFileCopy, dataMatrix.c_str()); // append pressure data into register file
            // // Serial.println("******");
            // if (err == ESP_FAIL)
            // {
            //     indicatorSetColor(RED);
            // }
        }

        // Write file with movement matrix information
        if (flMovementReady & fl_movFileOpen)
        {
            flMovementReady = false;
            // movemFile.print(dataMovement.c_str());

            // Serial.println(dataMovement);
            // Serial.println("+++++");

            // strcpy(moveFileCopy, moveFileName);
            // err = appendFile(SD, moveFileCopy, dataMovement.c_str()); // append movement data into register file
            // Serial.println("------");
            // if (err == ESP_FAIL)
            // {
            //     indicatorSetColor(RED);
            // }
        }

        vTaskDelay(10); // stop task to attend other actions
    }
    vTaskDelete(tStoreHandle);
}

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/