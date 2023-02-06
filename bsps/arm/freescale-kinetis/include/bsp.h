/**
 * @file
 *
 * @ingroup RTEMSBSPsARMFREESCALE_KINETIS
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <lichx2@sugon.com>
 *
 * Based on TMS570 BSP
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_FREESCALE_KINETIS_BSP_H
#define LIBBSP_ARM_FREESCALE_KINETIS_BSP_H

/**
 * @defgroup RTEMSBSPsARMFREESCALE_KINETIS FREESCALE_KINETIS
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief FREESCALE_KINETIS Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>
#include <bsp/kinetis.h>
#include <bsp/spi-config.h>

#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT (13 << 4)

#define BSP_ARMV7M_SYSTICK_PRIORITY (14 << 4)

#define BSP_ARMV7M_SYSTICK_FREQUENCY 96000000

#define BSP_GPIO_PIN_COUNT 192
#define BSP_GPIO_PINS_PER_BANK 32
#define BSP_GPIO_PINS_PER_SELECT_BANK 6

rtems_status_code kinetis_sd_card_init( bool mount);

#endif /* ASM */

/* @} */

#endif /* LIBBSP_ARM_FREESCALE_KINETIS_BSP_H */
