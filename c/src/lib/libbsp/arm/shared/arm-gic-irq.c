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

#include <bsp/arm-gic.h>

#include <rtems/score/armv4.h>

#include <libcpu/arm-cp15.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#define GIC_CPUIF ((volatile gic_cpuif *) BSP_ARM_GIC_CPUIF_BASE)

#define GIC_DIST ((volatile gic_dist *) BSP_ARM_GIC_DIST_BASE)

#define PRIORITY_DEFAULT 128

void bsp_interrupt_dispatch(void)
{
  volatile gic_cpuif *cpuif = GIC_CPUIF;
  uint32_t icciar = cpuif->icciar;
  rtems_vector_number vector = GIC_CPUIF_ICCIAR_ACKINTID_GET(icciar);
  rtems_vector_number spurious = 1023;

  if (vector != spurious) {
    uint32_t psr = _ARMV4_Status_irq_enable();

    bsp_interrupt_handler_dispatch(vector);

    _ARMV4_Status_restore(psr);

    cpuif->icceoir = icciar;
  }
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    volatile gic_dist *dist = GIC_DIST;

    gic_id_enable(dist, vector);
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    volatile gic_dist *dist = GIC_DIST;

    gic_id_disable(dist, vector);
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

static inline uint32_t get_id_count(volatile gic_dist *dist)
{
  uint32_t id_count = GIC_DIST_ICDICTR_IT_LINES_NUMBER_GET(dist->icdictr);

  id_count = 32 * (id_count + 1);
  id_count = id_count <= 1020 ? id_count : 1020;

  return id_count;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  volatile gic_cpuif *cpuif = GIC_CPUIF;
  volatile gic_dist *dist = GIC_DIST;
  uint32_t id_count = get_id_count(dist);
  uint32_t id;

  for (id = 0; id < id_count; ++id) {
    gic_id_set_priority(dist, id, PRIORITY_DEFAULT);
  }

  for (id = 32; id < id_count; ++id) {
    gic_id_set_targets(dist, id, 0x01);
  }

  cpuif->iccpmr = GIC_CPUIF_ICCPMR_PRIORITY(0xff);
  cpuif->iccbpr = GIC_CPUIF_ICCBPR_BINARY_POINT(0x0);
  cpuif->iccicr = GIC_CPUIF_ICCICR_ENABLE;

  dist->icddcr = GIC_DIST_ICDDCR_ENABLE;

  arm_cp15_set_exception_handler(
    ARM_EXCEPTION_IRQ,
    _ARMV4_Exception_interrupt,
    BSP_ARM_MMU_READ_WRITE
  );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code arm_gic_irq_set_priority(
  rtems_vector_number vector,
  uint8_t priority
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    volatile gic_dist *dist = GIC_DIST;

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
    volatile gic_dist *dist = GIC_DIST;

    *priority = gic_id_get_priority(dist, vector);
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_status_code arm_gic_irq_generate_software_irq(
  rtems_vector_number vector,
  arm_gic_irq_software_irq_target_filter filter,
  uint8_t targets
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (vector < 16) {
    volatile gic_dist *dist = GIC_DIST;

    dist->icdsgir = GIC_DIST_ICDSGIR_TARGET_LIST_FILTER(filter)
      | GIC_DIST_ICDSGIR_CPU_TARGET_LIST(targets)
      | GIC_DIST_ICDSGIR_SGIINTID(vector);
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}
