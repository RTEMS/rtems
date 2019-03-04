/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCGen83XX
 *
 * @brief Global BSP definitions.
 */

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
| http://www.rtems.org/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains board specific definitions                   |
\*===============================================================*/


#ifndef LIBBSP_POWERPC_GEN83XX_BSP_H
#define LIBBSP_POWERPC_GEN83XX_BSP_H

/**
 * @defgroup RTEMSBSPsPowerPCGen83XX NXP MPC83XX
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief NXP MPC83XX Board Support Package.
 *
 * @{
 */

#define BSP_FEATURE_IRQ_EXTENSION

#include <bspopts.h>

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

LINKER_SYMBOL(bsp_work_area_start);

LINKER_SYMBOL(IMMRBAR);

#ifndef ASM

#include <rtems.h>
#include <bsp/vectors.h>
#include <bsp/irq.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * indicate, that BSP has no IDE driver
 */
#undef RTEMS_BSP_HAS_IDE_DRIVER

/* misc macros */
#define BSP_ARRAY_CNT(arr) (sizeof(arr)/sizeof(arr[0]))

void *bsp_idle_thread( uintptr_t ignored );
#define BSP_IDLE_TASK_BODY bsp_idle_thread

/* functions */
rtems_status_code bsp_register_i2c(void);
rtems_status_code bsp_register_spi(void);

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int BSP_tsec_attach(struct rtems_bsdnet_ifconfig *config,int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	BSP_tsec_attach

#ifdef MPC83XX_BOARD_MPC8313ERDB
  #define RTEMS_BSP_NETWORK_DRIVER_NAME "tsec2"
  #define RTEMS_BSP_NETWORK_DRIVER_NAME2 "tsec1"
#else
  #define RTEMS_BSP_NETWORK_DRIVER_NAME "tsec1"
  #define RTEMS_BSP_NETWORK_DRIVER_NAME2 "tsec2"
#endif

#if defined(MPC83XX_BOARD_MPC8349EAMDS)
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
#endif /* defined(MPC83XX_BOARD_MPC8349EAMDS) */

#if defined(MPC83XX_BOARD_HSC_CM01)
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
#endif /* defined(MPC83XX_BOARD_HSC_CM01) */

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

void bsp_restart(void *addr);

#if defined(HAS_UBOOT)
  /* Routine to obtain U-Boot environment variables */
  const char *bsp_uboot_getenv(
    const char *name
  );
#endif

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* GEN83xx */
