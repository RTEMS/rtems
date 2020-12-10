/**
 *  @file
 *
 *  @ingroup arm_gic
 *
 *  @brief ARM GIC IRQ
 */

/*
 * Copyright (c) 2013, 2019 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_SHARED_ARM_GIC_IRQ_H
#define LIBBSP_ARM_SHARED_ARM_GIC_IRQ_H

#include <bsp.h>
#include <dev/irq/arm-gic.h>
#include <rtems/score/processormask.h>

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

#define ARM_GIC_DIST ((volatile gic_dist *) BSP_ARM_GIC_DIST_BASE)

rtems_status_code arm_gic_irq_set_priority(
  rtems_vector_number vector,
  uint8_t priority
);

rtems_status_code arm_gic_irq_get_priority(
  rtems_vector_number vector,
  uint8_t *priority
);

rtems_status_code arm_gic_irq_set_group(
  rtems_vector_number vector,
  gic_group group
);

rtems_status_code arm_gic_irq_get_group(
  rtems_vector_number vector,
  gic_group *group
);

void bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
);

void bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
);

void arm_gic_trigger_sgi(rtems_vector_number vector, uint32_t targets);

static inline rtems_status_code arm_gic_irq_generate_software_irq(
  rtems_vector_number vector,
  uint32_t targets
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (vector <= ARM_GIC_IRQ_SGI_15) {
    arm_gic_trigger_sgi(vector, targets);
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

/**
 * This architecture-specific function sets the exception vector for handling
 * IRQs.
 */
void arm_interrupt_facility_set_exception_handler(void);

/**
 * This architecture-specific function dispatches a triggered IRQ.
 *
 * @param[in] vector The vector on which the IRQ occurred.
 */
void arm_interrupt_handler_dispatch(rtems_vector_number vector);

uint32_t arm_gic_irq_processor_count(void);

void arm_gic_irq_initialize_secondary_cpu(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_GIC_IRQ_H */
