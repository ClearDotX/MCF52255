/***************************************************************************
 * mcfcau.c - Implementation of DES & Triple DES EDE Cipher Algorithms
 *                for Freescale ColdFire Cryptographic Acceleration Unit (CAU).
 *
 * Author: Andrey Butok
 * Copyright Freescale Semiconductor Inc.  2007
 *
 * NOTE: You can find the ColdFire CAU module on MCF54455 and MCF52235.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ***************************************************************************
 * Changes:
 * v0.01	28 September 2006	Andrey Butok
 *   		Initial Release - developed on 2.6.20 Linux kernel.
 */
#include <linux/module.h>

DEFINE_SPINLOCK(mcfcau_lock);
EXPORT_SYMBOL(mcfcau_lock);
