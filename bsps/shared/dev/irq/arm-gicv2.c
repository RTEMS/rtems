/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief This source file contains the implementation of ARM GICv2 support.
 */

/*
 * Copyright (C) 2013, 2021 embedded brains GmbH & Co. KG
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

#include <dev/irq/arm-gic.h>
#include <dev/irq/arm-gic-arch.h>

#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/start.h>

#define GIC_CPUIF ((volatile gic_cpuif *) BSP_ARM_GIC_CPUIF_BASE)

#define PRIORITY_DEFAULT 127

/*
 * The following variants
 *
 *  - GICv1 with Security Extensions,
 *  - GICv2 without Security Extensions, or
 *  - within Secure processor mode
 *
 * have the ability to assign group 0 or 1 to individual interrupts.  Group
 * 0 interrupts can be configured to raise an FIQ exception.  This enables
 * the use of NMIs with respect to RTEMS.
 *
 * BSPs can enable this feature with the BSP_ARM_GIC_ENABLE_FIQ_FOR_GROUP_0
 * define.  Use arm_gic_irq_set_group() to change the group of an
 * interrupt (default group is 1, if BSP_ARM_GIC_ENABLE_FIQ_FOR_GROUP_0 is
 * defined).
 */
#ifdef BSP_ARM_GIC_ENABLE_FIQ_FOR_GROUP_0
#define DIST_ICDDCR (GIC_DIST_ICDDCR_ENABLE_GRP_1 | GIC_DIST_ICDDCR_ENABLE)
#define CPUIF_ICCICR \
  (GIC_CPUIF_ICCICR_CBPR | GIC_CPUIF_ICCICR_FIQ_EN \
    | GIC_CPUIF_ICCICR_ACK_CTL | GIC_CPUIF_ICCICR_ENABLE_GRP_1 \
    | GIC_CPUIF_ICCICR_ENABLE)
#else
#define DIST_ICDDCR GIC_DIST_ICDDCR_ENABLE
#define CPUIF_ICCICR GIC_CPUIF_ICCICR_ENABLE
#endif

void bsp_interrupt_dispatch(void)
{
  volatile gic_cpuif *cpuif = GIC_CPUIF;
  uint32_t icciar = cpuif->icciar;
  rtems_vector_number vector = GIC_CPUIF_ICCIAR_ACKINTID_GET(icciar);
  rtems_vector_number spurious = 1023;

  if (vector != spurious) {
    arm_interrupt_handler_dispatch(vector);

    cpuif->icceoir = icciar;
  }
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  volatile gic_dist *dist = ARM_GIC_DIST;

  *pending = gic_id_is_pending(dist, vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector <= ARM_GIC_IRQ_SGI_LAST) {
    arm_gic_trigger_sgi(vector, 1U << _SMP_Get_current_processor());
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
  volatile gic_dist *dist = ARM_GIC_DIST;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector <= ARM_GIC_IRQ_SGI_LAST) {
    return RTEMS_UNSATISFIED;
  }

  gic_id_clear_pending(dist, vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  volatile gic_dist *dist = ARM_GIC_DIST;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);

  *enabled = gic_id_is_enabled(dist, vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  volatile gic_dist *dist = ARM_GIC_DIST;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  gic_id_enable(dist, vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  volatile gic_dist *dist = ARM_GIC_DIST;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  gic_id_disable(dist, vector);
  return RTEMS_SUCCESSFUL;
}

static inline uint32_t get_id_count(volatile gic_dist *dist)
{
  uint32_t id_count = GIC_DIST_ICDICTR_IT_LINES_NUMBER_GET(dist->icdictr);

  id_count = 32 * (id_count + 1);
  id_count = id_count <= 1020 ? id_count : 1020;

  return id_count;
}

static void enable_fiq(void)
{
#ifdef BSP_ARM_GIC_ENABLE_FIQ_FOR_GROUP_0
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  level &= ~ARM_PSR_F;
  rtems_interrupt_local_enable(level);
#endif
}

void bsp_interrupt_facility_initialize(void)
{
  volatile gic_cpuif *cpuif = GIC_CPUIF;
  volatile gic_dist *dist = ARM_GIC_DIST;
  uint32_t id_count = get_id_count(dist);
  uint32_t id;

  arm_interrupt_facility_set_exception_handler();

  for (id = 0; id < id_count; id += 32) {
#ifdef BSP_ARM_GIC_ENABLE_FIQ_FOR_GROUP_0
    dist->icdigr[id / 32] = 0xffffffff;
#endif
    dist->icdicer[id / 32] = 0xffffffff;
  }

  for (id = 0; id < id_count; ++id) {
    gic_id_set_priority(dist, id, PRIORITY_DEFAULT);
  }

  for (id = 32; id < id_count; ++id) {
    gic_id_set_targets(dist, id, 0x01);
  }

  cpuif->iccpmr = GIC_CPUIF_ICCPMR_PRIORITY(0xff);
  cpuif->iccbpr = GIC_CPUIF_ICCBPR_BINARY_POINT(0x0);
  cpuif->iccicr = CPUIF_ICCICR;

  dist->icddcr = GIC_DIST_ICDDCR_ENABLE_GRP_1 | GIC_DIST_ICDDCR_ENABLE;

  enable_fiq();
}

#ifdef RTEMS_SMP
BSP_START_TEXT_SECTION void arm_gic_irq_initialize_secondary_cpu(void)
{
  volatile gic_cpuif *cpuif = GIC_CPUIF;
  volatile gic_dist *dist = ARM_GIC_DIST;
  uint32_t id;

  while ((dist->icddcr & GIC_DIST_ICDDCR_ENABLE) == 0) {
    /* Wait */
  }

#ifdef BSP_ARM_GIC_ENABLE_FIQ_FOR_GROUP_0
  dist->icdigr[0] = 0xffffffff;
#endif

  /* Initialize priority of SGIs and PPIs */
  for (id = 0; id <= ARM_GIC_IRQ_PPI_LAST; ++id) {
    gic_id_set_priority(dist, id, PRIORITY_DEFAULT);
  }

  cpuif->iccpmr = GIC_CPUIF_ICCPMR_PRIORITY(0xff);
  cpuif->iccbpr = GIC_CPUIF_ICCBPR_BINARY_POINT(0x0);
  cpuif->iccicr = CPUIF_ICCICR;

  enable_fiq();
}
#endif

rtems_status_code arm_gic_irq_set_priority(
  rtems_vector_number vector,
  uint8_t priority
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    volatile gic_dist *dist = ARM_GIC_DIST;

    gic_id_set_priority(dist, vector, priority);
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
    volatile gic_dist *dist = ARM_GIC_DIST;

    *priority = gic_id_get_priority(dist, vector);
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_status_code arm_gic_irq_set_group(
  rtems_vector_number vector,
  gic_group group
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    volatile gic_dist *dist = ARM_GIC_DIST;

    gic_id_set_group(dist, vector, group);
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_status_code arm_gic_irq_get_group(
  rtems_vector_number vector,
  gic_group *group
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    volatile gic_dist *dist = ARM_GIC_DIST;

    *group = gic_id_get_group(dist, vector);
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
  rtems_interrupt_attributes attr;
  rtems_status_code sc;

  memset( &attr, 0, sizeof( attr ) );
  sc = bsp_interrupt_get_attributes( vector, &attr );

  if ( sc ) {
    return sc;
  }

  if ( vector <= ARM_GIC_IRQ_PPI_LAST ) {
    return RTEMS_UNSATISFIED;
  }

  if ( attr.can_set_affinity == 0 ) {
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
  rtems_interrupt_attributes attr;
  rtems_status_code sc;

  memset( &attr, 0, sizeof( attr ) );
  sc = bsp_interrupt_get_attributes( vector, &attr );

  if ( sc ) {
    return sc;
  }

  if ( vector <= ARM_GIC_IRQ_PPI_LAST ) {
    return RTEMS_UNSATISFIED;
  }

  targets = gic_id_get_targets(dist, vector);

  if ( attr.can_get_affinity == 0 ) {
    return RTEMS_UNSATISFIED;
  }

  _Processor_mask_From_uint32_t(affinity, targets, 0);
  return RTEMS_SUCCESSFUL;
}

void arm_gic_trigger_sgi(rtems_vector_number vector, uint32_t targets)
{
  volatile gic_dist *dist = ARM_GIC_DIST;

  dist->icdsgir = GIC_DIST_ICDSGIR_TARGET_LIST_FILTER(0)
    | GIC_DIST_ICDSGIR_CPU_TARGET_LIST(targets)
#ifdef BSP_ARM_GIC_ENABLE_FIQ_FOR_GROUP_0
    | GIC_DIST_ICDSGIR_NSATT
#endif
    | GIC_DIST_ICDSGIR_SGIINTID(vector);
}

uint32_t arm_gic_irq_processor_count(void)
{
  volatile gic_dist *dist = ARM_GIC_DIST;

  return GIC_DIST_ICDICTR_CPU_NUMBER_GET(dist->icdictr) + 1;
}
