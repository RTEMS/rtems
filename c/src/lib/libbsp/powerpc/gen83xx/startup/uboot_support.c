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
/* XXX TODO fill in with real information */
/* Base address of U-Boot environment variables */
const uint8_t *uboot_environment      = (const uint8_t *) 0x00000000;

/* Length of area reserved for U-Boot environment variables */
const size_t  *uboot_environment_size = (const size_t *) 0x00010000;
#endif
