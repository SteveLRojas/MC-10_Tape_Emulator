/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
   1,Exam 1 :
   USBFS host controller enumeration routines for byte manipulation of USB flash drives,
 including file creation, modification, reading and deletion.
   2,Exam 6 :
   USBFS host controller enumeration routines for sector manipulation of USB flash drives,
 including file creation, modification, reading and deletion
   3,Exam 9 :
   The USBFS host controller enumerates the USB flash drive to create folders on the drive,
 including the primary and secondary directories, and the demo files in the corresponding directories.
   4,Exam 11 :
   USBFS host controller enumerates USB flash drives, enabling the enumeration of
 the first 1000 files on the drive.
   5,Exam 13 :USBFS host controller enumerates USB drives to create long filename files,
 or to get long filename files.
  Important: Only FAT12/FAT16/FAT32 formats are supported:
*/

#include "debug.h"
#include "stdio.h"
#include "string.h"
#include "Udisk_Operation.h"
#include "usbh_msc_bot.h"


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    USART_Printf_Init( 115200 );
    Delay_Init( );
    printf( "SystemClk:%d\r\n", SystemCoreClock );
    printf("booba\n");

    /* General USB Host UDisk Operation Initialization */
    Udisk_USBH_Initialization( );

    while(1)
    {
    	Udisk_USBH_EnumRootDevice(0);

    	USBH_MSC_BOT_REQ_Reset();
    	Delay_Ms(10);
    	uint8_t max_lun;
    	USBH_MSC_BOT_REQ_GetMaxLUN(&max_lun);
    	printf("Max LUN is: %02x\n", max_lun);

        printf("\nDone.\n\n");
    	Delay_Ms(10000);
    }
}

