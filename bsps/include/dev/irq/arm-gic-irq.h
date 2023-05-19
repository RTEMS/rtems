/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup arm_gic
 *
 *  @brief ARM GIC IRQ
 */

/*
 * Copyright (C) 2013, 2019 embedded brains GmbH & Co. KG
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
#define ARM_GIC_IRQ_SGI_LAST 15

#define ARM_GIC_IRQ_PPI_LAST 31

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

rtems_status_code bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
);

rtems_status_code bsp_interrupt_get_affinity(
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

uint32_t arm_gic_irq_processor_count(void);

void arm_gic_irq_initialize_secondary_cpu(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_GIC_IRQ_H */
