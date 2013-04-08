/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Global BSP variables and functions
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_MPC55XXEVB_BSP_H
#define LIBBSP_POWERPC_MPC55XXEVB_BSP_H

#include <bspopts.h>

#define BSP_SMALL_MEMORY 1

#define BSP_INTERRUPT_STACK_AT_WORK_AREA_BEGIN

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#include <libcpu/powerpc-utility.h>

#include <bsp/tictac.h>
#include <bsp/linker-symbols.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief System clock frequency */
extern unsigned int bsp_clock_speed;

/** @brief Time base clicks per micro second */
extern uint32_t bsp_clicks_per_usec;

/** @brief Convert Decrementer ticks to microseconds */
#define BSP_Convert_decrementer( _value ) \
  (((unsigned long long) (_value)) / ((unsigned long long)bsp_clicks_per_usec))

rtems_status_code mpc55xx_sd_card_init( bool mount);

/* Network driver configuration */

struct rtems_bsdnet_ifconfig;

int smsc9218i_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

#define RTEMS_BSP_NETWORK_DRIVER_ATTACH smsc9218i_attach_detach

#define RTEMS_BSP_NETWORK_DRIVER_NAME "eth0"

rtems_status_code bsp_register_i2c(void);

void bsp_restart(void *addr);

void *bsp_idle_thread(uintptr_t arg);

#define BSP_IDLE_TASK_BODY bsp_idle_thread

LINKER_SYMBOL(bsp_section_dsram_begin)
LINKER_SYMBOL(bsp_section_dsram_end)
LINKER_SYMBOL(bsp_section_dsram_size)
LINKER_SYMBOL(bsp_section_dsram_load_begin)
LINKER_SYMBOL(bsp_section_dsram_load_end)

#define BSP_DSRAM_SECTION __attribute__((section(".bsp_dsram")))

LINKER_SYMBOL(bsp_section_sysram_begin)
LINKER_SYMBOL(bsp_section_sysram_end)
LINKER_SYMBOL(bsp_section_sysram_size)
LINKER_SYMBOL(bsp_section_sysram_load_begin)
LINKER_SYMBOL(bsp_section_sysram_load_end)

#define BSP_SYSRAM_SECTION __attribute__((section(".bsp_sysram")))

typedef enum {
  MPC55XX_FATAL_FMPLL_LOCK,
  MPC55XX_FATAL_CLOCK_EMIOS_IRQ_INSTALL,
  MPC55XX_FATAL_CLOCK_EMIOS_PRESCALER,
  MPC55XX_FATAL_CLOCK_EMIOS_INTERVAL,
  MPC55XX_FATAL_CLOCK_PIT_IRQ_INSTALL,
  MPC55XX_FATAL_CONSOLE_GENERIC_COUNT,
  MPC55XX_FATAL_CONSOLE_GENERIC_REGISTER,
  MPC55XX_FATAL_CONSOLE_GENERIC_REGISTER_CONSOLE,
  MPC55XX_FATAL_CONSOLE_ESCI_BAUD,
  MPC55XX_FATAL_CONSOLE_ESCI_ATTRIBUTES,
  MPC55XX_FATAL_CONSOLE_ESCI_IRQ_INSTALL,
  MPC55XX_FATAL_CONSOLE_LINFLEX_BAUD,
  MPC55XX_FATAL_CONSOLE_LINFLEX_ATTRIBUTES,
  MPC55XX_FATAL_CONSOLE_LINFLEX_RX_IRQ_INSTALL,
  MPC55XX_FATAL_CONSOLE_LINFLEX_TX_IRQ_INSTALL,
  MPC55XX_FATAL_CONSOLE_LINFLEX_ERR_IRQ_INSTALL,
  MPC55XX_FATAL_CONSOLE_LINFLEX_RX_IRQ_REMOVE,
  MPC55XX_FATAL_CONSOLE_LINFLEX_TX_IRQ_REMOVE,
  MPC55XX_FATAL_CONSOLE_LINFLEX_ERR_IRQ_REMOVE
} mpc55xx_fatal_code;

void mpc55xx_fatal(mpc55xx_fatal_code code) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_POWERPC_MPC55XXEVB_BSP_H */
