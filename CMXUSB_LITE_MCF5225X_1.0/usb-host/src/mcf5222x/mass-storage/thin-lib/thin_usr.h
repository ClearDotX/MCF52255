#ifndef _THIN_USR_H
#define _THIN_USR_H

/****************************************************************************
 *
 *            Copyright (c) 2003-2007 by CMX Systems, Inc. 
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
#ifdef __cplusplus
extern "C" {
#endif


#define FAT_SUPERTHIN	/* enable this line if superthin is required */

#ifdef FAT_SUPERTHIN
/**************************************************************************
**
**  FAT super thin user settings
**
**************************************************************************/

#define F_GETVERSION	0	/*defines f_getversion*/
#define F_FORMATTING	0	/*defines f_hardformat*/
#define F_GETFREESPACE	1	/*defines f_getfreespace*/
#define F_CHDIR		0   /*defines f_chdir*/
#define F_MKDIR		0	/*defines f_mkdir*/
#define F_RMDIR		0	/*defines f_rmdir*/
#define F_DELETE	0	/*defines f_delete*/
#define F_FILELENGTH	1	/*defines f_filelength*/
#define F_FINDING	1	/*defines f_findfirst, f_findnext*/
#define F_TELL		1	/*defines f_tell*/
#define F_GETC		1	/*defines f_getc*/
#define F_PUTC		1	/*defines f_putc*/
#define F_REWIND	1	/*defines f_rewind*/
#define F_EOF		1	/*defines f_eof*/
#define F_SEEK		1	/*defines f_seek*/
#define F_WRITE		1	/*defines f_write*/
#define F_DIRECTORIES	0 /*enables subdirectories*/
#define F_CHECKNAME	1	/*enables name checking*/
#define F_TRUNCATE	0	/*enable truncate command*/
#define F_SETEOF	0	/*enable seteof*/

#define F_WRITING	1	/*removes everything related to write*/

#define F_FAT12		1	/*enables FAT12 usage*/
#define F_FAT16		1	/*enables FAT16 usage*/
#define F_FAT32		1	/*enables FAT32 usage*/


#define F_MAXPATH	64	/*considered unsigned char in thin version*/


#include "fat_sthin/fat_sthin.h"

#else
/**************************************************************************
**
**  FAT thin user settings
**
**************************************************************************/

#define F_GETVERSION	1	/*defines f_getversion*/
#define F_FORMATTING	1	/*defines f_format, f_hardformat*/
#define F_SETLABEL	1	/*defines f_setlabel*/
#define F_GETLABEL	1	/*defines f_getlabel*/
#define F_GETFREESPACE	1	/*defines f_getfreespace*/
  #define F_GETFREESPACE_QUICK 1/*use quick getfreespace */
#define F_CHDIR		1	/*defines f_chdir*/
#define F_MKDIR		1	/*defines f_mkdir*/
#define F_RMDIR		1	/*defines f_rmdir*/
#define F_RENAME	1	/*defines f_rename*/
#define F_DELETE	1	/*defines f_delete*/
#define F_GETTIMEDATE	1	/*defines f_gettimedate*/
#define F_SETTIMEDATE	1	/*defines f_settimedate*/
#define F_GETATTR	1	/*defines f_getattr*/
#define F_SETATTR	1	/*defines f_setattr*/
#define F_FILELENGTH	1	/*defines f_filelength*/
#define F_FINDING	1	/*defines f_findfirst, f_findnext*/
#define F_TELL		1	/*defines f_tell*/
#define F_GETC		1	/*defines f_getc*/
#define F_PUTC		1	/*defines f_putc*/
#define F_REWIND	1	/*defines f_rewind*/
#define F_EOF		1	/*defines f_eof*/
#define F_SEEK		1	/*defines f_seek*/
#define F_WRITE		1	/*defines f_write*/
#define F_DIRECTORIES	1	/*enables subdirectories*/
#define F_CHECKNAME	1	/*enables name checking*/
#define F_TRUNCATE	1	/*enable truncate command*/
#define F_SETEOF	1	/*enable seteof*/
#define F_LONGFILENAME	0	/* 0 - 8+3 names   1 - long file names  */

#define F_WRITING	1	/*removes everything related to write*/

#define FATBITFIELD_ENABLE 0	/*enable bitfield for FAT, requires malloc and quick getfreespace
                                  will be automaticaly enabled */ 

#define F_FAT12		0	/*enables FAT12 usage*/
#define F_FAT16		1	/*enables FAT16 usage*/
#define F_FAT32		0	/*enables FAT32 usage*/

#define F_MAXFILES	1	/*maximum number of files */


#define F_MAXPATH	64	/*considered unsigned char in thin version*/

#if F_LONGFILENAME
#define F_MAXLNAME	128	/*maximum length of the file name in long file name mode */
#endif


#include "fat_thin/fat_thin.h"

#endif




#ifdef __cplusplus
}
#endif

#endif

