/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Global BSP definitions.
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

#ifndef LIBBSP_ARM_LPC24XX_BSP_H
#define LIBBSP_ARM_LPC24XX_BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#define LPC24XX_PCLK (LPC24XX_CCLK / LPC24XX_PCLKDIV)

#define LPC24XX_EMCCLK (LPC24XX_CCLK / LPC24XX_EMCCLKDIV)

#define LPC24XX_MPU_REGION_COUNT 8

#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT (29 << 3)

#define BSP_ARMV7M_SYSTICK_PRIORITY (30 << 3)

#define BSP_ARMV7M_SYSTICK_FREQUENCY LPC24XX_CCLK

#ifndef ASM

struct rtems_bsdnet_ifconfig;

/**
 * @defgroup lpc24xx LPC24XX Support
 *
 * @ingroup bsp_kit
 *
 * @brief LPC24XX support package.
 *
 * @{
 */

/**
 * @brief Network driver attach and detach function.
 */
int lpc_eth_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

/**
 * @brief Standard network driver attach and detach function.
 */
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH lpc_eth_attach_detach

/**
 * @brief Standard network driver name.
 */
#define RTEMS_BSP_NETWORK_DRIVER_NAME "eth0"

/**
 * @brief Optimized idle task.
 *
 * This idle task sets the power mode to idle.  This causes the processor clock
 * to be stopped, while on-chip peripherals remain active.  Any enabled
 * interrupt from a peripheral or an external interrupt source will cause the
 * processor to resume execution.
 *
 * To enable the idle task use the following in the system configuration:
 *
 * @code
 * #include <bsp.h>
 *
 * #define CONFIGURE_INIT
 *
 * #define CONFIGURE_IDLE_TASK_BODY bsp_idle_thread
 *
 * #include <confdefs.h>
 * @endcode
 */
void *bsp_idle_thread(uintptr_t ignored);

#ifdef ARM_MULTILIB_ARCH_V4
  #define BSP_CONSOLE_UART_BASE 0xe000c000
#else
  #define BSP_CONSOLE_UART_BASE 0x4000c000
#endif

void bsp_restart(void *addr);

/** @} */

#endif /* ASM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_BSP_H */
