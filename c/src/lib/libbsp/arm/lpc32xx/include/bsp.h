/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC32XX_BSP_H
#define LIBBSP_ARM_LPC32XX_BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

struct rtems_bsdnet_ifconfig;

/**
 * @defgroup lpc32xx LPC32XX Support
 *
 * @ingroup bsp_kit
 *
 * @brief LPC32XX support package.
 *
 * @{
 */

/**
 * @brief Network driver attach and detach function.
 */
int lpc32xx_eth_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

/**
 * @brief Standard network driver attach and detach function.
 */
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	lpc32xx_eth_attach_detach

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
 * #define CONFIGURE_IDLE_TASK_BODY lpc32xx_idle
 *
 * #include <confdefs.h>
 * @endcode
 */
void *lpc32xx_idle(uintptr_t ignored);

/** @} */

#endif /* ASM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_BSP_H */
