/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 **************************************************************************//*!
 *
 * @file disk.c
 *
 * @author 
 *
 * @version 
 *
 * @date May-08-2009
 *
 * @brief  RAM Disk has been emulated via this Mass Storage Demo
 *****************************************************************************/
 
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "types.h"          /* User Defined Data Types */
#include "hidef.h"          /* for EnableInterrupts macro */
#include <string.h>
#include <stdlib.h>         /* ANSI memory controls */
#include "stdio.h"
#include "derivative.h"     /* include peripheral declarations */
#include "usb_msc.h"        /* USB MSC Class Header File */
#include "disk.h"           /* Disk Application Header File */
#include "usb_class.h"
#include "FAT16.h"
#include "Bootloader.h"
#include "Boot_loader_task.h"
#ifdef __MCF52xxx_H__
#include "Wdt_cfv2.h"
#include "flash_cfv2.h"
#endif 
#ifdef _MCF51JM128_H
#include "flash.h"
#endif
#ifdef MCU_MK60N512VMD100
#include "flash_FTFL.h"
#endif

#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
#include "exceptions.h"
#endif

/* skip the inclusion in dependency statge */
#ifndef __NO_SETJMP
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "aes.h"

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void            TestApp_Init(void);
extern void     Watchdog_Reset(void);
extern uint_8   filetype;             /* image file type */  
uint_32         file_size;            /* bytes */ 
uint_8          new_file;  
uint_8          error=FLASH_IMAGE_SUCCESS;
boolean boot_complete = FALSE; 
/****************************************************************************
 * Global Variables
 ****************************************************************************/ 
 /* Add all the variables needed for disk.c to this structure */
DISK_GLOBAL_VARIABLE_STRUCT g_disk;
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint_8 controller_ID,  uint_8 event_type, void* val);
void MSD_Event_Callback(uint_8 controller_ID, uint_8 event_type, void* val);
void Disk_App(void);
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/******************************************************************************
 * 
 *    @name       Disk_App
 *    
 *    @brief      
 *                  
 *    @param      None
 * 
 *    @return     None
 *    
 *****************************************************************************/

void Disk_App(void)
{
    /* Body */
    uint_32 i = 0;
    
    if(TRUE== boot_complete)
    {
        /* De-Init MSD device */
        USB_Class_MSC_DeInit(USB_CONTROLLER_ID);
        
        /* if bootloader fail, erase memory */
        if(BootloaderSuccess != BootloaderStatus)
        {
            erase_flash();
        } /* EndIf */
        
        boot_complete = FALSE;
        for(i=0;i<1000;i++);
        
        /* Re-enumerate MSD device */
        USB_Class_MSC_Init(g_disk.app_controller_ID,
        	USB_App_Callback,NULL, MSD_Event_Callback);
    } /* EndIf */
    return;
} /* EndBody */

/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       controller_ID : To Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Callback
    (
        uint_8 controller_ID, 
        uint_8 event_type, 
        void* val
    )
{
    /* Body */
    UNUSED (controller_ID)
    UNUSED (val)    
    if(event_type == USB_APP_BUS_RESET) 
    {
        g_disk.start_app=FALSE;    
    }
    else
    {
        if(event_type == USB_APP_ENUM_COMPLETE) 
        {
            g_disk.start_app=TRUE;
        }
        else
        {
            if(event_type == USB_APP_ERROR)
            {
                /* add user code for error handling */
            } /* EndIf */
        } /* EndIf */
    } /* EndIf */
    return;
} /* EndBody */

/******************************************************************************
 * 
 *    @name        MSD_Event_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       controller_ID : To Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void MSD_Event_Callback
    (
        uint_8 controller_ID, 
        uint_8 event_type, 
        void* val
    )
{
    /* Body */
    PTR_LBA_APP_STRUCT lba_data_ptr;
    uint_32 i;
    uint_8_ptr prevent_removal_ptr, load_eject_start_ptr;
    PTR_DEVICE_LBA_INFO_STRUCT device_lba_info_ptr;
    UNUSED (controller_ID)
    switch(event_type)
    {
        case USB_APP_DATA_RECEIVED :
            break;
        case USB_APP_SEND_COMPLETE :
            break;
        case USB_MSC_START_STOP_EJECT_MEDIA :
            load_eject_start_ptr = (uint_8_ptr)val;
            /* Code to be added by user for starting, stopping or 
            ejecting the disk drive. e.g. starting/stopping the motor in 
            case of CD/DVD*/
            break;
        case USB_MSC_DEVICE_READ_REQUEST :
            /* copy data from storage device before sending it on USB Bus 
            (Called before calling send_data on BULK IN endpoints)*/
            lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
            /* read data from mass storage device to driver buffer */
            FATReadLBA(lba_data_ptr->offset>>9,lba_data_ptr->buff_ptr);
            break;
        case USB_MSC_DEVICE_WRITE_REQUEST :
            /* copy data from USb buffer to Storage device 
            (Called before after recv_data on BULK OUT endpoints)*/
            lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
            if((lba_data_ptr->offset>>9)== FATTable0Sec0) 
            {   
                /* write new file */
                new_file = TRUE;
            } /* EndIf */
            if(((lba_data_ptr->offset>>9)== FATRootDirSec0)&&(new_file == TRUE)) 
            {
                /* read file size of the file was received */
                /* search for the file entry was received */
                /* each file entry contain 32 bytes */ 
                for(i = lba_data_ptr->size -32; i>0; i -=32) 
                {
                    if(*(lba_data_ptr->buff_ptr +i) != 0)
                    {
                        break;
                    } /* EndIf */
                } /* Endfor */
                /* the file size field is offet 28 of file entry */
#if (defined MCU_MK60N512VMD100)
                file_size = *(uint_32*)( lba_data_ptr->buff_ptr + i + 28);
#else
                file_size = BYTESWAP32(*(uint_32*)( lba_data_ptr->buff_ptr + i + 28));
#endif
                new_file = FALSE;
            } /* EndIf */
            if((lba_data_ptr->offset>>9)== FATDataSec0) 
            {
                erase_flash();
                filetype = UNKNOWN; 
                printf("\n\tOpen Image File");
            } /* EndIf */
            if((lba_data_ptr->offset>>9)>= FATDataSec0) 
            {
                SetOutput(BSP_LED3, TRUE);
                /* parse and flash an array to flash memory */
                if (FLASH_IMAGE_SUCCESS == error)
                {
                   	error = aes_main(lba_data_ptr->buff_ptr,&(lba_data_ptr->size));
                   	if (!error)
                   		error = FlashApplication(lba_data_ptr->buff_ptr,lba_data_ptr->size);
                }
                SetOutput(BSP_LED3, FALSE);
            } /* EndIf */
            /* rest of file */ 
            if(((lba_data_ptr->offset>>9) - FATDataSec0) == ((file_size -1)/512))
            {
                boot_complete=TRUE;     //tranfer file done
                
                if(BootloaderStatus == BootloaderReady)
                {
                    BootloaderStatus = BootloaderSuccess;
                }
                
                /* print flashing status */
                if(BootloaderStatus != BootloaderSuccess) 
                {
                    printf("\nFlash image file fail!");
                } else 
                {
                     printf("\nFlash image file complete!");
                     printf("\nPress RESET button to enter application mode.");
                } /* EndIf */
            } /* EndIf */
            break;
        case USB_MSC_DEVICE_FORMAT_COMPLETE :
            break;
        case USB_MSC_DEVICE_REMOVAL_REQUEST :
            prevent_removal_ptr = (uint_8_ptr) val;
            if(SUPPORT_DISK_LOCKING_MECHANISM)
            {
                g_disk.disk_lock = *prevent_removal_ptr;
            }
            else 
            {
                if((!SUPPORT_DISK_LOCKING_MECHANISM)&&(!(*prevent_removal_ptr)))
                {
                    /*there is no support for disk locking and removal of medium is enabled*/
                    /* code to be added here for this condition, if required */ 
                } /* EndIf */
            } /* EndIf */
            break;
        case USB_MSC_DEVICE_GET_INFO :
            device_lba_info_ptr = (PTR_DEVICE_LBA_INFO_STRUCT)val;
            device_lba_info_ptr->total_lba_device_supports = TOTAL_LOGICAL_ADDRESS_BLOCKS;
            device_lba_info_ptr->length_of_each_lab_of_device = LENGTH_OF_EACH_LAB; 
            device_lba_info_ptr->num_lun_supported = LOGICAL_UNIT_SUPPORTED;
            break;
        default : 
            break;
    } /* EndSwitch */
    return;
} /* EndBody */

 /******************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       This function is the entry for mouse (or other usuage)
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
 
void TestApp_Init(void)
{
    /* Body */
    uint_8   error;
    Flash_Init(59);
    /* initialize the Global Variable Structure */
    USB_memzero(&g_disk, sizeof(DISK_GLOBAL_VARIABLE_STRUCT));
    g_disk.app_controller_ID = USB_CONTROLLER_ID;
    DisableInterrupts;
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_dis();
#endif
    /* Initialize the USB interface */
    error = USB_Class_MSC_Init(g_disk.app_controller_ID,
    USB_App_Callback,NULL, MSD_Event_Callback);
    EnableInterrupts;
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_en();
#endif
} /* EndBody */


/******************************************************************************
 *
 *   @name        TestApp_Task
 *
 *   @brief       Application task function. It is called from the main loop
 *
 *   @param       None
 *
 *   @return      None
 *
 *****************************************************************************
 * Application task function. It is called from the main loop
 *****************************************************************************/
void TestApp_Task(void)
{
    /* Body */
    /* call the periodic task function */      
    USB_MSC_Periodic_Task(); 
    /*check whether enumeration is complete or not */
    if(g_disk.start_app==TRUE)
    {
        Disk_App(); 
    } /* EndIf */
} /* EndBody */

/* EOF */
