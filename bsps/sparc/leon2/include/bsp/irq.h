/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON2
 * @brief Interrupts definitions
 *
 * Based on libbsp/shared/include/irq.h.
 */

/*
 * Copyright (c) 2012.
 * Aeroflex Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_LEON2_IRQ_CONFIG_H
#define LIBBSP_LEON2_IRQ_CONFIG_H

#define BSP_INTERRUPT_VECTOR_MAX_STD 15 /* Standard IRQ controller */
#define BSP_INTERRUPT_VECTOR_MIN 0
#define BSP_INTERRUPT_VECTOR_MAX BSP_INTERRUPT_VECTOR_MAX_STD

/* No extra check is needed */
#undef BSP_INTERRUPT_CUSTOM_VALID_VECTOR

#endif /* LIBBSP_LEON2_IRQ_CONFIG_H */
