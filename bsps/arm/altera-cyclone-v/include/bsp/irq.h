/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycVIRQ
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_ALTERA_CYCLONE_V_IRQ_H
#define LIBBSP_ARM_ALTERA_CYCLONE_V_IRQ_H

#ifndef ASM

#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#include <bsp/arm-a9mpcore-irq.h>
#include <bsp/arm-gic-irq.h>
#include <bsp/alt_interrupt_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsARMCycVIRQ Interrupt Support
 *
 * @ingroup RTEMSBSPsARMCycV
 *
 * @ingroup bsp_interrupt
 *
 * @brief Intel Cyclone V Interrupt Support.
 *
 * @{
 */

/* Use interrupt IDs as defined in alt_interrupt_common.h */
#define BSP_INTERRUPT_VECTOR_MIN ALT_INT_INTERRUPT_SGI0
#define BSP_INTERRUPT_VECTOR_MAX ALT_INT_INTERRUPT_RAM_ECC_UNCORRECTED_IRQ

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_ALTERA_CYCLONE_V_IRQ_H */
