/* SPDX-License-Identifier: BSD-2-Clause */

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
 * by embedded brains GmbH & Co. KG
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
    _ARM_Send_event();

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
