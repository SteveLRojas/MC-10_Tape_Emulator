/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "UDisk_HW.h"
#include "usbh_msc_bot.h"
#include "usbh_msc_scsi.h"

#define LUN 0

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv		/* Physical drive nmuber to identify the drive */)
{
	(void)pdrv;
	USBH_MSC_SCSI_TestUnitReady(LUN);
	return hbot.csw.field.Status != 0;
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv				/* Physical drive nmuber to identify the drive */)
{
	uint8_t status = 0;
	(void)pdrv;

	if(USBOTG_H_FS->MIS_ST & USBFS_UMS_DEV_ATTACH)
	{
		status = Udisk_USBH_EnumRootDevice(0);
		status |= USBH_MSC_BOT_REQ_Reset();
		Delay_Ms(10);
		USBH_MSC_BOT_Init();
		if(status == ERR_SUCCESS)
			return RES_OK;
	}
	return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	uint8_t status;
	status = USBH_MSC_SCSI_Read(0, sector, buff, count);
	return status == ERR_SUCCESS ? RES_OK : RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	uint8_t status;
	status = USBH_MSC_SCSI_Write(0, sector, (uint8_t*)buff, count);
	return status == ERR_SUCCESS ? RES_OK : RES_ERROR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;
	SCSI_CapacityTypeDef size_info;

	switch (cmd)
	{
		case CTRL_SYNC:
			res = RES_OK;
			break;
		case GET_SECTOR_COUNT:
			if(USBH_MSC_SCSI_ReadCapacity(0, &size_info) == ERR_SUCCESS)
			{
				*((LBA_t*)buff) = (size_info.block_nbr + 1);
				res = RES_OK;
			}
			else
			{
				res = RES_ERROR;
			}
			break;
		case GET_SECTOR_SIZE:
			if(USBH_MSC_SCSI_ReadCapacity(0, &size_info) == ERR_SUCCESS)
			{
				*((LBA_t*)buff) = (size_info.block_size);
				res = RES_OK;
			}
			else
			{
				res = RES_ERROR;
			}
			break;
		case GET_BLOCK_SIZE:
			*((DWORD*)buff) = 1;
			res = RES_OK;
			break;
		case CTRL_TRIM:
			break;
		default:
			break;
	}

	return res;
}

