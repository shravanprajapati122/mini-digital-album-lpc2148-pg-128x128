/*==========================================================================================
this sd card interface was adopted from SD interface written by Daniel D. March 2009
===========================================================================================*/
#include "sd.h"
#include "spi.h"
#include <stdlib.h>

/*------------------------------------------------------------------
Function : to initialize the SD card in SPI mode

Returns  : 0 = success; 1 = ERROR

Arguments: None
-------------------------------------------------------------------*/
char SD_init(void) {
    char j, response;
    unsigned int i;

    for (j = 0; j < 10; j++) //at least 74 clocks to take card in to SPI mode
    {
        SPI_write(0xff);
    }

    i = 0;

    do {
        response = SD_sendCommand(GO_IDLE_STATE, 0); //send 'reset & go idle' command untill card becomes ready
    } while (response != 0x01 && i++ < 0xfffe);

    if (i > 0xfffe) {
        return 1;     //loop was exited due to timeout, card is not responding (too bad)
    }
    SD_CS_DEASSERT;              //deselect card
    SPI_write(0xff);
    SPI_write(0xff);

    i = 0;

    do {
        response = SD_sendCommand(SEND_OP_COND, 0); //activate card's initialization process
        response = SD_sendCommand(SEND_OP_COND, 0); //same command sent again for compatibility with some cards 
    } while (response != 0x00 && i++ < 0xfffe);

    if (i > 0xfffe) {
        return 1;                      // loop was exited due to timeout (not again!)
    }
    SD_sendCommand(SET_BLOCK_LEN, 512); //set block size to 512

    SD_CS_DEASSERT;                     //deselect card

    return 0;                          //Yay! Successfully initialized
}

/*------------------------------------------------------------------
Function :  To send a standard command to SD/MMC 

Returns  : Card response

Arguments: Command to transmit & command arguments

Note:      See available commands in "SD.h"
-------------------------------------------------------------------*/
char SD_sendCommand(char cmd, unsigned long arg)
{
    char response;
    unsigned long retry = 0;
    SD_CS_ASSERT;             //select card (activate bus)

    SPI_write(cmd | 0x40);    //send command, first two bits always '01'
    SPI_write(arg >> 24);       //send command argument (32 bit) a byte at a time starting from MSB
    SPI_write(arg >> 16);
    SPI_write(arg >> 8);
    SPI_write(arg);
    SPI_write(0x95);          //pre-calculated checksum(see SD Card manual)

    while (((response = SPI_read()) == 0xff) && (retry++ < 0x0fffffff));  //wait response

    SPI_read();                                                          //extra 8 CLK

    return response;                                                     //return card response
}

/*---------------------------------------------------------------------------------
Function : to read a single image from SD/MMC (2048 Bytes)

Returns  : 0 for success, 1 for error

Arguments: starting address on the card from where data is to be read, and buffer where read data will be stored

Note     : Card access is Block-wise and each block has 512 Bytes (just like a Hard disk)
-----------------------------------------------------------------------------------*/
char SD_readImage(unsigned long startBlock,unsigned char *img)
{
    char response;
    unsigned long  retry = 0;
    unsigned int max, i, start;

    for (start = 0; start < 2048; start += 512) {
        response = SD_sendCommand(READ_SINGLE_BLOCK, startBlock * 512 + start); //read a Block command

        if (response != 0x00) //check for SD status: 0x00 - OK (No flags set)
        {
            return response;             //at least one Error flag is up, return error
        }
        retry = 0;

        while ((SPI_read() != 0xfe) && retry++ < 0x00ffffff);       //wait for start block token 
        if (retry > 0x00ffffff) {
            SD_CS_DEASSERT;
            return 1;
        }        //return error if time-out
        max = start + 512;
        for (i = start; i < max; i++) //read 512 bytes in to buffer
        {
            img[i] = SPI_read();
        }

        SPI_read();                    //receive incoming CRC (16-bit), evenif CRC is ignored
        SPI_read();

        SPI_read();                    //extra 8 clock pulses
    }
    SD_CS_DEASSERT;                //deselect card
    return 0;                      //512 bytes read successfully
}
