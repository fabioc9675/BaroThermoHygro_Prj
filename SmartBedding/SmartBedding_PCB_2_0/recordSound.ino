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
#include "recordSound.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// Tasks handlers
extern xTaskHandle tSoundHandle;

// variables to control the register in proccess
extern uint16_t regNumber;
extern uint16_t soundNumber;
extern String sdDir; // Variable String to save root dir
//extern String sPressF;
//extern String sImuF;

// Counter to know the data cuantity
extern int counterPress;
extern int counterMove;

extern int hour;
extern int minute;
extern int second;
extern int milisecond;

String tstamp = "";

// variables to make a delay to finis some delayed task
extern volatile uint8_t fl_newSoundMov;
extern volatile uint8_t fl_newSoundPre;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// record sound
const i2s_port_t i2s_port = I2S_NUM_0;

// wav file variables
const int recordTime = 60; // seconds
const int wavDataSize = recordTime * SAMPLE_RATE * 2 * 2;
const int numCommunicationData = 8192; // 2^13
const int numPartWavData = numCommunicationData / 2;

byte wavHeader[wavHeaderSize];
char communicationData[numCommunicationData];
char partWavData[numPartWavData]; //---

// file recording variables
File soundFile;
String soundName = "";
/******************************/

String sPressF;
String sImuF;

//----------------------------------
char soundFileName[50];
char soundState = noneState;
extern char pressFileName[50]; // Variable char to save press file name
extern char moveFileName[50];

// Files variables to store the acquired data
extern File pressFile;
extern File movemFile;
extern volatile uint8_t fl_movFileOpen;
extern volatile uint8_t fl_preFileOpen;

/* ***************************************************************************
 * **** FUNCTIONS ************************************************************
 * ***************************************************************************/

// recordSetup()
// @param none: void
// @return none: void
void recordSetup(void)
{
  soundName.reserve(100); // sound name memory reserve
  soundName = String("");
  soundNumber = 0;
  soundState = noneState;
  sPressF.reserve(50);
  sPressF = "";
  sImuF.reserve(50);
  sImuF = "";

  // creation of wav header
  createWavHeader(wavHeader, wavDataSize);
}

/* *****************************************************************************
 * **** TASK TO RECORD DATA SOUND **********************************************
 * *****************************************************************************/
void task_RecordSound(void *arg)
{
  esp_err_t err = ESP_OK;       // variable to control error
  uint16_t contAudioSample = 0; // counter control audio sample
  size_t bytesRead = 0;

  recordSetup();
  I2S_Init(i2s_port);
  soundState = headingFile;

  Serial.println("Task record sound initiated");
  // delay for initiate task
  vTaskDelay(200 / portTICK_PERIOD_MS);

  // infinity loop
  while (1)
  {
    switch (soundState)
    {
    /* Case to create and add header to the file */
    case headingFile:
      err = ESP_OK;
      // creation of file data wav to record sound
      soundNumber += 1;
      soundName = sdDir + "/Sound" + String(soundNumber) + ".wav";
      soundName.toCharArray(soundFileName, soundName.length() + 1);
      //*****************************************************************************
      sPressF = sdDir + "/RegPress_" + String(regNumber) + "S" + String(soundNumber) + ".txt";
      sPressF.toCharArray(pressFileName, sPressF.length() + 1);
      sImuF = sdDir + "/RegMove_" + String(regNumber) + "S" + String(soundNumber) + ".txt";
      sImuF.toCharArray(moveFileName, sImuF.length() + 1);
      // writeFile(SD, pressFileName, "Pressure Data Register\r\n");
      // writeFile(SD, moveFileName, "Movement Data Register\r\n");
      movemFile = SD.open(moveFileName, FILE_WRITE);
      pressFile = SD.open(pressFileName, FILE_WRITE);
      movemFile.print("Movement Data Register\r\n");
      pressFile.print("Pressure Data Register\r\n");
      fl_movFileOpen = true;
      fl_preFileOpen = true;

      //********************************************************************************
      // open file to write data
      soundFile = SD.open(soundFileName, FILE_WRITE);
      if (!soundFile)
      {
        err = ESP_FAIL;
        soundState = noneState;
        if (err == ESP_FAIL)
        {
          indicatorSetColor(CYAN);
        }
        break;
      }
      // writing header into file
      soundFile.write(wavHeader, wavHeaderSize);
      contAudioSample = 0;
      // go to the next state
      soundState = recordingData;

      Serial.println("Begin Recording");
      break;

    /* Case to sample and record audio file */
    case recordingData:

      if (contAudioSample < (wavDataSize / numPartWavData))
      {
        contAudioSample += 1;
        // Read audio data from I2S interface
        err = I2S_ReadData(i2s_port, communicationData, numCommunicationData, &bytesRead);
        // Prepare and write audio data in SD card
        for (int i = 0; i < numCommunicationData / 4; i++)
        {
          partWavData[2 * i] = communicationData[4 * i + 2];
          partWavData[2 * i + 1] = communicationData[4 * i + 3];
        }
        soundFile.write((const byte *)partWavData, numPartWavData);
      }
      else
      {
        soundState = closingFile;
      }
      break;

    /* Case to close audio file */
    case closingFile:
      // Audio file close
      soundFile.close();

      movemFile.close();
      pressFile.close();
      fl_movFileOpen = false;
      fl_preFileOpen = false;

      // initiate a new recording
      soundState = headingFile;

      Serial.println("Finish");

      Serial.print("PressCont = ");
      Serial.println(counterPress);
      Serial.print("MovementCount = ");
      Serial.println(counterMove);

      Serial.print("TimeStamp = ");
      Serial.println(esp_log_timestamp() / 1000);

      tstamp = String(hour) + ":" + String(minute) + ":" + String(second);

      Serial.print("internal Time = ");
      Serial.println(tstamp);

      counterPress = 0;
      counterMove = 0;

      fl_newSoundMov = true;
      fl_newSoundPre = true;

      vTaskDelay(5000 / portTICK_PERIOD_MS);
      break;

    /* in case of error repeat the process */
    case noneState:
      soundState = headingFile;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      break;

    default:
      break;
    }
  }
  vTaskDelete(tSoundHandle);
}

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/