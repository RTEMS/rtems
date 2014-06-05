/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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
#include <bsp/linker-symbols.h>
#include <bsp/start.h>

#include <socal/alt_rstmgr.h>
#include <socal/alt_sysmgr.h>
#include <socal/hps.h>
#include <socal/socal.h>

static void bsp_inter_processor_interrupt(void *arg)
{
  _SMP_Inter_processor_interrupt_handler();
}

uint32_t _CPU_SMP_Initialize(void)
{
  uint32_t hardware_count = arm_gic_irq_processor_count();
  uint32_t linker_count = (uint32_t) bsp_processor_count;

  return hardware_count <= linker_count ? hardware_count : linker_count;
}

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  bool started;

  if (cpu_index == 1) {
    alt_write_word(
      ALT_SYSMGR_ROMCODE_ADDR + ALT_SYSMGR_ROMCODE_CPU1STARTADDR_OFST,
      ALT_SYSMGR_ROMCODE_CPU1STARTADDR_VALUE_SET((uint32_t) _start)
    );

    alt_clrbits_word(
      ALT_RSTMGR_MPUMODRST_ADDR,
      ALT_RSTMGR_MPUMODRST_CPU1_SET_MSK
    );

    /*
     * Wait for secondary processor to complete its basic initialization so
     * that we can enable the unified L2 cache.
     */
    started = _Per_CPU_State_wait_for_non_initial_state(cpu_index, 0);
  } else {
    started = false;
  }

  return started;
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

    /* Enable unified L2 cache */
    rtems_cache_enable_data();
  }
}

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
  arm_gic_irq_generate_software_irq(
    ARM_GIC_IRQ_SGI_0,
    ARM_GIC_IRQ_SOFTWARE_IRQ_TO_ALL_IN_LIST,
    (uint8_t) (1U << target_processor_index)
  );
}
