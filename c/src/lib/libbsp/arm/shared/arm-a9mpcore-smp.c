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

#include <assert.h>

#include <rtems/bspsmp.h>

#include <libcpu/arm-cp15.h>

#include <bsp/irq.h>

int bsp_smp_processor_id(void)
{
  return (int) arm_cortex_a9_get_multiprocessor_cpu_id();
}

static void ipi_handler(void *arg)
{
  rtems_smp_process_interrupt();
}

uint32_t bsp_smp_initialize(uint32_t configured_cpu_count)
{
  rtems_status_code sc;
  uint32_t max_cpu_count = arm_gic_irq_processor_count();
  uint32_t used_cpu_count = configured_cpu_count < max_cpu_count ?
    configured_cpu_count : max_cpu_count;

  sc = rtems_interrupt_handler_install(
    ARM_GIC_IRQ_SGI_0,
    "IPI",
    RTEMS_INTERRUPT_UNIQUE,
    ipi_handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  return used_cpu_count;
}

void bsp_smp_broadcast_interrupt(void)
{
  /*
   * FIXME: This broadcasts the interrupt also to processors not used by RTEMS.
   */
  rtems_status_code sc = arm_gic_irq_generate_software_irq(
    ARM_GIC_IRQ_SGI_0,
    ARM_GIC_IRQ_SOFTWARE_IRQ_TO_ALL_EXCEPT_SELF,
    0xff
  );
}

void bsp_smp_interrupt_cpu(int cpu)
{
  rtems_status_code sc = arm_gic_irq_generate_software_irq(
    ARM_GIC_IRQ_SGI_0,
    ARM_GIC_IRQ_SOFTWARE_IRQ_TO_ALL_IN_LIST,
    (uint8_t) (1U << cpu)
  );
}
