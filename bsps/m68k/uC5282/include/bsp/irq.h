/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsm68kuC5282
 *
 * @brief Core IRQ definitions
 */

/*
 * Copyright (c) 2025 Jeremy Lorelli <lorelli@slac.stanford.edu>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_M68K_UC5282_IRQ_H
#define LIBBSP_M68K_UC5282_IRQ_H

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#define BSP_INTERRUPT_CUSTOM_VALID_VECTOR

#define BSP_INTERRUPT_VECTOR_COUNT 192

#define BSP_VME_INTERRUPT_VECTOR_COUNT (255 - BSP_INTERRUPT_VECTOR_COUNT)

#define BSP_FIRST_VME_INTERRUPT_VECTOR BSP_INTERRUPT_VECTOR_COUNT

extern void uC5282_interrupt_vector_install(rtems_vector_number vector);
extern void uC5282_interrupt_vector_remove(rtems_vector_number vector);

#define bsp_interrupt_vector_install(v) uC5282_interrupt_vector_install(v)
#define bsp_interrupt_vector_remove(v) uC5282_interrupt_vector_remove(v)

#endif // LIBBSP_M68K_UC5282_IRQ_H
