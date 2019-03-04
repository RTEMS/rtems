/**
 * @file
 *
 * @ingroup RTEMSBSPsSH1
 *
 * @brief Global BSP definitions.
 */

/*
 *  generic sh1
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
 */

#ifndef LIBBSP_SH_GENSH1_BSP_H
#define LIBBSP_SH_GENSH1_BSP_H

/**
 * @defgroup RTEMSBSPsSH1 SH-1
 *
 * @ingroup RTEMSBSPsSH
 *
 * @brief SH-1 Board Support Package.
 *
 * @{
 */

#include <rtems.h>
#include <termios.h> /* for tcflag_t */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EDIT: To activate the sci driver, change the define below */
#if 1
#include <rtems/devnull.h>
#define BSP_CONSOLE_DEVNAME "/dev/null"
#define BSP_CONSOLE_DRIVER_TABLE_ENTRY DEVNULL_DRIVER_TABLE_ENTRY
#else
#include <sh/sci.h>
#define BSP_CONSOLE_DEVNAME "/dev/sci0"
#define BSP_CONSOLE_DRIVER_TABLE_ENTRY DEVSCI_DRIVER_TABLE_ENTRY
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
