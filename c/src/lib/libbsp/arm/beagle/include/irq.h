/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief Basic BSP IRQ info.
 */

#ifndef LIBBSP_ARM_BEAGLE_IRQ_H
#define LIBBSP_ARM_BEAGLE_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#define BSP_INTERRUPT_VECTOR_MIN 0
#define BSP_INTERRUPT_VECTOR_MAX 127

#endif /* ASM */

#endif /* LIBBSP_ARM_BEAGLE_IRQ_H */
