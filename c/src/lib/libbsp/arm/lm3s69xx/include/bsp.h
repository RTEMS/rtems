/*
 * Copyright (c) 2011-2012 Sebastian Huber.  All rights reserved.
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

#ifndef LIBBSP_ARM_LM3S69XX_BSP_H
#define LIBBSP_ARM_LM3S69XX_BSP_H

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT (5 << 5)

#define BSP_ARMV7M_SYSTICK_PRIORITY (6 << 5)

#define BSP_ARMV7M_SYSTICK_FREQUENCY LM3S69XX_SYSTEM_CLOCK

#ifndef ASM

#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#endif /* ASM */

#endif /* LIBBSP_ARM_LM3S69XX_BSP_H */
