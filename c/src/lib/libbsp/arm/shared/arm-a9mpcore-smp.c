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

#include <assert.h>

#include <rtems/score/smpimpl.h>

#include <libcpu/arm-cp15.h>

#include <bsp/irq.h>

static void bsp_inter_processor_interrupt(void *arg)
{
  _SMP_Inter_processor_interrupt_handler();
}

uint32_t _CPU_SMP_Initialize(void)
{
  return arm_gic_irq_processor_count();
}

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  (void) cpu_index;

  /* Nothing to do */

  return true;
}

void _CPU_SMP_Finalize_initialization(uint32_t cpu_count)
{
  if (cpu_count > 0) {
    rtems_status_code sc;

    sc = rtems_interrupt_handler_install(
      ARM_GIC_IRQ_SGI_0,
      "IPI",
      RTEMS_INTERRUPT_UNIQUE,
      bsp_inter_processor_interrupt,
      NULL
    );
    assert(sc == RTEMS_SUCCESSFUL);
  }
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
  arm_gic_irq_generate_software_irq(
    ARM_GIC_IRQ_SGI_0,
    ARM_GIC_IRQ_SOFTWARE_IRQ_TO_ALL_IN_LIST,
    (uint8_t) (1U << target_processor_index)
  );
}
