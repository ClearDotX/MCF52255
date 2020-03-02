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
#include "mcf5222x_reg.h"
#include "uart-drv/uart.h"
#include "usb-drv/usb_host.h"
#include "mst-drv/usb_mst.h"
#include "mst-drv/scsi.h"
#include "thin-lib/thin_usr.h"
#include "thin-lib/mst_glue.h"
#include "terminal/hcc_terminal.h"
#include "target.h"

#define DMST_NODEV   0u
#define DMST_ACTIVE  1u
#define DMST_BADDEV  2u

static struct {
  hcc_u8 state;
  hcc_u8 nlun;
} demo_info;

void enable_usb_pull_downs(void)
{
  MCF_USB_OTG_CTRL = MCF_USB_OTG_CTRL_DP_LOW | MCF_USB_OTG_CTRL_DM_LOW |MCF_USB_OTG_CTRL_OTG_EN;
  MCF_GPIO_PQSPAR |= MCF_GPIO_PQSPAR_PQSPAR5(3) | MCF_GPIO_PQSPAR_PQSPAR6(3);
}

static void demo_process(void)
{
  switch(demo_info.state)
  {
  case DMST_NODEV: /* In this state we have no device connected. */
    /* Check if a device has been connected. */
    if(host_scan_for_device())
    {
      print("Device connection detected.\r\n");
      /* Try to start USB driver. */
      demo_info.nlun=(hcc_u8)usb_mst_init();
      /* No ligical units -> not mass storage or error. */
      if (demo_info.nlun)
      { /* attach scsi to device. */
        print("Mass-storage driver started.\r\n");
        scsi_open_device(usb_mst_transfer);
        f_initvolume();
        demo_info.state=DMST_ACTIVE;
      }
      else
      {
        demo_info.state=DMST_BADDEV;
        print("Mass-storage driver initialization failure.\r\n");
        print("Please connect another device.\r\n");
      }
    }
    else
    {
      if (tr_error != tre_none)
      {
        demo_info.state=DMST_BADDEV;
        print("Mass-storage driver initialization failure.\r\n");
        print("Please connect another device.\r\n");
      }
    }
    break;
  case DMST_ACTIVE:
    if (!host_has_device())
    {
      demo_info.nlun=0;
      demo_info.state=DMST_NODEV;
      print("Device disconnected.\r\n");
    }
    break;
  case DMST_BADDEV:
    if (!host_has_device())
    {
      demo_info.state=DMST_NODEV;
      print("Device disconnected.\r\n");
    }
    break;
  }
}

void cmd_dir(char *param);
void cmd_dir(char *param)
{
  F_FIND find;
  int ret;

  param++;
  ret=f_findfirst("*.*",&find);
  if (ret != F_NO_ERROR)
  {
    if (ret == F_ERR_NOTFOUND)
    {
      print("No files found\r\n");
    }
    else
    {
      print("Request failed.\r\n");
    }
    return;
  }
  else
  {
    do
    {
      print(find.filename);
      print("\r\n");
      ret=f_findnext(&find);
    } while(ret == F_NO_ERROR);
  }
}


const hcc_u8 hdigits[]="0123456789ABCDEF";
const hcc_u8 ascii[]= {
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31,
    0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
    0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,
    0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
    0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
    0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
    0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
    0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
    0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd,
    0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1,
    0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb,
    0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,
    0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
    0xfa, 0xfb, 0xfc, 0xfd, '.', '.', '.'
};

void cmd_dump(char *param);
void cmd_dump(char *param)
{
  F_FILE *file;
  hcc_u8 line[8];
  file=f_open(param, "r");
  if (file == 0)
  {
    print("Failed to open ");
    print(param);
    print(".\r\n");
    return;
  }

  while(1)
  {
    int r = f_read(line, 1, sizeof(line), file);
    if (r)
    {
      hcc_u8 hstring[(5*sizeof(line))+1];
      hcc_u8 astring[sizeof(line)+1];
      int line_pos;
      int hpos=0;
      for(line_pos=0; line_pos<r; line_pos++)
      {

        hstring[hpos++]='0';
        hstring[hpos++]='x';
        hstring[hpos++]=hdigits[(line[line_pos] & 0xf0)>>4];
        hstring[hpos++]=hdigits[line[line_pos] & 0x0f];
        hstring[hpos++]=' ';
        astring[line_pos]=ascii[line[line_pos]];
      }
      hstring[hpos-1]=0;
      astring[line_pos]=0;
      print((char *)hstring);
      print("  ");
      print((char *)astring);
      print("\r\n");
    }
    else
    {
      if (!f_eof(file))
      {
        print("Error while reading ");
        print(param);
        print(".\r\n");
      }
      f_close(file);
      break;
    }
  }
  return;
}

void cmd_type(char *param);
void cmd_type(char *param)
{
  F_FILE *file;
  hcc_u8 line[8];
  file=f_open(param, "r");
  if (file == 0)
  {
    print("Failed to open ");
    print(param);
    print(".\r\n");
    return;
  }

  while(1)
  {
    int r = f_read(line, 1, sizeof(line)-1, file);
    if (r)
    {
      line[r]='\0';
      print((char *)line);
    }
    else
    {
      if (!f_eof(file))
      {
        print("Error while reading ");
        print(param);
        print(".\r\n");
      }
      f_close(file);
      break;
    }
  }
  return;
}

static const command_t dir_cmd = {
  "dir", cmd_dir, "List directory."
};
static const command_t dump_cmd = {
  "dump", cmd_dump, "Dump contents of a binary file."
};
static const command_t type_cmd = {
  "type", cmd_type, "Dumpt contents of a text file."
};


int main(void)
{
  hw_init();
//MCF52259 Update
  uart_init(115200, 0, 'n', 8);
//uart_init(115200, 1, 'n', 8);
  terminal_init(uart_putch, uart_getch, uart_kbhit);
  terminal_add_cmd((void*)&dir_cmd);
  terminal_add_cmd((void*)&dump_cmd);
  terminal_add_cmd((void*)&type_cmd);

  Usb_Vbus_On();

  host_init();
  scsi_init();

  demo_info.nlun=0;
  demo_info.state=DMST_NODEV;

  while(1)
  {
    terminal_process();
    demo_process();
  }
}
/****************************** END OF FILE **********************************/
