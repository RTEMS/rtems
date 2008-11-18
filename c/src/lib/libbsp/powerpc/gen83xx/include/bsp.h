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

/*
 * MPC8313E Reference Design Board
 */

#ifdef MPC8313ERDB

#define HAS_UBOOT

/* For U-Boot */
#define CONFIG_MPC83XX
#define CONFIG_HAS_ETH1

/* We want to prompt for a reset and then reset the board */
#define BSP_PRESS_KEY_FOR_RESET 1
#define BSP_RESET_BOARD_AT_EXIT 1

#endif /* MPC8313ERDB */

#include <libcpu/powerpc-utility.h> 

#include <bsp/hwreg_vals.h>

/*
 * Some symbols defined in the linker command file.
 */

LINKER_SYMBOL(bsp_ram_start);
LINKER_SYMBOL(bsp_ram_end);
LINKER_SYMBOL(bsp_ram_size);

LINKER_SYMBOL(bsp_rom_start);
LINKER_SYMBOL(bsp_rom_end);
LINKER_SYMBOL(bsp_rom_size);

LINKER_SYMBOL(bsp_section_text_start);
LINKER_SYMBOL(bsp_section_text_end);
LINKER_SYMBOL(bsp_section_text_size);

LINKER_SYMBOL(bsp_section_data_start);
LINKER_SYMBOL(bsp_section_data_end);
LINKER_SYMBOL(bsp_section_data_size);

LINKER_SYMBOL(bsp_section_bss_start);
LINKER_SYMBOL(bsp_section_bss_end);
LINKER_SYMBOL(bsp_section_bss_size);

LINKER_SYMBOL(bsp_interrupt_stack_start);
LINKER_SYMBOL(bsp_interrupt_stack_end);
LINKER_SYMBOL(bsp_interrupt_stack_size);

LINKER_SYMBOL(bsp_work_area_start);

LINKER_SYMBOL(IMMRBAR);

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
#include <bsp/tictac.h>

#ifdef HAS_UBOOT

#include <bsp/u-boot.h>

extern bd_t mpc83xx_uboot_board_info;

extern const size_t mpc83xx_uboot_board_info_size;

#endif /* HAS_UBOOT */

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

#if defined(MPC8249EAMDS) || defined(HSC_CM01)
#define BSP_USE_UART2 TRUE
#else
#define BSP_USE_UART2 FALSE
#endif

#define SINGLE_CHAR_MODE
#define UARTS_USE_TERMIOS_INT   1

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int BSP_tsec_attach(struct rtems_bsdnet_ifconfig *config,int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	BSP_tsec_attach

#ifdef MPC8313ERDB

#define RTEMS_BSP_NETWORK_DRIVER_NAME	"tsec2"
#define RTEMS_BSP_NETWORK_DRIVER_NAME2	"tsec1"

#else /* MPC8313ERDB */

#define RTEMS_BSP_NETWORK_DRIVER_NAME	"tsec1"
#define RTEMS_BSP_NETWORK_DRIVER_NAME2	"tsec2"

#endif /* MPC8313ERDB */

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

extern unsigned int BSP_bus_frequency;

extern uint32_t bsp_clicks_per_usec;

/*
 *  Convert decrementer value to tenths of microseconds (used by shared timer
 *  driver).
 */
#define BSP_Convert_decrementer( _value ) \
  ((int) (((_value) * 10) / bsp_clicks_per_usec))

void mpc83xx_zero_4( void *dest, size_t n);

void cpu_init( void);

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* GEN83xx */
