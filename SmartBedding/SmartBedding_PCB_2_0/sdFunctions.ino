/* *****************************************************************************
 * File:     sdFunctions.ino                                                   *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on March 29, 2021, 12:31 PM                                         *
 * *****************************************************************************/

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "sdFunctions.h"

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

// createDir()
// @param fs: fileSystem object
// @param path: path dir
// @return err: esp_err_t
esp_err_t createDir(fs::FS &fs, const char *path)
{
    esp_err_t err = ESP_OK;
    if (fs.mkdir(path))
    {
        err = ESP_OK;
    }
    else
    {
        err = ESP_FAIL;
    }
    return err;
}

// removeDir()
// @param fs: fileSystem object
// @param path: path dir
// @return err: esp_err_t
esp_err_t removeDir(fs::FS &fs, const char *path)
{
    esp_err_t err = ESP_OK;
    if (fs.rmdir(path))
    {
        err = ESP_OK;
    }
    else
    {
        err = ESP_FAIL;
    }
    return err;
}

// writeFile()
// @param fs: fileSystem object
// @param path: path dir
// @param message = message to write
// @return err: esp_err_t
esp_err_t writeFile(fs::FS &fs, const char *path, const char *message)
{
    esp_err_t err = ESP_OK;
    // Serial.printf("Writing file: %s\n", path);
    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        //Serial.println("Failed to open file for writing");
        err = ESP_FAIL;
        return err;
    }
    if (file.print(message))
    {
        //Serial.println("File written");
        err = ESP_OK;
    }
    else
    {
        // Serial.println("Write failed");
        err = ESP_FAIL;
    }
    file.close();

    return err;
}

// appendFile()
// @param fs: fileSystem object
// @param path: path dir
// @param message = message to append
// @return err: esp_err_t
esp_err_t appendFile(fs::FS &fs, const char *path, const char *message)
{
    esp_err_t err = ESP_OK;
    //Serial.printf("Appending to file: %s\n", path);
    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        //Serial.println("Failed to open file for appending");
        err = ESP_FAIL;
        return err;
    }
    if (file.print(message))
    {
        //Serial.println("Message appended");
        err = ESP_OK;
    }
    else
    {
        //Serial.println("Append failed");
        err = ESP_FAIL;
    }
    file.close();

    return err;
}

// readFile()
// @param fs: fileSystem object
// @param path: path dir
// @param message = message to append
// @param data: pointer to save Data
// @param beginP: where begint the read
// @param dataLength: length of data to read
// @return err: esp_err_t
esp_err_t readFile(fs::FS &fs, const char *path, uint8_t *data, int beginP, int dataLength)
{
    esp_err_t err = ESP_OK;
    //Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
        //Serial.println("Failed to open file for reading");
        err = ESP_FAIL;
        return err;
    }

    // going to specific position into file
    if (file.seek(beginP))
    {
        err = ESP_OK;
    }
    else
    {
        err = ESP_FAIL;
        file.close();
        return err;
    }

    // read data buffer size controlled
    if (file.read(data, dataLength) > 0)
    {
        err = ESP_OK;
    }
    else
    {
        err = ESP_FAIL;
    }

    file.close();
    return err;
}

// deleteFile()
// @param fs: fileSystem object
// @param path: path dir
// @return err: esp_err_t
esp_err_t deleteFile(fs::FS &fs, const char *path)
{
    esp_err_t err = ESP_OK;
    if (fs.remove(path))
    {
        err = ESP_OK;
    }
    else
    {
        err = ESP_FAIL;
    }
    return err;
}

// listDir()
// @param fs: fileSystem object
// @param dirname: path dir
// @param levels: levels
// @return none: void
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    //Serial.printf("Listing directory: %s\n", dirname);
    File root = fs.open(dirname);
    if (!root)
    {
        //Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        //Serial.println("Not a directory");
        return;
    }
    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            //Serial.print("  DIR : ");
            //Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            //Serial.print("  FILE: ");
            //Serial.print(file.name());
            //Serial.print("  SIZE: ");
            //Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

/* ******************************************************************************
 * **** AUDIO RECORDING FUNCTIONS ***********************************************
 * ******************************************************************************/

// writeFile()
// @param fs: fileSystem object
// @param file: file address to create
// @param path: path dir
// @param message = message to write
// @return err: esp_err_t
esp_err_t createAudioFile(fs::FS &fs, File *file, const char *path, const char *header)
{
    esp_err_t err = ESP_OK;
    // Serial.printf("Writing file: %s\n", path);
    *file = fs.open(path, FILE_WRITE);
    if (!(*file))
    {
        //Serial.println("Failed to open file for writing");
        err = ESP_FAIL;
        return err;
    }
    if (file->print(header))
    {
        //Serial.println("File written");
        err = ESP_OK;
    }
    else
    {
        // Serial.println("Write failed");
        err = ESP_FAIL;
    }
    // file.close();
    return err;
}

// writeAudioFile()
// @param file: audio file to write
// @param path: path dir
// @param message = message to write
// @return err: esp_err_t
esp_err_t writeAudioFile(File *file, const char *path, const char *data)
{
    esp_err_t err = ESP_OK;
    // Serial.printf("Writing file: %s\n", path);
    if (!(*file))
    {
        //Serial.println("Failed to open file for writing");
        err = ESP_FAIL;
        return err;
    }
    if (file->print(data))
    {
        //Serial.println("File written");
        err = ESP_OK;
    }
    else
    {
        // Serial.println("Write failed");
        err = ESP_FAIL;
    }
    // file.close();

    return err;
}

// closeAudioFile()
// @param file: audio file to write
// @param path: path dir
// @param message = message to write
// @return err: esp_err_t
esp_err_t closeAudioFile(File *file)
{
    esp_err_t err = ESP_OK;
    // Serial.printf("Writing file: %s\n", path);
    if (!(*file))
    {
        //Serial.println("Failed to open file for writing");
        err = ESP_FAIL;
        return err;
    }
    file->close();

    return err;
}

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/