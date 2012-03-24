/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
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

#ifndef LIBBSP_ARM_SHARED_ARMV7M_IRQ_H
#define LIBBSP_ARM_SHARED_ARMV7M_IRQ_H

#ifdef __cplusplus
extern "C" {
#endif

void _ARMV7M_NVIC_Interrupt_dispatch(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARMV7M_IRQ_H */
