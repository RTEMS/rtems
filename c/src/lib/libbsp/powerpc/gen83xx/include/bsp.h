/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains board specific definitions                   |
\*===============================================================*/

#ifndef __GEN83xx_BSP_h
#define __GEN83xx_BSP_h

#include <bsp/hwreg_vals.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

#include "bspopts.h"

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>

/* miscellaneous stuff assumed to exist */

/*
 * We need to decide how much memory will be non-cacheable. This
 * will mainly be memory that will be used in DMA (network and serial
 * buffers).
 */
/*
 *  Stuff for Time Test 27
 */
#define MUST_WAIT_FOR_INTERRUPT 0

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */
#define BSP_UART1_MINOR 0
#define BSP_UART2_MINOR 1

/*
 * NOTE: Use the standard Clock driver entry
 */

/*
 * indicate, that BSP has no IDE driver
 */
#undef RTEMS_BSP_HAS_IDE_DRIVER

/*
 * How many libio files we want
 */
#define BSP_LIBIO_MAX_FDS       20

/* misc macros */
#define BSP_ARRAY_CNT(arr) (sizeof(arr)/sizeof(arr[0]))

/* functions */

void bsp_cleanup(void);
rtems_status_code bsp_register_i2c(void);
rtems_status_code bsp_register_spi(void);

/* console modes (only termios) */
#ifdef  PRINTK_MINOR
#undef  PRINTK_MINOR
#endif
#define PRINTK_MINOR BSP_UART1_MINOR

#if defined(MPC8249EAMDS)
#define BSP_USE_UART2 TRUE
#else
#define BSP_USE_UART2 FALSE
#endif

#define SINGLE_CHAR_MODE
#define UARTS_USE_TERMIOS_INT   1

/*
 *  Convert decrement value to tenths of microsecnds (used by
 *  shared timer driver).
 *
 *    + CPU has a csb_clock bus,
 *    + There are 4 bus cycles per click
 *    + We return value in 1/10 microsecond units.
 *   Modified following equation to integer equation to remove
 *   floating point math.
 *   (int) ((float)(_value) / ((XLB_CLOCK/1000000 * 0.1) / 4.0))
 */

extern unsigned int BSP_bus_frequency;
#define BSP_Convert_decrementer( _value ) \
  (int) (((_value) * 4000) / (BSP_bus_frequency/10000))

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int BSP_tsec_attach(struct rtems_bsdnet_ifconfig *config,int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"tsec1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	BSP_tsec_attach

#define RTEMS_BSP_NETWORK_DRIVER_NAME2	"tsec2"

#if defined(MPC8349EAMDS)
/*
 * i2c EEPROM device name
 */
#define RTEMS_BSP_I2C_EEPROM_DEVICE_NAME "eeprom"
#define RTEMS_BSP_I2C_EEPROM_DEVICE_PATH "/dev/i2c1.eeprom"

/*
 * SPI Flash device name
 */
#define RTEMS_BSP_SPI_FLASH_DEVICE_NAME "flash"
#define RTEMS_BSP_SPI_FLASH_DEVICE_PATH "/dev/spi.flash"
#endif /* defined(MPC8349EAMDS) */

#if defined(HSC_CM01)
/*
 * i2c EEPROM device name
 */
#define RTEMS_BSP_I2C_EEPROM_DEVICE_NAME "eeprom"
#define RTEMS_BSP_I2C_EEPROM_DEVICE_PATH "/dev/i2c1.eeprom"

/*
 * SPI FRAM device name
 */
#define RTEMS_BSP_SPI_FRAM_DEVICE_NAME "fram"
#define RTEMS_BSP_SPI_FRAM_DEVICE_PATH "/dev/spi.fram"
#endif /* defined(HSC_CM01) */

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* GEN83xx */
