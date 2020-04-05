/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate Per-CPU Configuration Options
 *
 * Since the idle thread stack size (CONFIGURE_IDLE_TASK_STACK_SIZE) depends on
 * CONFIGURE_MINIMUM_TASK_STACK_SIZE, the POSIX-specific
 * CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE configuration option is also evaluated
 * in this header file for simplicity.
 *
 * This header file defines _CONFIGURE_MAXIMUM_PROCESSORS for use by other
 * configuration header files.
 */

/*
 * Copyright (C) 2018, 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_CONFDEFS_PERCPU_H
#define _RTEMS_CONFDEFS_PERCPU_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#include <rtems/confdefs/bsp.h>
#include <rtems/score/context.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Ensure that _CONFIGURE_MAXIMUM_PROCESSORS > 1 only in SMP configurations */
#if defined(CONFIGURE_MAXIMUM_PROCESSORS) && defined(RTEMS_SMP)
  #define _CONFIGURE_MAXIMUM_PROCESSORS CONFIGURE_MAXIMUM_PROCESSORS
#else
  #define _CONFIGURE_MAXIMUM_PROCESSORS 1
#endif

#ifdef RTEMS_SMP
  const uint32_t _SMP_Processor_configured_maximum =
    _CONFIGURE_MAXIMUM_PROCESSORS;

  Per_CPU_Control_envelope
    _Per_CPU_Information[ _CONFIGURE_MAXIMUM_PROCESSORS ];
#endif

/* Interrupt stack configuration */

#ifndef CONFIGURE_INTERRUPT_STACK_SIZE
  #ifdef BSP_INTERRUPT_STACK_SIZE
    #define CONFIGURE_INTERRUPT_STACK_SIZE BSP_INTERRUPT_STACK_SIZE
  #else
    #define CONFIGURE_INTERRUPT_STACK_SIZE CPU_STACK_MINIMUM_SIZE
  #endif
#endif

#if CONFIGURE_INTERRUPT_STACK_SIZE % CPU_INTERRUPT_STACK_ALIGNMENT != 0
  #error "CONFIGURE_INTERRUPT_STACK_SIZE fails to meet the CPU port interrupt stack alignment"
#endif

RTEMS_DEFINE_GLOBAL_SYMBOL(
  _ISR_Stack_size,
  CONFIGURE_INTERRUPT_STACK_SIZE
);

#define _CONFIGURE_INTERRUPT_STACK_AREA_SIZE \
  ( CONFIGURE_INTERRUPT_STACK_SIZE * _CONFIGURE_MAXIMUM_PROCESSORS )

char _ISR_Stack_area_begin[ _CONFIGURE_INTERRUPT_STACK_AREA_SIZE ]
RTEMS_ALIGNED( CPU_INTERRUPT_STACK_ALIGNMENT )
RTEMS_SECTION( ".rtemsstack.interrupt" );

RTEMS_DEFINE_GLOBAL_SYMBOL(
  _ISR_Stack_area_end,
  RTEMS_SYMBOL_NAME( _ISR_Stack_area_begin )
    + _CONFIGURE_INTERRUPT_STACK_AREA_SIZE
);

/* Thread stack size configuration */

#ifndef CONFIGURE_MINIMUM_TASK_STACK_SIZE
  #define CONFIGURE_MINIMUM_TASK_STACK_SIZE CPU_STACK_MINIMUM_SIZE
#endif

#ifndef CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE
  #define CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE \
    ( 2 * CONFIGURE_MINIMUM_TASK_STACK_SIZE )
#endif

/* Idle thread configuration */

#ifndef CONFIGURE_IDLE_TASK_STACK_SIZE
  #ifdef BSP_IDLE_TASK_STACK_SIZE
    #define CONFIGURE_IDLE_TASK_STACK_SIZE BSP_IDLE_TASK_STACK_SIZE
  #else
    #define CONFIGURE_IDLE_TASK_STACK_SIZE CONFIGURE_MINIMUM_TASK_STACK_SIZE
  #endif
#endif

#if CONFIGURE_IDLE_TASK_STACK_SIZE < CONFIGURE_MINIMUM_TASK_STACK_SIZE
  #error "CONFIGURE_IDLE_TASK_STACK_SIZE less than CONFIGURE_MINIMUM_TASK_STACK_SIZE"
#endif

const size_t _Thread_Idle_stack_size = CONFIGURE_IDLE_TASK_STACK_SIZE;

char _Thread_Idle_stacks[
  _CONFIGURE_MAXIMUM_PROCESSORS
    * ( CONFIGURE_IDLE_TASK_STACK_SIZE + CPU_IDLE_TASK_IS_FP * CONTEXT_FP_SIZE )
] RTEMS_ALIGNED( CPU_INTERRUPT_STACK_ALIGNMENT )
RTEMS_SECTION( ".rtemsstack.idle" );

#if defined(CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION) && \
  !defined(CONFIGURE_IDLE_TASK_BODY)
  #error "If you define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION, then you must define CONFIGURE_IDLE_TASK_BODY as well"
#endif

#if !defined(CONFIGURE_IDLE_TASK_BODY) && defined(BSP_IDLE_TASK_BODY)
  #define CONFIGURE_IDLE_TASK_BODY BSP_IDLE_TASK_BODY
#endif

#ifdef CONFIGURE_IDLE_TASK_BODY
  const Thread_Idle_body _Thread_Idle_body = CONFIGURE_IDLE_TASK_BODY;
#endif

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_PERCPU_H */
