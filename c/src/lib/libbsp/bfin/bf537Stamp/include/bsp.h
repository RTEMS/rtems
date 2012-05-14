/*  bsp.h
 *
 *  This include file contains all board IO definitions for bf537Stamp.
 *
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#include <libcpu/bf537.h>
#include <libcpu/memoryRegs.h>

/* configure data cache to use 16K of each SRAM bank when enabled */
#define BSP_DATA_CACHE_CONFIG  (3 << DMEM_CONTROL_DMC_SHIFT)


/*
 * PLL and clock setup values:
 */

/*
 *  PLL configuration for bf533Stamp
 *
 *  XTL   =  27 MHz
 *  CLKIN =  13 MHz
 *  VCO   = 391 MHz
 *  CCLK  = 391 MHz
 *  SCLK  = 130 MHz
 */

#define PLL_CSEL    0x0000      /* CCLK = VCO      */
#define PLL_SSEL    0x0003      /* SCLK = CCLK/3   */
#define PLL_MSEL    0x3A00      /* VCO = 29xCLKIN  */
#define PLL_DF      0x0001      /* CLKIN = XTL/2   */

#define CCLK        500000000   /* CORE CLOCK     */
#define SCLK        100000000   /* SYSTEM CLOCK   */

#define CONSOLE_FORCE_BAUD   57600

/*
 * Blackfin environment memory map
 */
#define L1_DATA_SRAM_A 0xff800000L

#define FIFOLENGTH 0x100

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define rtems_bsp_delay( microseconds ) \
  { \
  }

/* Constants */

#define RAM_START 0
#define RAM_END   0x4000000

/* functions */

/*
 * Helper Function to use the EzKits LEDS.
 * Can be used by the Application.
 */
void setLED(uint8_t value);

/*
 * Helper Function to use the EzKits LEDS
 */
uint8_t getLED(void);

rtems_isr_entry set_vector(                     /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int bf537Stamp_network_driver_attach(struct rtems_bsdnet_ifconfig *, int);

#define RTEMS_BSP_NETWORK_DRIVER_NAME   "eth1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH bf537Stamp_network_driver_attach

#ifdef __cplusplus
}
#endif

#endif
