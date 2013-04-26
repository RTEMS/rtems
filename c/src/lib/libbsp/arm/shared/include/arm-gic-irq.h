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
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_GIC_IRQ_H
#define LIBBSP_ARM_SHARED_ARM_GIC_IRQ_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ARM_GIC_IRQ_SGI_0 0
#define ARM_GIC_IRQ_SGI_1 1
#define ARM_GIC_IRQ_SGI_2 2
#define ARM_GIC_IRQ_SGI_3 3
#define ARM_GIC_IRQ_SGI_5 5
#define ARM_GIC_IRQ_SGI_6 6
#define ARM_GIC_IRQ_SGI_7 7
#define ARM_GIC_IRQ_SGI_8 8
#define ARM_GIC_IRQ_SGI_9 9
#define ARM_GIC_IRQ_SGI_10 10 
#define ARM_GIC_IRQ_SGI_11 11
#define ARM_GIC_IRQ_SGI_12 12
#define ARM_GIC_IRQ_SGI_13 13
#define ARM_GIC_IRQ_SGI_14 14
#define ARM_GIC_IRQ_SGI_15 15

rtems_status_code arm_gic_irq_set_priority(
  rtems_vector_number vector,
  uint8_t priority
);

rtems_status_code arm_gic_irq_get_priority(
  rtems_vector_number vector,
  uint8_t *priority
);

typedef enum {
  ARM_GIC_IRQ_SOFTWARE_IRQ_TO_ALL_IN_LIST,
  ARM_GIC_IRQ_SOFTWARE_IRQ_TO_ALL_EXCEPT_SELF,
  ARM_GIC_IRQ_SOFTWARE_IRQ_TO_SELF
} arm_gic_irq_software_irq_target_filter;

rtems_status_code arm_gic_irq_generate_software_irq(
  rtems_vector_number vector,
  arm_gic_irq_software_irq_target_filter filter,
  uint8_t targets
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_GIC_IRQ_H */
