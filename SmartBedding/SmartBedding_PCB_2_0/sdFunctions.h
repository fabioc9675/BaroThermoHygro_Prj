/* *****************************************************************************
 * File:     sdFunctions.h                                                     *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on March 29, 2021, 12:31 PM                                         *
 * *****************************************************************************/

#ifndef SDFUNCTIONS_H
#define SDFUNCTIONS_H

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "Arduino.h"
#include <Esp.h>
#include <stdio.h>

#include <FS.h>
#include <SD.h>
#include <SPI.h>

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ PROTOTYPES ****************************************************
 ** ****************************************************************************/

// createDir()
// @param fs: fileSystem object
// @param path: path dir
// @return err: esp_err_t
esp_err_t createDir(fs::FS &fs, const char *path);

// removeDir()
// @param fs: fileSystem object
// @param path: path dir
// @return err: esp_err_t
esp_err_t removeDir(fs::FS &fs, const char *path);

// writeFile()
// @param fs: fileSystem object
// @param path: path dir
// @param message = message to write
// @return err: esp_err_t
esp_err_t writeFile(fs::FS &fs, const char *path, const char *message);

// appendFile()
// @param fs: fileSystem object
// @param path: path dir
// @param message = message to append
// @return err: esp_err_t
esp_err_t appendFile(fs::FS &fs, const char *path, const char *message);

// readFile()
// @param fs: fileSystem object
// @param path: path dir
// @param message = message to append
// @param data: pointer to save Data
// @param beginP: where begint the read
// @param dataLength: length of data to read
// @return err: esp_err_t
esp_err_t readFile(fs::FS &fs, const char *path, uint8_t *data, int beginP, int dataLength);

// deleteFile()
// @param fs: fileSystem object
// @param path: path dir
// @return err: esp_err_t
esp_err_t deleteFile(fs::FS &fs, const char *path);

// listDir()
// @param fs: fileSystem object
// @param dirname: path dir
// @param levels: levels
// @return none: void
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

/* ******************************************************************************
 * **** AUDIO RECORDING FUNCTIONS ***********************************************
 * ******************************************************************************/

// writeFile()
// @param fs: fileSystem object
// @param file: file address to create
// @param path: path dir
// @param message = message to write
// @return err: esp_err_t
esp_err_t createAudioFile(fs::FS &fs, File *file, const char *path, const char *header);

// writeAudioFile()
// @param file: audio file to write
// @param path: path dir
// @param message = message to write
// @return err: esp_err_t
esp_err_t writeAudioFile(File *file, const char *path, const char *data);

// closeAudioFile()
// @param file: audio file to write
// @param path: path dir
// @param message = message to write
// @return err: esp_err_t
esp_err_t closeAudioFile(File *file);

#endif /* SDFUNCTIONS_H */

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/