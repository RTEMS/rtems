/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"

#include <rtems.h>
#include <rtems/score/smpimpl.h>

#include <bsp.h>
#include <bsp/bootcard.h>

#include <assert.h>
#include <stdlib.h>

const char rtems_test_name[] = "SMPFATAL 8";

/*
 * This test is a hack since there is no easy way to test this fatal error path
 * without BSP support.
 */

void bsp_start_on_secondary_processor(struct Per_CPU_Control *cpu_self)
{
  /* Provided to avoid multiple definitions of the CPU SMP support functions */
  (void) cpu_self;
  /* Block secondary cpus if they are running to avoid clobbering output */
  (void) _CPU_Thread_Idle_body( 0 );
}

#if QORIQ_THREAD_COUNT > 1
void qoriq_start_thread(Per_CPU_Control *cpu_self)
{
  /* Provided to avoid multiple definitions of the CPU SMP support functions */
  (void) cpu_self;
}
#endif

uint32_t _CPU_SMP_Initialize(void)
{
  return 2;
}

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  (void) cpu_index;

  return false;
}

void _CPU_SMP_Finalize_initialization(uint32_t cpu_count)
{
  (void) cpu_count;

  assert(0);
}

void _CPU_SMP_Prepare_start_multitasking(void)
{
  assert(0);
}

#if defined(RTEMS_PARAVIRT) \
  || (!defined(__leon__) && !defined(__PPC__) \
    && !defined(__arm__) && !defined(__riscv))
uint32_t _CPU_SMP_Get_current_processor(void)
{
  return 0;
}
#endif

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
  (void) target_processor_index;
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  assert(0);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  TEST_BEGIN();

  if (
    source == RTEMS_FATAL_SOURCE_SMP
      && !always_set_to_false
      && code == SMP_FATAL_START_OF_MANDATORY_PROCESSOR_FAILED
  ) {
    TEST_END();
  }
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_SIMPLE_SMP(a);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(a, rtems_build_name('S', 'I', 'M', 'P'))

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY)

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
