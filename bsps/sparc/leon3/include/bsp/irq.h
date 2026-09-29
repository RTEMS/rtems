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

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
/*
 * A bus line is an interrupt line of the system interrupt bus.  A controller
 * line is an interrupt line of the IRQ(A)MP.  With an interrupt map, an
 * interrupt vector is a bus line.  The dispatch table has one entry for each
 * controller line.
 */
#define BSP_INTERRUPT_VECTOR_COUNT        LEON3_IRQMAP_BUS_LINE_COUNT
#define BSP_INTERRUPT_DISPATCH_TABLE_SIZE ( BSP_INTERRUPT_VECTOR_MAX_EXT + 1 )
#else
#define BSP_INTERRUPT_VECTOR_COUNT ( BSP_INTERRUPT_VECTOR_MAX_EXT + 1 )
#endif

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
 * In SMP and multiprocessing configurations, bus line 0 connects to the
 * controller line defined by the BSP option LEON3_IPI_CONTROLLER_LINE.
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

/**
 * @brief Sets the controller line of the bus line.
 *
 * Where the BSP uses the interrupt map, the directive connects the bus line to
 * the controller line in the interrupt map.  The directive leaves the mask,
 * level, force and affinity state of both controller lines unchanged.  The
 * state of the new controller line then applies to the bus line.
 *
 * A controller line of zero disconnects the bus line.  The bus line is then
 * no valid interrupt vector.
 *
 * Bus lines which map to the same controller line share its interrupt
 * handlers.  They also share its enable, pending and affinity state.
 *
 * The interrupt map works only if the boot processor uses the first internal
 * interrupt controller of the IRQ(A)MP.
 *
 * Where the BSP uses no interrupt map, a bus line maps to the controller line
 * of the same number and the map cannot change.
 *
 * @param bus_line is the bus line number.
 *
 * @param controller_line is the controller line number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The number specified by `bus_line` was
 *   zero, or it was greater than or equal to the count of bus lines.
 *
 * @retval ::RTEMS_INVALID_NUMBER The number specified by `controller_line`
 *   was greater than the last controller line.
 *
 * @retval ::RTEMS_UNSATISFIED The BSP used no interrupt map, and the number
 *   specified by `controller_line` was not equal to `bus_line`.
 *
 * @retval ::RTEMS_INCORRECT_STATE The interrupt support was not initialized.
 *
 * @retval ::RTEMS_CALLED_FROM_ISR The directive was called from within
 *   interrupt context.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE An interrupt handler was installed on the
 *   current controller line of the bus line, and the number specified by
 *   `controller_line` was not equal to this controller line.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within device driver initialization
 *   context.
 *
 * - The directive may be called from within task context.
 *
 * - The directive may obtain and release the object allocator mutex.  This
 *   may cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code leon3_irqmap_set(
  rtems_vector_number bus_line,
  rtems_vector_number controller_line
);

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
extern uint8_t LEON3_IrqCtrl_Mapping[ BSP_INTERRUPT_VECTOR_COUNT ];

#define bsp_interrupt_vector_modify( v ) LEON3_IrqCtrl_Mapping[ ( v ) ]
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_LEON3_IRQ_CONFIG_H */
