#include "debug.h"
#include "stdio.h"
#include "string.h"
#include "UDisk_HW.h"
#include "usbh_msc_bot.h"
#include "usbh_msc_scsi.h"



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
    	//uint8_t set_feature_status = USBH_SetFeature(USB_REQ_FEAT_REMOTE_WAKEUP, 0x00);
    	//printf("set feature status: %02X\n", set_feature_status);

    	uint8_t bot_reset_status = USBH_MSC_BOT_REQ_Reset();
    	printf("BOT reset status: %02X\n", bot_reset_status);
    	Delay_Ms(10);

    	uint8_t max_lun;
    	USBH_MSC_BOT_REQ_GetMaxLUN(&max_lun);
    	printf("Max LUN is: %02x\n", max_lun);

    	USBH_MSC_BOT_Init();
    	printf("Starting USBH_MSC_SCSI_TestUnitReady!\n");
    	USBH_MSC_SCSI_TestUnitReady(0);
    	printf("Finished USBH_MSC_SCSI_TestUnitReady!\n");

    	printf("\n---CBW---\n");
    	printf("Signature............[%08X]\n", hbot.cbw.field.Signature);
    	printf("Tag..................[%08X]\n", hbot.cbw.field.Tag);
    	printf("DataTransferLength...[%08X]\n", hbot.cbw.field.DataTransferLength);
    	printf("Flags................[%02X]\n", hbot.cbw.field.Flags);
    	printf("LUN..................[%02X]\n", hbot.cbw.field.LUN);
    	printf("CBLength.............[%02X]\n", hbot.cbw.field.CBLength);
    	printf("CB[0]................[%02X]\n", hbot.cbw.field.CB[0]);

    	printf("\n---CSW---\n");
    	printf("Signature............[%08X]\n", hbot.csw.field.Signature);
    	printf("Tag..................[%08X]\n", hbot.csw.field.Tag);
    	printf("Data Residue.........[%08X]\n", hbot.csw.field.DataResidue);
    	printf("Status...............[%02X]\n", hbot.csw.field.Status);

        printf("\nDone.\n\n");
    	Delay_Ms(10000);
    }
}

