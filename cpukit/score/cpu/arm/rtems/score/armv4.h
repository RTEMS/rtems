/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef RTEMS_SCORE_ARMV4_H
#define RTEMS_SCORE_ARMV4_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef ARM_MULTILIB_ARCH_V4

void bsp_interrupt_dispatch( void );

void _ARMV4_Exception_interrupt( void );

typedef void arm_exc_abort_handler( arm_cpu_context *context );

void arm_exc_data_abort_set_handler( arm_exc_abort_handler handler );

void arm_exc_data_abort( void );

void arm_exc_prefetch_abort_set_handler( arm_exc_abort_handler handler );

void arm_exc_prefetch_abort( void );

#endif /* ARM_MULTILIB_ARCH_V4 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SCORE_ARMV4_H */
