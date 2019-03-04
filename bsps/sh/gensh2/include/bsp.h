/**
 * @file
 *
 * @ingroup RTEMSBSPsSH2
 *
 * @brief Global BSP definitions.
 */

/*
 *  generic sh2
 *
 *  This include file contains all board IO definitions.
 */

/*
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Minor adaptations for sh2 by:
 *  John M. Mills (jmills@tga.com)
 *  TGA Technologies, Inc.
 *  100 Pinnacle Way, Suite 140
 *  Norcross, GA 30071 U.S.A.
 *
 *  This modified file may be copied and distributed in accordance
 *  the above-referenced license. It is provided for critique and
 *  developmental purposes without any warranty nor representation
 *  by the authors or by TGA Technologies.
 */

#ifndef LIBBSP_SH_GENSH2_BSP_H
#define LIBBSP_SH_GENSH2_BSP_H

/**
 * @defgroup RTEMSBSPsSH2 SH-2
 *
 * @ingroup RTEMSBSPsSH
 *
 * @brief SH-2 Board Support Package.
 *
 * @{
 */

#include <rtems.h>

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <termios.h> /* for tcflag_t */

#include <sh/sci.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 1
/* FIXME:
 *   These definitions will be no longer necessary if the old
 *   implementation of SCI driver will be droped
 */
#define BSP_CONSOLE_DEVNAME "/dev/sci0"
#define BSP_CONSOLE_MINOR_NUMBER ((rtems_device_minor_number) 0)
#define BSP_CONSOLE_DRIVER_TABLE_ENTRY DEVSCI_DRIVER_TABLE_ENTRY
#define BSP_CONSOLE_DEVICE_TERMIOS_HANDLERS (sh_sci_get_termios_handlers(TRUE))
#endif

/* Constants */

/*
 * BSP methods that cross file boundaries.
 */
void bsp_hw_init(void);

int _sci_get_brparms(
  unsigned int   spd,
  unsigned char *smr,
  unsigned char *brr
);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
