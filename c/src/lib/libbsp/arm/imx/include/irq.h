/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_IMX_IRQ_H
#define LIBBSP_ARM_IMX_IRQ_H

#ifndef ASM

#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#include <bsp/arm-gic-irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_INTERRUPT_VECTOR_MIN 0
#define BSP_INTERRUPT_VECTOR_MAX 159

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_IMX_IRQ_H */
