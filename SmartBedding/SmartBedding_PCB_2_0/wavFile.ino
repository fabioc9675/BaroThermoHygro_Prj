/* *****************************************************************************
 * File:     wavFile.ino                                                       *
 * Project:  Smart Bedding                                                     *
 * Author:   GIBIC Medtech                                                     *
 * Version:  V_1.1                                                             *
 *                                                                             *
 * Created on March 29, 2021, 12:31 PM                                         *
 * *****************************************************************************/

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "wavFile.h"

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
void createWavHeader(byte *header, int waveDataSize)
{

    uint32_t fileSize = waveDataSize + wavHeaderSize - 8; // minus 8 because the protocole

    header[0] = 'R';
    header[1] = 'I';
    header[2] = 'F';
    header[3] = 'F';
    header[4] = (byte)(fileSize & 0xFF);
    header[5] = (byte)((fileSize >> 8) & 0xFF);
    header[6] = (byte)((fileSize >> 16) & 0xFF);
    header[7] = (byte)((fileSize >> 24) & 0xFF);
    header[8] = 'W';
    header[9] = 'A';
    header[10] = 'V';
    header[11] = 'E';
    header[12] = 'f';
    header[13] = 'm';
    header[14] = 't';
    header[15] = ' ';
    header[16] = 0x10; // format data length = 16 bits
    header[17] = 0x00;
    header[18] = 0x00;
    header[19] = 0x00;
    header[20] = 0x01; // linear PCM
    header[21] = 0x00;
    header[22] = 0x02; // number of channels = 2
    header[23] = 0x00;
    header[24] = 0x44; // sampling rate = 44100 = 0xAC44
    header[25] = 0xAC;
    header[26] = 0x00;
    header[27] = 0x00;
    header[28] = 0x10; // Bytes/sec = 44100*16*2/8 = 176400 = 0x2B110
    header[29] = 0xB1;
    header[30] = 0x02;
    header[31] = 0x00;
    header[32] = 0x04; // Bytes/sample = 16*2/8 = 0x04
    header[33] = 0x00;
    header[34] = 0x10; // bits/sample = 16
    header[35] = 0x00;
    header[36] = 'd';
    header[37] = 'a';
    header[38] = 't';
    header[39] = 'a';
    header[40] = (byte)(waveDataSize & 0xFF); // size of the data section
    header[41] = (byte)((waveDataSize >> 8) & 0xFF);
    header[42] = (byte)((waveDataSize >> 16) & 0xFF);
    header[43] = (byte)((waveDataSize >> 24) & 0xFF);
}

/********************** (C) COPYRIGHT GIBIC MedTech ****** END OF FILE ****/