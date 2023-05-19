/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the constants used by the test suite
 *   configuration.
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

#ifndef _TS_CONFIG_H
#define _TS_CONFIG_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSTestSuitesValidation
 *
 * @{
 */

#define TEST_MICROSECONDS_PER_TICK 1000

#define TEST_RUNNER_NAME rtems_build_name( 'R', 'U', 'N', ' ' )

#define TEST_RUNNER_ARGUMENT 123456789

#define TEST_RUNNER_INITIAL_MODES RTEMS_NO_ASR

#define TEST_SCHEDULER_A_NAME rtems_build_name( 'A', ' ', ' ', ' ' )

#define TEST_SCHEDULER_B_NAME rtems_build_name( 'B', ' ', ' ', ' ' )

#define TEST_SCHEDULER_C_NAME rtems_build_name( 'C', ' ', ' ', ' ' )

#define TEST_SCHEDULER_D_NAME rtems_build_name( 'D', ' ', ' ', ' ' )

#if defined( __OPTIMIZE__ ) && !defined( RTEMS_GCOV_COVERAGE )
#define TEST_BASE_STACK_SIZE RTEMS_MINIMUM_STACK_SIZE
#else
#define TEST_BASE_STACK_SIZE ( 4 * RTEMS_MINIMUM_STACK_SIZE )
#endif

#define TEST_MAXIMUM_TLS_SIZE \
  RTEMS_ALIGN_UP( 64, RTEMS_TASK_STORAGE_ALIGNMENT )

#define TEST_MINIMUM_STACK_SIZE \
  ( TEST_BASE_STACK_SIZE + CPU_STACK_ALIGNMENT )

#define TEST_IDLE_STACK_SIZE \
  ( TEST_BASE_STACK_SIZE + 2 * CPU_STACK_ALIGNMENT )

#define TEST_INTERRUPT_STACK_SIZE \
  ( TEST_BASE_STACK_SIZE + 4 * CPU_INTERRUPT_STACK_ALIGNMENT )

#define TEST_MAXIMUM_BARRIERS 7

#define TEST_MAXIMUM_MESSAGE_QUEUES 3

#define TEST_MAXIMUM_PARTITIONS 4

#define TEST_MAXIMUM_PERIODS 2

#define TEST_MAXIMUM_SEMAPHORES 7

#define TEST_MAXIMUM_TASKS 32

#define TEST_MAXIMUM_TIMERS 10

#define TEST_MAXIMUM_USER_EXTENSIONS 5

/*
 * Use at least two so that the CPU time budget decrement in
 * _Scheduler_default_Tick() does not always result in a zero.
 */
#define TEST_TICKS_PER_TIMESLICE 2

void *test_task_stack_allocate( size_t size );

void test_task_stack_deallocate( void *stack );

void *test_idle_task_stack_allocate( uint32_t cpu_index, size_t *size );

extern rtems_task_argument test_runner_argument;

extern rtems_task_priority test_runner_initial_priority;

extern rtems_mode test_runner_initial_modes;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TS_CONFIG_H */
