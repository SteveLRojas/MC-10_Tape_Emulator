/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_prop.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : All processing related to Virtual COM Port Demo (Endpoint 0) 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __usb_prop_H
#define __usb_prop_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v20x.h"

#define CDC_GET_LINE_CODING         0x21                                      /* This request allows the host to find out the currently configured line coding */
#define CDC_SET_LINE_CODING         0x20                                      /* Configures DTE rate, stop-bits, parity, and number-of-character */
#define CDC_SET_LINE_CTLSTE         0x22                                      /* This request generates RS-232/V.24 style control signals */
#define CDC_SEND_BREAK              0x23      


#define USBD_GetConfiguration          NOP_Process
// #define USBD_SetConfiguration          NOP_Process
#define USBD_GetInterface              NOP_Process
#define USBD_SetInterface              NOP_Process
#define USBD_GetStatus                 NOP_Process
// #define USBD_ClearFeature              NOP_Process
#define USBD_SetEndPointFeature        NOP_Process
// #define USBD_SetDeviceFeature          NOP_Process
// #define USBD_SetDeviceAddress          NOP_Process



void USBD_init(void);
void USBD_Reset(void);
void USBD_SetConfiguration(void);
void USBD_SetDeviceAddress (void);
void USBD_SetDeviceFeature (void);
void USBD_ClearFeature (void);
void USBD_Status_In (void);
void USBD_Status_Out (void);
RESULT USBD_Data_Setup(uint8_t);
RESULT USBD_NoData_Setup(uint8_t);
RESULT USBD_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *USBD_GetDeviceDescriptor(uint16_t );
uint8_t *USBD_GetConfigDescriptor(uint16_t);
uint8_t *USBD_GetStringDescriptor(uint16_t);
uint8_t USBD_ENDPx_DataUp( uint8_t endp, uint16_t len );
#ifdef __cplusplus
}
#endif

#endif /* __usb_prop_H */







