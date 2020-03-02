/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: main.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains device driver for mass storage class. This code tests
*   the FAT functionalities.
*
*END************************************************************************/

/**************************************************************************
Include the OS and BSP dependent files that define IO functions and
basic types. You may like to change these files for your board and RTOS 
**************************************************************************/
/**************************************************************************
Include the USB stack header files.
**************************************************************************/
#include "aes.h"
#include "derivative.h"
#include "hidef.h"
#include "usb_bsp.h"
#include "sci.h"
#include "rtc.h"
#include "usb_host_hub_sm.h"
#include "diskio.h"
#include "Bootloader.h"
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
#include "exceptions.h"
#endif
#include "poll.h"
#if (defined __MCF52259_H__)
#include "flash_cfv2.h"
#elif (defined _MCF51JM128_H)
#include "flash.h"
#elif (defined MCU_MK60N512VMD100)
#include "flash_FTFL.h"
#endif
#include "ff.h"
//#include "usb_host_msd_ufi.h"

 /* USB device states */
#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

/**************************************************************************
   Global variables
**************************************************************************/
extern volatile DEVICE_STRUCT       mass_device;   /* mass storage device struct */  
extern uint_8                       filetype;      /* image file type */     
static uint_32                      New_sp,New_pc; /* stack pointer and program counter */
/**************************************************************************
   Funciton prototypes
**************************************************************************/
extern int      _startup    (void);
void            Main_Task   (void);
void            BootLoader  (void);
//void            load_image  (void);
uint_8          erase_flash (void);
static void     Switch_mode (void); 
static FRESULT  put_rc      (FRESULT rc);
void            usb_host_mass_device_event (_usb_device_instance_handle,_usb_interface_descriptor_handle,uint_32);
void            GPIO_Init   (void);
void SetOutput(uint_32 output,boolean state);

const USB_HOST_DRIVER_INFO DriverInfoTable[] = {
   /* Floppy drive */
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_UFI,        /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_mass_device_event    /* Application call back function   */
   },

   /* USB 2.0 hard drive */
   {
      {0x49,0x0D},                  /* Vendor ID per USB-IF             */
      {0x00,0x30},                  /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_SCSI,       /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_mass_device_event    /* Application call back function   */
   },

   /* USB 1.1 hub */
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HUB,                /* Class code                       */
      USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
      USB_PROTOCOL_HUB_LS,          /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hub_device_event     /* Application call back function   */
   },

   {
      {0x00,0x00},                  /* All-zero entry terminates        */
      {0x00,0x00},                  /*    driver info list.             */
      0,
      0,
      0,
      0,
      NULL
   }

};

/* Protect bootloader area (sector 0 to sector 3)*/
#if defined(_MCF51JM128_H)
#ifdef __DEBUG__
	const byte NVPROT_INIT @0x0000040D  = 0xBB;    // 0x00000000-0x00001FFF are protected 
	const byte NVOPT_INIT  @0x0000040F  = 0x00;    // flash unsecure    
#else
	const byte NVPROT_INIT @0x0000040D  = 0xCB;    // 0x00000000-0x00001FFF are protected 
	const byte NVOPT_INIT  @0x0000040F  = 0x00;    // flash unsecure    
#endif
#endif /* End __MCF52259_H__ */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
void main() 
{
    /* Body */
    USB_STATUS           status = USB_OK;
    _usb_host_handle     host_handle;
    uint_32 i;
    /* Initialize the current platform. Call for the _bsp_platform_init which is specific to each processor family */
    _bsp_platform_init();
    GPIO_Init();
    
    /* Protect bootloader area (sector 0 to sector 3)*/
#ifdef  __MCF52259_H__ 
	#ifdef __DEBUG__
		MCF_CFM_CFMPROT = 0x0F;
	#else
		MCF_CFM_CFMPROT = 0x07;
	#endif
#elif defined(MCU_MK60N512VMD100)
    FTFL_FPROT3 = 0xF0;    
#endif /* End __MCF52259_H__ */

#ifdef __DEBUG__
    sci1_init();
#endif
    
//    for(i=0;i<=2000;i++);
    Switch_mode();                /* check for bootloader mode */ 
    TimerInit();
    POLL_init();                  /* Init polling global variable */
    
    SetOutput(BSP_LED1, FALSE);
    SetOutput(BSP_LED2, FALSE);
    
    DisableInterrupts; 
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_dis();
#endif  
    status = _usb_host_init
             (HOST_CONTROLLER_NUMBER,    /* Use value in header file */
             MAX_FRAME_SIZE,             /* Frame size per USB spec  */
             &host_handle);              /* Returned pointer */

    if (status != USB_OK) {
#ifdef __DEBUG__
        printf("\nUSB Host Initialization failed. STATUS: %x", status);
#endif
    }

    /*
    ** Since we are going to act as the host driver, register
    ** the driver information for wanted class/subclass/protocols
    */
    status = _usb_host_driver_info_register(host_handle, (void*)DriverInfoTable);
    if (status != USB_OK) {
#ifdef __DEBUG__
        printf("\nDriver Registration failed. STATUS: %x", status);
#endif
    } 
    EnableInterrupts;
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_en();
#endif
    /* Run bootloader mode */
#ifdef __DEBUG__
    printf("\n****************************************************************************\n\r");
    printf("*                              Bootloader mode                             *\n\r");
    printf("****************************************************************************\n\r"); 
    printf("\nUSB stick boot loader\n\rWaiting for USB mass storage to be attached...\n\r");
#endif
    for(;;) {
        Main_Task();
        Poll();
        __RESET_WATCHDOG(); /* feeds the dog */
    } /* loop forever */
    /* please make sure that you never leave main */
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : GPIO_Init
* Returned Value : none
* Comments       :
*     Init LEDs and Buttons
*
*END*--------------------------------------------------------------------*/
void GPIO_Init()
{
    /* Body */
#if defined(MCU_MK60N512VMD100)
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;      /* Enable clock gating to PORTA */
	
    /* Enable LEDs Port A pin 28 & 29 */
    PORTA_PCR28|= PORT_PCR_SRE_MASK         /* Slow slew rate */
              |  PORT_PCR_ODE_MASK          /* Open Drain Enable */
              |  PORT_PCR_DSE_MASK          /* High drive strength */
              ;
    PORTA_PCR28 = PORT_PCR_MUX(1);

    PORTA_PCR29|= PORT_PCR_SRE_MASK         /* Slow slew rate */
              |  PORT_PCR_ODE_MASK          /* Open Drain Enable */
              |  PORT_PCR_DSE_MASK          /* High drive strength */
              ;
    PORTA_PCR29 = PORT_PCR_MUX(1);

    GPIOA_PSOR |= 1 << 28 | 1 << 29;
    GPIOA_PDDR |= 1 << 28 | 1 << 29;

    /* set in put PORTA pin 19 */
    PORTA_PCR19 =  PORT_PCR_MUX(1);
    GPIOA_PDDR &= ~((uint_32)1 << 19);

    PORTA_PCR19 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;   /* pull up*/
#endif /* End MCU_MK60N512VMD100 */

#ifdef _MCF51JM128_H
    /* init buttons on port G */
    PTGDD &= 0xF0;                    /* set PTG0-3 to input */
    PTGPE |= 0x0F;                    /* enable PTG0-3 pull-up resistor */

    /* Enable LEDs Port E pin 2 & 3 */
    PTEDD_PTEDD2 = 1;
    PTEDD_PTEDD3 = 1;
    PTED_PTED2   = 1;
    PTED_PTED3   = 1;
#endif /* End _MCF51JM128_H */

#if (defined __MCF52259_H__)
	MCF_GPIO_DDRTC = MCF_GPIO_DDRTC_DDRTC0 | MCF_GPIO_DDRTC_DDRTC1;
	/* Assign PORTC[0-3] as GPIO */
	MCF_GPIO_PTCPAR = MCF_GPIO_PTCPAR_PTCPAR0(MCF_GPIO_PTCPAR_DTIN0_GPIO) |
	                  MCF_GPIO_PTCPAR_PTCPAR3(MCF_GPIO_PTCPAR_DTIN1_GPIO);
#endif	                  
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Switch_mode
* Returned Value : none
* Comments       : Jump between application and bootloader
*     
*
*END*--------------------------------------------------------------------*/
static void Switch_mode(void)
{
    /* Body */
	uint_32 i;
	volatile uint_32  temp = 1;   /* default the button is not pressed */

    /* Get PC and SP of application region */
    New_sp  = IMAGE_ADDR[0];
    New_pc  = IMAGE_ADDR[1];

    /* Check switch is pressed*/

    for (i=0; i<2000; i++)
    {    	
#ifdef  __MCF52259_H__ 
    	temp =(uint_8) ((1<<5) & *(uint_8 *)0x40100044);      /* DES READ SW1 of M52259EVB */
#elif defined(MCU_MK60N512VMD100)
    	temp =(uint_32) ((1<<19) & GPIOA_PDIR);               /* DES READ SW1 of TWK60 */
#elif defined(_MCF51JM128_H)
    	temp =(uint_32) ((1<<1) & PTGD);                      /* DES READ SW1 of JM128EVB */
#endif /* End __MCF52259_H__ */
    if (temp)
    	break;
    }
    
    if(temp)
    {
        if((New_sp != 0xffffffff)&&(New_pc != 0xffffffff))
        {
#ifdef __DEBUG__
            printf("\nUser application is running...\n"); /* Run the application */
#endif
#if (!defined _MK_xxx_H_)
            asm
            {
                move.w   #0x2700,sr
                move.l   New_sp,a0
                move.l   New_pc,a1
                move.l   a0,a7
                jmp     (a1)
            }
#elif defined(__CWCC__)
            asm
            {
                ldr   r4,=New_sp
                ldr   sp, [r4]
                ldr   r4,=New_pc
                ldr   r5, [r4]
                blx   r5
            }
#elif defined(__IAR_SYSTEMS_ICC__)
            asm("mov32   r4,New_sp");
            asm("ldr     sp,[r4]");
            asm("mov32   r4,New_pc");
            asm("ldr     r5, [r4]");
            asm("blx     r5");
#endif /* end (!defined _MK_xxx_H_) */
        } /* EndIf */
    } /* EndIf */
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Main_Task
* Returned Value : none
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
void Main_Task ( void )
{ 
    /* Body */
   static uint_8 bootloader_task = 0;

    /*----------------------------------------------------**
    ** Infinite loop, waiting for events requiring action **
    **----------------------------------------------------*/
    switch ( mass_device.dev_state ) {
        case USB_DEVICE_IDLE:
            break ;
        case USB_DEVICE_ATTACHED:
#ifdef __DEBUG__
            printf( "Mass Storage Device Attached\n\r" );
#endif
            mass_device.dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
            _usb_hostdev_select_interface(mass_device.dev_handle,
                 mass_device.intf_handle, (pointer)&mass_device.class_intf);
             /* Can run bootloader_task task */
             bootloader_task = 1;   
            break ;
        case USB_DEVICE_SET_INTERFACE_STARTED:
            break;
        case USB_DEVICE_INTERFACED:
            if(1 == bootloader_task)
            {
                /* USB attached, call bootloader function */
            	BootLoader();
            }
            /* Disable flag to run bootloader_task */
            bootloader_task = 0;
            break ;
         case USB_DEVICE_DETACHED:
#ifdef __DEBUG__
            printf( "\nMass Storage Device Detached\n\r" );
#endif
            mass_device.dev_state = USB_DEVICE_IDLE;
            break;
         case USB_DEVICE_OTHER:
            break ;
         default:
#ifdef __DEBUG__
            printf( "Unknown Mass Storage Device State = %d\n\r",\
               mass_device.dev_state );
#endif
            break ;
      } /* Endswitch */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : erase_flash
* Returned Value : none
* Comments       : erase flash memory in application area
*     
*
*END*--------------------------------------------------------------------*/
uint_8 erase_flash(void)
{ 
    /* Body */
    uint_8 error = FALSE;
    uint_8 i;
#ifdef __DEBUG__
    printf("\n\nErasing flash memory...\n\r");
#endif
    DisableInterrupts;                      
    for (i=0;i<(MAX_FLASH1_ADDRESS -(uint_32) IMAGE_ADDR)/ERASE_SECTOR_SIZE;i++)
    {
#if (!defined _MK_xxx_H_)    
        error = Flash_SectorErase((uint_32*)((uint_32) IMAGE_ADDR + ERASE_SECTOR_SIZE*i)) ; /* ERASE 4k flash memory */
#else
        error = Flash_SectorErase((uint_32) IMAGE_ADDR + ERASE_SECTOR_SIZE*i) ; /* ERASE 4k flash memory */
#endif
        if(error != Flash_OK)
        {
#ifdef __DEBUG__
            printf("\nErase flash error!\n\r");
#endif
            break;
        } /* Endif */
#ifdef __DEBUG__
        printf("#");
#endif
    } /* Endfor */
    EnableInterrupts;
#ifdef __DEBUG__
    printf("\n\rERASE complete!\n\r");
#endif
    return error;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : put_rc
* Returned Value : FRESULT
* Comments       : describe errors in file tasks
*     
*
*END*--------------------------------------------------------------------*/
static FRESULT put_rc 
    (
        /* [IN] return code value*/
        FRESULT rc
    )
{
    /* Body */
    const TCHAR *str =
        "OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
        "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
        "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
        "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
    FRESULT i;

    for (i = FR_OK; i != rc && *str; i++) 
    {
        while (*str++) ;
    } /* Endfor */
#ifdef __DEBUG__
    printf("  returnCode=%u (FR_%s)\n\r", (uint_32)rc, str);
#endif
    return rc;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : SetOutput
* Returned Value : None
* Comments       : Set out put of the LEDs
*     
*END*--------------------------------------------------------------------*/
void SetOutput
    (
        /* [IN] the output pin */
        uint_32 output,
        /* [IN] the state to set */
        boolean state
    )
{
    /* Body */
    /* For 52259EVB */
#if (defined __MCF52259_H__)
    if(state)
        MCF_GPIO_SETTC |= output; 
    else
        MCF_GPIO_CLRTC = ~output; 
#endif

/* For TWR-K60 */
#if (defined MCU_MK60N512VMD100)
    if(state)
        GPIOA_PCOR |= output; 
    else
        GPIOA_PSOR |= output; 
#endif

/* For 51JM128EVB */
#if (defined _MCF51JM128_H)
    if(state)
        PTED &=~ output; 
    else
        PTED |= output; 
#endif
} /* Endbody */



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : load_image
* Returned Value : none
* Comments       : load the image file to application area
*
*END*--------------------------------------------------------------------*/


void load_image(void)
{
    /* Body */
    static uint_8	result;
    FRESULT returnCode;                 /* Result code */
    FATFS   fatfs;                      /* File system object */
    FIL     fp;                         /* File pointer */
    uint_32  size;
    uint_8  buffer[BUFFER_LENGTH];      /* Buffer for parse */
    returnCode = f_mount(0, &fatfs);
    if(returnCode)
    {
        put_rc(returnCode);
        return;
    } /* EndIf */

 
    disk_initialize(0);
    disk_ioctl(0, UFI_INQUIRY_CMD, NULL);
    disk_ioctl(0, UFI_READ_FORMAT_CAPACITY_CMD, NULL);
    disk_ioctl(0, REQUEST_SENSE_CMD, NULL);
//    disk_ioctl(0, UFI_READ_FORMAT_CAPACITY_CMD, NULL);
   
    
    
#ifdef __DEBUG__
    printf("\n\tSearch Image File ...\n\r");
#endif
    returnCode = f_open(&fp,"image.bin",FA_READ);
    if (returnCode) 
    {
        returnCode = f_open(&fp,"image.S19",FA_READ);
    } /* EndIf */
    
    if (returnCode) 
    {
        return;
    } /* EndIf */
    else 
    {
#ifdef __DEBUG__
    	printf("Image file found.\n\r");
    	printf("File size = %4d\n\r",fp.fsize);	
#endif
    	SetOutput(BSP_LED2, FALSE);
    	erase_flash();
    	SetOutput(BSP_LED2, TRUE);
    }
    filetype = UNKNOWN;                 /* reset the filetype variable */
    while(TRUE)
    {
        /* Get BUFFER_LENGTH bytes and parse for programming */
        returnCode = f_read(&fp, buffer,BUFFER_LENGTH,&size); 
        if(size == 0) 
            break;                          /* if run out of data, break the loop */
        if(returnCode)
        {
            put_rc(returnCode);
            return;
        } /* Endif */
        SetOutput(BSP_LED2, FALSE);

//        printf("test\n");
        result = aes_main(buffer,&size);
        if (result==0)
        	result = FlashApplication(buffer,size);  /* parse and flash an array to flash memory */
        SetOutput(BSP_LED2, TRUE);
        if(result != 0) 
        {
#ifdef __DEBUG__
        	printf("\n\tFlash image fail! Re-erase flash memory\r");
#endif
        	erase_flash();
        	return;
        }
        	
    } /* Endwhile */
    SetOutput(BSP_LED1, TRUE);
#ifdef __DEBUG__
    printf("\n\r\tFlash image file complete!\r");
#endif
} /* EndBody */





/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : BootLoader
* Returned Value : none
* Comments       : Bootloader tasks.
*     
*
*END*--------------------------------------------------------------------*/
void BootLoader(void)
{ 
    /* Body */
    USB_STATUS           error = FALSE;
#if (!defined _MK_xxx_H_) 
    Flash_Init(59);
#endif
    if(error == Flash_OK)
    {
#ifdef __DEBUG__
        printf("\nAttemp Flashing Image\n\r");
#endif
        /* Send some SCSI requests first */
       
        load_image();
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_device_event
* Returned Value : None
* Comments       : Called when mass storage device has been attached, detached, etc.
*
*END*--------------------------------------------------------------------*/
void usb_host_mass_device_event
    (
        /* [IN] pointer to device instance */
        _usb_device_instance_handle      dev_handle,

        /* [IN] pointer to interface descriptor */
        _usb_interface_descriptor_handle intf_handle,

        /* [IN] code number for event causing callback */
        uint_32           event_code
    )
{
    /* Body */
    switch (event_code) {
        case USB_CONFIG_EVENT:
         /* Drop through into attach, same processing */
        case USB_ATTACH_EVENT:
            if (mass_device.dev_state == USB_DEVICE_IDLE) {
                mass_device.dev_handle = dev_handle;
                mass_device.intf_handle = intf_handle;
                mass_device.dev_state = USB_DEVICE_ATTACHED;
            } else {
#ifdef __DEBUG__
                printf("Mass Storage Device Is Already Attached\n\r");
#endif
            } /* EndIf */
            break;
        case USB_INTF_EVENT:
            mass_device.dev_state = USB_DEVICE_INTERFACED;
            break;
        case USB_DETACH_EVENT:
            if (mass_device.dev_state != USB_DEVICE_IDLE) {
                mass_device.dev_handle = NULL;
                mass_device.intf_handle = NULL;
                mass_device.dev_state = USB_DEVICE_DETACHED;
            } else {
#ifdef __DEBUG__
                printf("Mass Storage Device Is Not Attached\n\r");
#endif
            } /* EndIf */
            break;
        default:
            mass_device.dev_state = USB_DEVICE_IDLE;
            break;
   } /* EndSwitch */
} /* Endbody */
/* EOF */
