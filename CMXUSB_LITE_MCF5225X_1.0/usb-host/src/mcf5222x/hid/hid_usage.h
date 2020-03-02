/***************************************************************************
 *
 *            Copyright (c) 2006-2007 by CMX Systems, Inc.
 *
 * This software is copyrighted by and is the sole property of
 * CMX.  All rights, title, ownership, or other interests
 * in the software remain the property of CMX.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of CMX.
 *
 * CMX reserves the right to modify this software without notice.
 *
 * CMX Systems, Inc.
 * 12276 San Jose Blvd. #511
 * Jacksonville, FL 32223
 * USA
 *
 * Tel:  (904) 880-1840
 * Fax:  (904) 880-1632
 * http: www.cmx.com
 * email: cmx@cmx.com
 *
 ***************************************************************************/
#ifndef _HID_USAGE_H_
#define _HID_USAGE_H_

#define UV(v)   ((hcc_u16)(v))
/****************************************************/
/* HID 1.11 usage pages                             */
/****************************************************/

#define HIDUPG_UNDEFINED  UV(0x00)   /* Undefined */
/**** Top level pages */
#define HIDUPG_GEN_DES    UV(0x01)   /* Generic Desktop Controls*/
#define HIDUPG_SIM_CTR    UV(0x02)   /* Simulation Controls */
#define HIDUPG_VR_CTR     UV(0x03)   /* VR Controls */
#define HIDUPG_SPORT_CTR  UV(0x04)   /* Sport Controls */
#define HIDUPG_GAME_CTR   UV(0x05)   /* Game Controls */
#define HIDUPG_GEN_DEV    UV(0x06)   /* Generic Device Controls */
#define HIDUPG_KEYB       UV(0x07)   /* Keyboard/Keypad */
#define HIDUPG_LED        UV(0x08)   /* LEDs */
#define HIDUPG_BUTTON     UV(0x09)   /* Button */
#define HIDUPG_ORDINAL    UV(0x0A)   /* Ordinal */
#define HIDUPG_PHONE      UV(0x0B)   /* Telephony */
#define HIDUPG_CONSUMER   UV(0x0C)   /* Consumer */
#define HIDUPG_DIGITIZER  UV(0x0D)   /* Digitizer*/
/* 0E    Reserved */
#define HIDUPG_PID        UV(0x0F)   /* PID Page (force feedback and related devices) */
#define HIDUPG_UNICODE    UV(0x10)   /* Unicode */
/* 11-13 Reserved */
#define HIDUPG_ALNUM_DISP UV(0x14)   /* Alphanumeric Display */
/* 15-1f Reserved */
/**** END of top level pages */
/* 25-3f Reserved */
#define HIDUPG_MEDICAL    UV(0x40)   /* Medical Instruments */
/* 41-7F Reserved */
/*80-83 Monitor pages USB Device Class Definition for Monitor Devices
  84-87 Power pages USB Device Class Definition for Power Devices */
/* 88-8B Reserved */
#define HIDUPG_BARCODE    UV(0x8C)   /* Bar Code Scanner page */
#define HIDUPG_SCALE      UV(0x8D)   /* Scale page */
#define HIDUPG_MSR        UV(0x8E)   /* Magnetic Stripe Reading (MSR) Devices */
#define HIDUPG_POS        UV(0x8F)   /* Reserved Point of Sale pages */
#define HIDUPG_CAMERA_CTR UV(0x90)   /* Camera Control Page */
#define HIDUPG_ARCADE     UV(0x91)   /* Arcade Page */

/****************************************************/
/* Usage definitions for the "Generic Decktop" page */
/****************************************************/
#define HIDU_UNDEFINED     UV(0x00)   /* Undefined */
#define HIDU_POINTER       UV(0x01)   /* Pointer (Physical Collection) */
#define HIDU_MOUSE         UV(0x02)   /* Mouse (Application Collection) */
/* 03 Reserved */
#define HIDU_JOYSTICK      UV(0x04)   /* Joystick (Application Collection) */
#define HIDU_GAMEPAD       UV(0x05)   /* Game Pad (Application Collection) */
#define HIDU_KBD           UV(0x06)   /* Keyboard (Application Collection) */
#define HIDU_KEYPAD        UV(0x07)   /* Keypad (Application Collection) */
#define HIDU_MAX_CTR       UV(0x08)   /* Multi-axis Controller (Application Collection) */
/* 09-2F Reserved */
#define HIDU_X             UV(0x30)   /* X (Dynamic Value) */
#define HIDU_Y             UV(0x31)   /* Y (Dynamic Value) */
#define HIDU_Z             UV(0x32)   /* Z (Dynamic Value) */
#define HIDU_RX            UV(0x33)   /* Rx (Dynamic Value) */
#define HIDU_RY            UV(0x34)   /* Ry (Dynamic Value) */
#define HIDU_RZ            UV(0x35)   /* Rz (Dynamic Value) */
#define HIDU_SLIDER        UV(0x36)   /* Slider (Dynamic Value) */
#define HIDU_DIAL          UV(0x37)   /* Dial (Dynamic Value) */
#define HIDU_WHEEL         UV(0x38)   /* Wheel (Dynamic Value) */
#define HIDU_HATSW         UV(0x39)   /* Hat switch (Dynamic Value) */
#define HIDU_COUNTEDBUF    UV(0x3A)   /* Counted Buffer (Logical Collection) */
#define HIDU_BYTECOUNT     UV(0x3B)   /* Byte Count (Dynamic Value) */
#define HIDU_MOTIONWAKE    UV(0x3C)   /* Motion Wakeup (One Shot Control) */
#define HIDU_START         UV(0x3D)   /* Start (On/Off Control) */
#define HIDU_SELECT        UV(0x3E)   /* Select (On/Off Control) */
/* 3F Reserved */
#define HIDU_VX            UV(0x40)   /* Vx (Dynamic Value) */
#define HIDU_VY            UV(0x41)   /* Vy (Dynamic Value) */
#define HIDU_VZ            UV(0x42)   /* Vz (Dynamic Value) */
#define HIDU_VBRX          UV(0x43)   /* Vbrx (Dynamic Value) */
#define HIDU_VBRY          UV(0x44)   /* Vbry (Dynamic Value) */
#define HIDU_VBRZ          UV(0x45)   /* Vbrz (Dynamic Value) */
#define HIDU_VNO           UV(0x46)   /* Vno (Dynamic Value) */
#define HIDU_FEATNOTIF     UV(0x47)   /* Feature Notification (Dynamic Value),(Dynamic Flag) */
/* 48-7F Reserved */
#define HIDU_SYSCTL        UV(0x80)   /* System Control (Application Collection) */
#define HIDU_PWDOWN        UV(0x81)   /* System Power Down (One Shot Control) */
#define HIDU_SLEEP         UV(0x82)   /* System Sleep (One Shot Control) */
#define HIDU_WAKEUP        UV(0x83)   /* System Wake Up (One Shot Control)  */
#define HIDU_CONTEXTM      UV(0x84)   /* System Context Menu (One Shot Control) */
#define HIDU_MAINM         UV(0x85)   /* System Main Menu (One Shot Control) */
#define HIDU_APPM          UV(0x86)   /* System App Menu (One Shot Control) */
#define HIDU_MENUHELP      UV(0x87)   /* System Menu Help (One Shot Control) */
#define HIDU_MENUEXIT      UV(0x88)   /* System Menu Exit (One Shot Control) */
#define HIDU_MENUSELECT    UV(0x89)   /* System Menu Select (One Shot Control) */
#define HIDU_SYSM_RIGHT    UV(0x8A)   /* System Menu Right (Re-Trigger Control) */
#define HIDU_SYSM_LEFT     UV(0x8B)   /* System Menu Left (Re-Trigger Control) */
#define HIDU_SYSM_UP       UV(0x8C)   /* System Menu Up (Re-Trigger Control) */
#define HIDU_SYSM_DOWN     UV(0x8D)   /* System Menu Down (Re-Trigger Control) */
#define HIDU_COLDRESET     UV(0x8E)   /* System Cold Restart (One Shot Control) */
#define HIDU_WARMRESET     UV(0x8F)   /* System Warm Restart (One Shot Control) */
#define HIDU_DUP           UV(0x90)   /* D-pad Up (On/Off Control) */
#define HIDU_DDOWN         UV(0x91)   /* D-pad Down (On/Off Control) */
#define HIDU_DRIGHT        UV(0x92)   /* D-pad Right (On/Off Control) */
#define HIDU_DLEFT         UV(0x93)   /* D-pad Left (On/Off Control) */
/* 94-9F Reserved */
#define HIDU_SYS_DOCK      UV(0xA0)   /* System Dock (One Shot Control) */
#define HIDU_SYS_UNDOCK    UV(0xA1)   /* System Undock (One Shot Control) */
#define HIDU_SYS_SETUP     UV(0xA2)   /* System Setup (One Shot Control) */
#define HIDU_SYS_BREAK     UV(0xA3)   /* System Break (One Shot Control) */
#define HIDU_SYS_DBGBRK    UV(0xA4)   /* System Debugger Break (One Shot Control) */
#define HIDU_APP_BRK       UV(0xA5)   /* Application Break (One Shot Control) */
#define HIDU_APP_DBGBRK    UV(0xA6)   /* Application Debugger Break (One Shot Control) */
#define HIDU_SYS_SPKMUTE   UV(0xA7)   /* System Speaker Mute (One Shot Control) */
#define HIDU_SYS_HIBERN    UV(0xA8)   /* System Hibernate (One Shot Control) */
/* A9-AF Reserved */
#define HIDU_SYS_SIDPINV   UV(0xB0)   /* System Display Invert (One Shot Control) */
#define HIDU_SYS_DISPINT   UV(0xB1)   /* System Display Internal (One Shot Control) */
#define HIDU_SYS_DISPEXT   UV(0xB2)   /* System Display External (One Shot Control) */
#define HIDU_SYS_DISPBOTH  UV(0xB3)   /* System Display Both (One Shot Control) */
#define HIDU_SYS_DISPDUAL  UV(0xB4)   /* System Display Dual (One Shot Control) */
#define HIDU_SYS_DISPTGLIE UV(0xB5)   /* System Display Toggle Int/Ext (One Shot Control) */
#define HIDU_SYS_DISP_SWAP UV(0xB6)   /* System Display Swap Primary/Secondary (One Shot Control) */
#define HIDU_SYS_DIPS_LCDA UV(0xB7)   /* System Display LCD Autoscale (One Shot Control) */
/* B8-FFFF Reserved */
#endif
/****************************** END OF FILE **********************************/
