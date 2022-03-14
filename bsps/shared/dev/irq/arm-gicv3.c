/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 On-Line Applications Research Corporation (OAR)
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

#include <dev/irq/arm-gicv3.h>

#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/start.h>

void bsp_interrupt_dispatch(void)
{
  uint32_t icciar = READ_SR(ICC_IAR1);
  rtems_vector_number vector = GIC_CPUIF_ICCIAR_ACKINTID_GET(icciar);
  rtems_vector_number spurious = 1023;

  if (vector != spurious) {
    arm_interrupt_handler_dispatch(vector);

    WRITE_SR(ICC_EOIR1, icciar);
  }
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  gicv3_get_attributes(vector, attributes);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);

  if (vector <= ARM_GIC_IRQ_PPI_LAST) {
    *pending = gicv3_sgi_ppi_is_pending(vector, _SMP_Get_current_processor());
  } else {
    volatile gic_dist *dist = ARM_GIC_DIST;

    *pending = gic_id_is_pending(dist, vector);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector <= ARM_GIC_IRQ_SGI_LAST) {
    arm_gic_trigger_sgi(vector, 1U << _SMP_Get_current_processor());
  } else if (vector <= ARM_GIC_IRQ_PPI_LAST) {
    gicv3_ppi_set_pending(vector, _SMP_Get_current_processor());
  } else {
    volatile gic_dist *dist = ARM_GIC_DIST;

    gic_id_set_pending(dist, vector);
  }

  return RTEMS_SUCCESSFUL;
}

#if defined(RTEMS_SMP)
rtems_status_code bsp_interrupt_raise_on(
  rtems_vector_number vector,
  uint32_t            cpu_index
)
{
  if (vector >= 16) {
    return RTEMS_UNSATISFIED;
  }

  arm_gic_trigger_sgi(vector, 1U << cpu_index);
  return RTEMS_SUCCESSFUL;
}
#endif

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector <= ARM_GIC_IRQ_SGI_LAST) {
    return RTEMS_UNSATISFIED;
  }

  if ( vector <= ARM_GIC_IRQ_PPI_LAST ) {
    gicv3_ppi_clear_pending(vector, _SMP_Get_current_processor());
  } else {
    volatile gic_dist *dist = ARM_GIC_DIST;

    gic_id_clear_pending(dist, vector);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);

  if ( vector <= ARM_GIC_IRQ_PPI_LAST ) {
    *enabled = gicv3_sgi_ppi_is_enabled(vector, _SMP_Get_current_processor());
  } else {
    volatile gic_dist *dist = ARM_GIC_DIST;

    *enabled = gic_id_is_enabled(dist, vector);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector > ARM_GIC_IRQ_PPI_LAST) {
    volatile gic_dist *dist = ARM_GIC_DIST;

    gic_id_enable(dist, vector);
  } else {
    gicv3_sgi_ppi_enable(vector, _SMP_Get_current_processor());
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector > ARM_GIC_IRQ_PPI_LAST) {
    volatile gic_dist *dist = ARM_GIC_DIST;

    gic_id_disable(dist, vector);
  } else {
    gicv3_sgi_ppi_disable(vector, _SMP_Get_current_processor());
  }

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_facility_initialize(void)
{
  arm_interrupt_facility_set_exception_handler();
  gicv3_init_dist(ARM_GIC_DIST);
  gicv3_init_cpu_interface(_SMP_Get_current_processor());
}

#ifdef RTEMS_SMP
BSP_START_TEXT_SECTION void arm_gic_irq_initialize_secondary_cpu(void)
{
  volatile gic_dist *dist = ARM_GIC_DIST;

  while ((dist->icddcr & GIC_DIST_ICDDCR_ENABLE) == 0) {
    /* Wait */
  }

  gicv3_init_cpu_interface(_SMP_Get_current_processor());
}
#endif

rtems_status_code arm_gic_irq_set_priority(
  rtems_vector_number vector,
  uint8_t priority
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    if (vector >= 32) {
      volatile gic_dist *dist = ARM_GIC_DIST;
      gic_id_set_priority(dist, vector, priority);
    } else {
      gicv3_sgi_ppi_set_priority(
        vector,
        priority,
        _SMP_Get_current_processor()
      );
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_status_code arm_gic_irq_get_priority(
  rtems_vector_number vector,
  uint8_t *priority
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    if (vector >= 32) {
      volatile gic_dist *dist = ARM_GIC_DIST;
      *priority = gic_id_get_priority(dist, vector);
    } else {
      *priority = gicv3_sgi_ppi_get_priority(
        vector,
        _SMP_Get_current_processor()
      );
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_status_code bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
)
{
  volatile gic_dist *dist = ARM_GIC_DIST;
  uint8_t targets = (uint8_t) _Processor_mask_To_uint32_t(affinity, 0);

  if ( vector <= ARM_GIC_IRQ_PPI_LAST ) {
    return RTEMS_UNSATISFIED;
  }

  gic_id_set_targets(dist, vector, targets);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
)
{
  volatile gic_dist *dist = ARM_GIC_DIST;
  uint8_t targets;

  if ( vector <= ARM_GIC_IRQ_PPI_LAST ) {
    return RTEMS_UNSATISFIED;
  }

  targets = gic_id_get_targets(dist, vector);
  _Processor_mask_From_uint32_t(affinity, targets, 0);
  return RTEMS_SUCCESSFUL;
}

void arm_gic_trigger_sgi(rtems_vector_number vector, uint32_t targets)
{
  gicv3_trigger_sgi(vector, targets);
}

uint32_t arm_gic_irq_processor_count(void)
{
  volatile gic_dist *dist = ARM_GIC_DIST;
  uint32_t cpu_count;

  if ((dist->icddcr & GIC_DIST_ICDDCR_ARE_S) == 0) {
    cpu_count = GIC_DIST_ICDICTR_CPU_NUMBER_GET(dist->icdictr) + 1;
  } else {
    int i;

    /* Assume that an interrupt export port exists */
    cpu_count = 0;

    for (i = 0; i < CPU_MAXIMUM_PROCESSORS; ++i) {
      volatile gic_redist *redist = gicv3_get_redist(i);

      if ((redist->icrtyper & GIC_REDIST_ICRTYPER_LAST) != 0) {
        break;
      }

      ++cpu_count;
    }
  }

  return cpu_count;
}
