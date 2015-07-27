/**
 * @file
 *
 * @ingroup arm_raspberrypi
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2013 Alan Cudmore
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_BSP_H
#define LIBBSP_ARM_RASPBERRYPI_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <bsp/raspberrypi.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_GPIO_PIN_COUNT 32
#define BSP_GPIO_PINS_PER_BANK 32
#define BSP_GPIO_PINS_PER_SELECT_BANK 10

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_RASPBERRYPI_BSP_H */

/**
 * @defgroup arm_raspberrypi Raspberry Pi Support
 *
 * @ingroup bsp_arm
 *
 * @brief Raspberry Pi support package
 *
 */
