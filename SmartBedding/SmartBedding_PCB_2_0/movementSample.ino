/* *****************************************************************************
 * File:     movementSample.ino                                                *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on April 7, 2021, 13:30 PM                                          *
 * *****************************************************************************/

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "movementSample.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// Task handlers
extern xTaskHandle tMovAdqHandle;

// Semaphores definition
extern SemaphoreHandle_t xSemaphoreMovem;

// Objects to control IMU
extern SPIClass vspi; // IMU SPI controller

// variable to save the movement data into a string
extern String dataMovement;

// flags to execute program
extern bool flMovementReady; // flag to indicate that movement measurement was performed
extern volatile uint8_t flNewMovementSample;

// counter to sample data without memory
extern volatile uint16_t countNewMovementSample;

// number of sound assosited with movement sample
extern uint16_t soundNumber;

// Counter to know the data cuantity
extern int counterMove;

// variables to make a delay to finis some delayed task
extern volatile uint8_t fl_newSoundMov;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// object to control mpu9250 chip
MPU9250 mpu(vspi, VSPI_SS_P);

// state machine control variable
uint8_t movementState = noneMov;

// control the quantity of samples before record
uint8_t countMovSamples = 0;

// flag to clean string
volatile uint8_t flCleanMovString = false;

// variables to acquire sensor data
float Ax, Ay, Az; // acceleration variables
float Gx, Gy, Gz; // Gyroscope variables
float Mx, My, Mz; // magnetometer variables

// Local variable to save data Movement
String lDataMovement;

/* ***************************************************************************
 * **** FUNCTIONS ************************************************************
 * ***************************************************************************/
// movementSetup()
// @param none: void
// @return err: esp_err_t
esp_err_t movementSetup(void)
{
    esp_err_t err = ESP_OK;
    int movStatus = 0;

    // mpu initialization
    movStatus = mpu.begin();
    if (movStatus < 0)
    {
        // Serial.println(movStatus);
        err = ESP_FAIL;
    }

    return err;
}

/* *****************************************************************************
 * **** TASK TO ACQUIRE DATA IMU ***********************************************
 * *****************************************************************************/
void task_MovAdq(void *arg)
{

    esp_err_t err = ESP_OK;
    int dataStatus = 0;
    // Peripheral initialization
    err = movementSetup();
    if (err == ESP_FAIL)
    {
        indicatorSetColor(MAGENTA);
    }

    // initializate data memory
    dataMovement.reserve(MOVEMENT_SIZE);
    lDataMovement.reserve(MOVEMENT_SIZE);
    dataMovement = "";
    lDataMovement = "";

    Serial.println("Task movement adq initiated");

    // delay for initiate task
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // infinity loop
    while (1)
    {

        switch (movementState)
        {

        case adqMovement:
            /* sample sensor */
            movementState = composeMov;

            // acquire data from mpu9250
            dataStatus = mpu.readSensor();

            if (dataStatus == 1)
            { // read status OK
                Ax = mpu.getAccelX_mss();
                Ay = mpu.getAccelY_mss();
                Az = mpu.getAccelZ_mss();
                Gx = mpu.getGyroX_rads();
                Gy = mpu.getGyroY_rads();
                Gz = mpu.getGyroZ_rads();
                Mx = mpu.getMagX_uT();
                My = mpu.getMagY_uT();
                Mz = mpu.getMagZ_uT();
            }
            else
            { // read status FAIL
                Ax = 0;
                Ay = 0;
                Az = 0;
                Gx = 0;
                Gy = 0;
                Gz = 0;
                Mx = 0;
                My = 0;
                Mz = 0;
            }
            dataStatus = 0;

            break;

        case composeMov:
            /* Compose string data to store in memory */
            movementState = noneMov;
            countMovSamples += 1;

            // data composition
            lDataMovement += "S" + String(soundNumber) + " mov = [";
            lDataMovement += String(Ax, MOV_ROUND) + ",";
            lDataMovement += String(Ay, MOV_ROUND) + ",";
            lDataMovement += String(Az, MOV_ROUND) + ",";
            lDataMovement += String(Gx, MOV_ROUND) + ",";
            lDataMovement += String(Gy, MOV_ROUND) + ",";
            lDataMovement += String(Gz, MOV_ROUND) + "]*";
            // lDataMovement += String(Mx, MOV_ROUND) + ",";
            // lDataMovement += String(My, MOV_ROUND) + ",";
            // lDataMovement += String(Mz, MOV_ROUND) + "]*";

            counterMove += 1;

            lDataMovement += String(counterMove) + "\r\n";

            if (countMovSamples >= MOVEMENT_SAMPLES)
            {
                countMovSamples = 0;
                flMovementReady = true; // data ready to send
                flCleanMovString = true;

                // Serial.println(dataMovement);
                // Serial.println("++++++");
            }

            break;

        case noneMov:
            /* initiate sample */
            if (xSemaphoreTake(xSemaphoreMovem, portMAX_DELAY) == pdTRUE)
            {

                // create a copy of data Matrix
                dataMovement = lDataMovement;

                // if ((!flMovementReady) && (flNewMovementSample))
                // {
                flNewMovementSample = false;
                countNewMovementSample = 0;
                movementState = adqMovement;

                if (flCleanMovString)
                {
                    flCleanMovString = false;
                    lDataMovement = "";
                }

                if (fl_newSoundMov == true)
                {
                    fl_newSoundMov = false;

                    vTaskDelay(5000);
                    lDataMovement = "";
                    dataMovement = "";
                }

                // vTaskDelay(1000 / portTICK_PERIOD_MS); // delay of 1 second before the next sample
                // }
                // else
                // {
                //     vTaskDelay(10 / portTICK_PERIOD_MS); // delay of 1 second before the next sample
                // }
            }

            break;

        default:
            break;
        }
    }
    vTaskDelete(tMovAdqHandle);
}

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/