/*
 *  This file contains variables which assist the shared
 *  U-Boot code.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdint.h>

#include <bsp.h>

#if defined(HAS_UBOOT)
/* Base address of U-Boot environment variables */
const uint8_t *uboot_environment      = (const uint8_t *)0xfff40000;

/* Length of area reserved for U-Boot environment variables */
const size_t  uboot_environment_size = 0x10000;
#endif
