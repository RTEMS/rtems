/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON3
 * @brief LEON3 generic shared IRQ setup
 *
 * Based on bsps/sparc/leon3/include/bsp/irq.h.
 */

/*
 * Copyright (c) 2012.
 * Aeroflex Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBBSP_LEON3_IRQ_CONFIG_H
#define LIBBSP_LEON3_IRQ_CONFIG_H

#include <rtems.h>
#include <bspopts.h>

#define BSP_INTERRUPT_VECTOR_MAX_STD 15 /* Standard IRQ controller */
#define BSP_INTERRUPT_VECTOR_MAX_EXT 31 /* Extended IRQ controller */
#define BSP_INTERRUPT_VECTOR_MAX_MAP \
  63 /* Extended IRQ controller with mapping registers */

#define BSP_INTERRUPT_VECTOR_COUNT ( BSP_INTERRUPT_VECTOR_MAX_EXT + 1 )

/* The check is different depending on IRQ controller, runtime detected */
#define BSP_INTERRUPT_CUSTOM_VALID_VECTOR

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the controller line of the bus line.
 *
 * Where the BSP uses no interrupt map, a bus line maps to the controller line
 * of the same number.  Where the BSP uses the interrupt map, the directive
 * gets the controller line to which the interrupt map connects the bus line.
 *
 * Bus lines which map to the same controller line share its interrupt
 * handlers.  They also share its enable, pending and affinity state.
 *
 * @param bus_line is the bus line number.
 *
 * @param[out] controller_line is the pointer to an rtems_vector_number object.
 *   When the directive call is successful, the number of the controller line
 *   of the bus line will be stored in this object.  When the number specified
 *   by `bus_line` is invalid, UINT32_MAX will be stored in this object.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The `controller_line` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NUMBER The number specified by `bus_line` was
 *   greater than or equal to the count of bus lines.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within interrupt context.
 *
 * - The directive may be called from within device driver initialization
 *   context.
 *
 * - The directive may be called from within task context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code leon3_irqmap_get(
  rtems_vector_number  bus_line,
  rtems_vector_number *controller_line
);

#ifdef LEON3_IRQAMP_IRQMAP
extern rtems_vector_number
  LEON3_IrqCtrl_Mapping[ BSP_INTERRUPT_VECTOR_MAX_MAP + 1 ];

#define bsp_interrupt_vector_modify( v ) LEON3_IrqCtrl_Mapping[ ( v ) ]
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_LEON3_IRQ_CONFIG_H */
