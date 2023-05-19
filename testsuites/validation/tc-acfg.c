/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValAcfg
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <rtems/confdefs.h>
#include <rtems/score/heap.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup AcfgValAcfg spec:/acfg/val/acfg
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the effect of application configuration options.
 *
 * This test case performs the following actions:
 *
 * - Check the effect of application configuration options.  In addition, this
 *   test case validates the effect of CONFIGURE_INIT.  The test case includes
 *   rtems/confdefs.h without defining CONFIGURE_INIT before the include.  If
 *   this header would define configuration data structures, then linking the
 *   test suite executable would result in multiple definition errors.  This
 *   header is included in the test suite runner translation unit while
 *   CONFIGURE_INIT is defined before the include.  If this would not result in
 *   the definition of application defined configuration data structures, then
 *   the checks below for non-default settings would fail.
 *
 *   - Check the default value CONFIGURE_IDLE_TASK_BODY where the optional
 *     BSP-provided default value is enabled.
 *
 *   - Check the configured CONFIGURE_INIT_TASK_ARGUMENTS.  This validates also
 *     the effect of CONFIGURE_INIT_TASK_ENTRY_POINT and
 *     CONFIGURE_RTEMS_INIT_TASKS_TABLE.
 *
 *   - Check the configured CONFIGURE_INIT_TASK_INITIAL_MODES.
 *
 *   - Check the configured CONFIGURE_INIT_TASK_NAME.
 *
 *   - Check the configured CONFIGURE_INIT_TASK_PRIORITY.  A priority of zero
 *     can only be set for system tasks.  This validates also
 *     CONFIGURE_INIT_TASK_ATTRIBUTES.
 *
 *   - Check that the configured
 *     CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE value reduced the
 *     stack space size.
 *
 *   - Check the configured CONFIGURE_MAXIMUM_BARRIERS value.
 *
 *   - Check the configured CONFIGURE_MAXIMUM_USER_EXTENSIONS value.
 *
 *   - Check the configured CONFIGURE_MAXIMUM_MESSAGE_QUEUES value.
 *
 *   - Check the configured CONFIGURE_MAXIMUM_PARTITIONS value.
 *
 *   - Check the configured CONFIGURE_MAXIMUM_PERIODS value.
 *
 *   - Check the default CONFIGURE_MAXIMUM_PORTS value.
 *
 *   - Check the configured CONFIGURE_MAXIMUM_PROCESSORS value.
 *
 *   - Check the default CONFIGURE_MAXIMUM_REGIONS value.
 *
 *   - Check the configured CONFIGURE_MAXIMUM_SEMAPHORES value.
 *
 *   - Check the configured CONFIGURE_MAXIMUM_TASKS value.
 *
 *   - Check the configured CONFIGURE_MAXIMUM_TIMERS value.
 *
 *   - Check the configured CONFIGURE_MICROSECONDS_PER_TICK value in
 *     microseconds.
 *
 *   - Check the configured CONFIGURE_MICROSECONDS_PER_TICK value in
 *     milliseconds.
 *
 *   - Check the configured CONFIGURE_MICROSECONDS_PER_TICK value in
 *     nanoseconds.
 *
 *   - Check the configured CONFIGURE_TASK_STACK_ALLOCATOR hook.  Using the
 *     test stack allocator validates also
 *     spec:/acfg/if/init-task-construct-storage-size, since the
 *     test_task_stack_allocate() allocate handler only supports
 *     CONFIGURE_MAXIMUM_TASKS minus one stacks and the validation test for
 *     spec:/rtems/task/req/create-errors creates for some pre-condition
 *     variants all tasks until RTEMS_TOO_MANY is returned.  In addition,
 *     test_task_stack_allocate() checks that the allocation size is greater
 *     than or equal to TEST_MINIMUM_STACK_SIZE which validates
 *     CONFIGURE_MINIMUM_TASK_STACK_SIZE.
 *
 *   - Check the configured CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE
 *     value.
 *
 *   - Check the configured CONFIGURE_TASK_STACK_DEALLOCATOR hook.
 *
 *   - Check the configured CONFIGURE_TASK_STACK_ALLOCATOR_FOR_IDLE hook.
 *
 *   - Check the configured CONFIGURE_IDLE_TASK_STACK_SIZE value.
 *
 *   - Check the configured CONFIGURE_INTERRUPT_STACK_SIZE value.
 *
 *   - Check the configured CONFIGURE_TICKS_PER_TIMESLICE value.
 *
 * @{
 */

/**
 * @brief Check the effect of application configuration options.  In addition,
 *   this test case validates the effect of CONFIGURE_INIT.  The test case
 *   includes rtems/confdefs.h without defining CONFIGURE_INIT before the
 *   include.  If this header would define configuration data structures, then
 *   linking the test suite executable would result in multiple definition
 *   errors.  This header is included in the test suite runner translation unit
 *   while CONFIGURE_INIT is defined before the include.  If this would not
 *   result in the definition of application defined configuration data
 *   structures, then the checks below for non-default settings would fail.
 */
static void AcfgValAcfg_Action_0( void )
{
  rtems_status_code sc;
  rtems_id          id;

  /*
   * Check the default value CONFIGURE_IDLE_TASK_BODY where the optional
   * BSP-provided default value is enabled.
   */
  T_eq_ptr(
    rtems_configuration_get_idle_task(),
  #if defined(BSP_IDLE_TASK_BODY)
    BSP_IDLE_TASK_BODY
  #else
    _CPU_Thread_Idle_body
  #endif
  );

  /*
   * Check the configured CONFIGURE_INIT_TASK_ARGUMENTS.  This validates also
   * the effect of CONFIGURE_INIT_TASK_ENTRY_POINT and
   * CONFIGURE_RTEMS_INIT_TASKS_TABLE.
   */
  T_eq_ulong( test_runner_argument, TEST_RUNNER_ARGUMENT );

  /*
   * Check the configured CONFIGURE_INIT_TASK_INITIAL_MODES.
   */
  T_eq_u32( test_runner_initial_modes, TEST_RUNNER_INITIAL_MODES );

  /*
   * Check the configured CONFIGURE_INIT_TASK_NAME.
   */
  sc = rtems_task_ident( TEST_RUNNER_NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
  T_rsc_success( sc );
  T_eq_u32( id, rtems_task_self() );

  /*
   * Check the configured CONFIGURE_INIT_TASK_PRIORITY.  A priority of zero can
   * only be set for system tasks.  This validates also
   * CONFIGURE_INIT_TASK_ATTRIBUTES.
   */
  T_eq_u32( test_runner_initial_priority, 0 );

  /*
   * Check that the configured
   * CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE value reduced the stack
   * space size.
   */
  T_eq_uptr(
    _Stack_Space_size,
    RTEMS_ALIGN_UP( HEAP_BLOCK_HEADER_SIZE, CPU_HEAP_ALIGNMENT )
  );

  /*
   * Check the configured CONFIGURE_MAXIMUM_BARRIERS value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_barriers(),
    TEST_MAXIMUM_BARRIERS
  );

  /*
   * Check the configured CONFIGURE_MAXIMUM_USER_EXTENSIONS value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_extensions(),
    TEST_MAXIMUM_USER_EXTENSIONS
  );

  /*
   * Check the configured CONFIGURE_MAXIMUM_MESSAGE_QUEUES value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_message_queues(),
    TEST_MAXIMUM_MESSAGE_QUEUES
  );

  /*
   * Check the configured CONFIGURE_MAXIMUM_PARTITIONS value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_partitions(),
    TEST_MAXIMUM_PARTITIONS
  );

  /*
   * Check the configured CONFIGURE_MAXIMUM_PERIODS value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_periods(),
    TEST_MAXIMUM_PERIODS
  );

  /*
   * Check the default CONFIGURE_MAXIMUM_PORTS value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_ports(),
    0
  );

  /*
   * Check the configured CONFIGURE_MAXIMUM_PROCESSORS value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_processors(),
  #if defined(RTEMS_SMP)
    5
  #else
    1
  #endif
  );

  /*
   * Check the default CONFIGURE_MAXIMUM_REGIONS value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_regions(),
    0
  );

  /*
   * Check the configured CONFIGURE_MAXIMUM_SEMAPHORES value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_semaphores(),
    TEST_MAXIMUM_SEMAPHORES
  );

  /*
   * Check the configured CONFIGURE_MAXIMUM_TASKS value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_tasks(),
    TEST_MAXIMUM_TASKS
  );

  /*
   * Check the configured CONFIGURE_MAXIMUM_TIMERS value.
   */
  T_eq_u32(
    rtems_configuration_get_maximum_timers(),
    TEST_MAXIMUM_TIMERS
  );

  /*
   * Check the configured CONFIGURE_MICROSECONDS_PER_TICK value in
   * microseconds.
   */
  T_eq_u32(
    rtems_configuration_get_microseconds_per_tick(),
    TEST_MICROSECONDS_PER_TICK
  );

  /*
   * Check the configured CONFIGURE_MICROSECONDS_PER_TICK value in
   * milliseconds.
   */
  T_eq_u32(
    rtems_configuration_get_milliseconds_per_tick(),
    TEST_MICROSECONDS_PER_TICK / 1000
  );

  /*
   * Check the configured CONFIGURE_MICROSECONDS_PER_TICK value in nanoseconds.
   */
  T_eq_u32(
    rtems_configuration_get_nanoseconds_per_tick(),
    TEST_MICROSECONDS_PER_TICK * 1000
  );

  /*
   * Check the configured CONFIGURE_TASK_STACK_ALLOCATOR hook.  Using the test
   * stack allocator validates also
   * spec:/acfg/if/init-task-construct-storage-size, since the
   * test_task_stack_allocate() allocate handler only supports
   * CONFIGURE_MAXIMUM_TASKS minus one stacks and the validation test for
   * spec:/rtems/task/req/create-errors creates for some pre-condition variants
   * all tasks until RTEMS_TOO_MANY is returned.  In addition,
   * test_task_stack_allocate() checks that the allocation size is greater than
   * or equal to TEST_MINIMUM_STACK_SIZE which validates
   * CONFIGURE_MINIMUM_TASK_STACK_SIZE.
   */
  T_eq_ptr(
    rtems_configuration_get_stack_allocate_hook(),
    test_task_stack_allocate
  );

  /*
   * Check the configured CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE
   * value.
   */
  T_true( rtems_configuration_get_stack_allocator_avoids_work_space() );

  /*
   * Check the configured CONFIGURE_TASK_STACK_DEALLOCATOR hook.
   */
  T_eq_ptr(
    rtems_configuration_get_stack_free_hook(),
    test_task_stack_deallocate
  );

  /*
   * Check the configured CONFIGURE_TASK_STACK_ALLOCATOR_FOR_IDLE hook.
   */
  T_eq_ptr(
    rtems_configuration_get_stack_allocate_for_idle_hook(),
    test_idle_task_stack_allocate
  );

  /*
   * Check the configured CONFIGURE_IDLE_TASK_STACK_SIZE value.
   */
  T_eq_sz(
    rtems_configuration_get_idle_task_stack_size(),
    TEST_IDLE_STACK_SIZE
  );

  /*
   * Check the configured CONFIGURE_INTERRUPT_STACK_SIZE value.
   */
  T_eq_sz(
    rtems_configuration_get_interrupt_stack_size(),
    TEST_INTERRUPT_STACK_SIZE
  );

  /*
   * Check the configured CONFIGURE_TICKS_PER_TIMESLICE value.
   */
  T_eq_u32(
    rtems_configuration_get_ticks_per_timeslice(),
    TEST_TICKS_PER_TIMESLICE
  );
}

/**
 * @fn void T_case_body_AcfgValAcfg( void )
 */
T_TEST_CASE( AcfgValAcfg )
{
  AcfgValAcfg_Action_0();
}

/** @} */
