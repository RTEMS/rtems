/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief Raspberry pi secondary CPU and IPI HW initialization
 */

/*
 * Copyright (c) 2016 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Reuses some ideas from Rohini Kulkarni <krohini1593@gmail.com>
 * GSoC 2015 project and Altera Cyclone-V SMP code
 * by embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/smpimpl.h>

#include <bsp/start.h>
#include <bsp/raspberrypi.h>
#include <bsp.h>
#include <bsp/arm-cp15-start.h>
#include <libcpu/arm-cp15.h>
#include <rtems.h>
#include <bsp/irq-generic.h>
#include <assert.h>

void rpi_ipi_initialize(void)
{
  uint32_t cpu_index_self = _SMP_Get_current_processor();

  /*
   * Includes support only for mailbox 3 interrupt.
   * Further interrupt support has to be added. This will have to be integrated
   * with existing interrupt support for Raspberry Pi
   */

  /* reset mailbox 3 contents to zero */
  BCM2835_REG(BCM2836_MAILBOX_3_READ_CLEAR_BASE + 0x10 * cpu_index_self) = 0xffffffff;

  BCM2835_REG(BCM2836_MAILBOX_IRQ_CTRL(cpu_index_self)) =
    BCM2836_MAILBOX_IRQ_CTRL_MBOX3_IRQ;
}

void rpi_start_rtems_on_secondary_processor(void)
{
  uint32_t ctrl;

  ctrl = arm_cp15_start_setup_mmu_and_cache(
    0,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  rpi_ipi_initialize();

  arm_cp15_set_domain_access_control(
    ARM_CP15_DAC_DOMAIN(ARM_MMU_DEFAULT_CLIENT_DOMAIN, ARM_CP15_DAC_CLIENT)
  );

  /* FIXME: Sharing the translation table between processors is brittle */
  arm_cp15_set_translation_table_base(
    (uint32_t *) bsp_translation_table_base
  );

  arm_cp15_tlb_invalidate();

  ctrl |= ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M;
  ctrl &= ~ARM_CP15_CTRL_V;
  arm_cp15_set_control(ctrl);

  _SMP_Start_multitasking_on_secondary_processor(_Per_CPU_Get());
}
