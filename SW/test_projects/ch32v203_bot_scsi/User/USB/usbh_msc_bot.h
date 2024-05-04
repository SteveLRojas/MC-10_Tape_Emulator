#ifndef __USBH_MSC_BOT_H
#define __USBH_MSC_BOT_H


#include "usbh_def.h"

#define BOT_CBW_SIGNATURE            0x43425355U
#define BOT_CBW_TAG                  0x20304050U
#define BOT_CSW_SIGNATURE            0x53425355U
#define BOT_CBW_LENGTH               31U
#define BOT_CSW_LENGTH               13U


#define BOT_SEND_CSW_DISABLE         0U
#define BOT_SEND_CSW_ENABLE          1U

#define BOT_DIR_IN                   0U
#define BOT_DIR_OUT                  1U
#define BOT_DIR_BOTH                 2U

#define BOT_PAGE_LENGTH              512U


#define BOT_CBW_CB_LENGTH            16U


#define MAX_BULK_STALL_COUNT_LIMIT       0x04U   /* If STALL is seen on Bulk
                                         Endpoint continuously, this means
                                         that device and Host has phase error
                                         Hence a Reset is needed */

typedef enum
{
  BOT_OK          = 0,
  BOT_FAIL        = 1,
  BOT_PHASE_ERROR = 2,
  BOT_BUSY        = 3
}
BOT_StatusTypeDef;

typedef enum
{
  BOT_CMD_IDLE  = 0,
  BOT_CMD_SEND,
  BOT_CMD_WAIT,
}
BOT_CMDStateTypeDef;

/* CSW Status Definitions */
typedef enum
{

  BOT_CSW_CMD_PASSED   =        0x00,
  BOT_CSW_CMD_FAILED   =        0x01,
  BOT_CSW_PHASE_ERROR  =        0x02,
}
BOT_CSWStatusTypeDef;

typedef enum
{
  BOT_SEND_CBW  = 1,
  BOT_SEND_CBW_WAIT,
  BOT_DATA_IN,
  BOT_DATA_IN_WAIT,
  BOT_DATA_OUT,
  BOT_DATA_OUT_WAIT,
  BOT_RECEIVE_CSW,
  BOT_RECEIVE_CSW_WAIT,
  BOT_ERROR_IN,
  BOT_ERROR_OUT,
  BOT_UNRECOVERED_ERROR
}
BOT_StateTypeDef;

typedef union
{
  struct __CBW
  {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataTransferLength;
    uint8_t  Flags;
    uint8_t  LUN;
    uint8_t  CBLength;
    uint8_t  CB[16];
  } field;
  uint8_t data[31];
}
BOT_CBWTypeDef;

typedef union
{
  struct __CSW
  {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataResidue;
    uint8_t  Status;
  } field;
  uint8_t data[13];
}
BOT_CSWTypeDef;

typedef struct
{
  uint32_t                   data[16];
  BOT_StateTypeDef           state;
  BOT_StateTypeDef           prev_state;
  BOT_CMDStateTypeDef        cmd_state;
  BOT_CBWTypeDef             cbw;
  uint8_t                    Reserved1;
  BOT_CSWTypeDef             csw;
  uint8_t                    Reserved2[3];
  uint8_t                    *pbuf;
}
BOT_HandleTypeDef;


extern BOT_HandleTypeDef hbot;
extern uint8_t endp_addr_out;
extern uint8_t endp_addr_in;
extern uint8_t endp_tog_out;
extern uint8_t endp_tog_in;


uint8_t USBH_MSC_BOT_REQ_Reset();
uint8_t USBH_MSC_BOT_REQ_GetMaxLUN(uint8_t* Maxlun);
USBH_StatusTypeDef USBH_MSC_BOT_Init();
uint8_t USBH_MSC_BOT_Command(uint8_t lun);

//USBH_StatusTypeDef USBH_MSC_BOT_Process(USBH_HandleTypeDef *phost, uint8_t lun);
//USBH_StatusTypeDef USBH_MSC_BOT_Error(USBH_HandleTypeDef *phost, uint8_t lun);

#endif  /* __USBH_MSC_BOT_H__ */

