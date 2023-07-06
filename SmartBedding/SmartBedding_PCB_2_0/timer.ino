/* *****************************************************************************
 * File:     timer.ino                                                         *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on April 2, 2021, 13:56 PM                                          *
 * *****************************************************************************/

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "timer.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// Tasks handlers
extern xTaskHandle tTimerHandle;

// Semaphores definition
extern SemaphoreHandle_t xSemaphoreTimer;
extern SemaphoreHandle_t xSemaphorePress;
extern SemaphoreHandle_t xSemaphoreMovem;

// flags to manage interruption
extern volatile uint8_t flagTimerInterrupt;

extern volatile uint8_t flNewMatrixSample;
extern volatile uint8_t flNewMovementSample;
extern bool flMatrixReady;   // flag to indicate that matrix measurement was performed
extern bool flMovementReady; // flag to indicate that movement measurement was performed

// counter to sample data without memory
extern volatile uint16_t countNewMatrixSample;
extern volatile uint16_t countNewMovementSample;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// initialization of timer counter
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// real timer counter
int hour = 0;
int minute = 0;
int second = 0;
int milisecond = 0;

/* ***************************************************************************
 * **** FUNCTIONS ************************************************************
 * ***************************************************************************/

// initTimer()
// @param none: void
// @return none: void
void initTimer(void)
{
    // Timer configuration at 1 kHz
    timer = timerBegin(0, 80, true);             // main clock frequency 80 MHz, prescaler 80, timer frec 1MHz
    timerAttachInterrupt(timer, &onTimer, true); // Atach onTimer function to the timer interrupt handler
    timerAlarmWrite(timer, 1000, true);          // interruption every 1000 timer counters, 1kHz
    timerAlarmEnable(timer);                     // Timer interrupt ISR enable
}

// onTimer()
// @param none: void
// @return none: void
void IRAM_ATTR onTimer(void)
{
    portENTER_CRITICAL_ISR(&timerMux);
    // enable flags and semaphores to work with timer
    flagTimerInterrupt = true;
    xSemaphoreGiveFromISR(xSemaphoreTimer, NULL); // Semaphore freedom
    portEXIT_CRITICAL_ISR(&timerMux);
}

/* *****************************************************************************
 * ****** TASK TO MANAGE TIMER INTERRUPTION HANDLER ****************************
 * *****************************************************************************/
void task_TimerHandler(void *arg)
{
    // Serial.println("Task timer initiated");

    while (1)
    {
        // Timer interrupt attention ISR
        if (xSemaphoreTake(xSemaphoreTimer, portMAX_DELAY) == pdTRUE)
        {
            if (flagTimerInterrupt == true)
            {
                portENTER_CRITICAL(&timerMux);
                flagTimerInterrupt = false;
                portEXIT_CRITICAL(&timerMux);

                /* ****************************************************************
                 * ****** TIMER INTERRUPT SEQUENCE ********************************
                 * ****************************************************************/
                // initiate a new matrix sample
                // if (!flNewMatrixSample)
                // {
                countNewMatrixSample += 1;
                if (countNewMatrixSample == NEW_MATRIX_SAMPLE_TIME)
                {
                    flNewMatrixSample = true;
                    flMatrixReady = false; // verify collision with memory
                    countNewMatrixSample = 0;
                    xSemaphoreGive(xSemaphorePress);
                }
                // }

                // initiate a new movement sample
                // if (!flNewMovementSample)
                // {
                countNewMovementSample += 1;
                if (countNewMovementSample == NEW_MOVEMENT_SAMPLE_TIME)
                {
                    flNewMovementSample = true;
                    flMovementReady = false; // verify collision with memory
                    countNewMovementSample = 0;
                    xSemaphoreGive(xSemaphoreMovem);
                }
                // }

                // Secuencia de prueba, contador en segundos, borrar
                milisecond++;
                if (milisecond == 1000)
                {
                    milisecond = 0;
                    second++;
                    // digitalWrite(2, !digitalRead(2));

                    if (second == 60)
                    {
                        second = 0;
                        minute++;

                        if (minute == 60)
                        {
                            minute = 0;
                            hour++;
                        }
                    }
                }
            }
        }
    }
    vTaskDelete(tTimerHandle);
}

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/