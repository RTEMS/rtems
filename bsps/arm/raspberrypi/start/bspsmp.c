/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief Raspberry pi SMP management functions provided to SuperCore
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

bool _CPU_SMP_Start_processor( uint32_t cpu_index )
{
  bool started;
  uint32_t cpu_index_self = _SMP_Get_current_processor();

  if (cpu_index != cpu_index_self) {

    BCM2835_REG(BCM2836_MAILBOX_3_WRITE_SET_BASE + 0x10 * cpu_index) = (uint32_t)_start;

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

uint32_t _CPU_SMP_Initialize(void)
{
  return 4;
}

void _CPU_SMP_Finalize_initialization( uint32_t cpu_count )
{
  /* Do nothing */
}

void _CPU_SMP_Prepare_start_multitasking( void )
{
  /* Do nothing */
}

void _CPU_SMP_Send_interrupt( uint32_t target_cpu_index )
{
  /* Generates IPI */
  BCM2835_REG(BCM2836_MAILBOX_3_WRITE_SET_BASE +
      0x10 * target_cpu_index) = 0x1;
}
