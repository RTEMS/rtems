/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScorePerCPU
 *
 * @brief This source file contains the static assertions for defines used in
 *   assembler files.
 */

/*
 * Copyright (C) 2012, 2016 embedded brains GmbH & Co. KG
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

#include <rtems/score/cpu.h>

#define _RTEMS_PERCPU_DEFINE_OFFSETS
#include <rtems/score/percpu.h>

#define PER_CPU_IS_POWER_OF_TWO( value ) \
  ( ( value ) > 1 && ( ( ( value ) - 1 ) & ( value ) ) == 0 )

/*
 * The minimum alignment of two is due to the Heap Handler which uses the
 * HEAP_PREV_BLOCK_USED flag to indicate that the previous block is used.
 */

RTEMS_STATIC_ASSERT(
  CPU_ALIGNMENT >= 2 && PER_CPU_IS_POWER_OF_TWO( CPU_ALIGNMENT ),
  CPU_ALIGNMENT
);

RTEMS_STATIC_ASSERT(
  CPU_HEAP_ALIGNMENT >= 2 && PER_CPU_IS_POWER_OF_TWO( CPU_HEAP_ALIGNMENT ),
  CPU_HEAP_ALIGNMENT_0
);

RTEMS_STATIC_ASSERT(
  CPU_HEAP_ALIGNMENT >= CPU_ALIGNMENT,
  CPU_HEAP_ALIGNMENT_1
);

RTEMS_STATIC_ASSERT(
  CPU_STACK_ALIGNMENT >= CPU_HEAP_ALIGNMENT &&
    PER_CPU_IS_POWER_OF_TWO( CPU_STACK_ALIGNMENT ),
  CPU_STACK_ALIGNMENT
);

RTEMS_STATIC_ASSERT(
  PER_CPU_IS_POWER_OF_TWO( CPU_STACK_MINIMUM_SIZE ),
  CPU_STACK_MINIMUM_SIZE
);

RTEMS_STATIC_ASSERT(
  sizeof(void *) == CPU_SIZEOF_POINTER,
  CPU_SIZEOF_POINTER
);

#if defined( __SIZEOF_POINTER__ )
  RTEMS_STATIC_ASSERT(
    CPU_SIZEOF_POINTER == __SIZEOF_POINTER__,
    __SIZEOF_POINTER__
  );
#endif

#if CPU_PER_CPU_CONTROL_SIZE > 0
  RTEMS_STATIC_ASSERT(
    sizeof( CPU_Per_CPU_control ) == CPU_PER_CPU_CONTROL_SIZE,
    CPU_PER_CPU_CONTROL_SIZE
  );
#endif

#if defined( RTEMS_SMP )
  RTEMS_STATIC_ASSERT(
    sizeof( Per_CPU_Control_envelope ) == PER_CPU_CONTROL_SIZE,
    PER_CPU_CONTROL_SIZE
  );
#endif

RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, isr_nest_level) == PER_CPU_ISR_NEST_LEVEL,
  PER_CPU_ISR_NEST_LEVEL
);

RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, isr_dispatch_disable)
    == PER_CPU_ISR_DISPATCH_DISABLE,
  PER_CPU_ISR_DISPATCH_DISABLE
);

RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, thread_dispatch_disable_level)
    == PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL,
  PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL
);

RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, executing) == PER_CPU_OFFSET_EXECUTING,
  PER_CPU_OFFSET_EXECUTING
);

RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, heir) == PER_CPU_OFFSET_HEIR,
  PER_CPU_OFFSET_HEIR
);

RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, dispatch_necessary) == PER_CPU_DISPATCH_NEEDED,
  PER_CPU_DISPATCH_NEEDED
);

#if defined(RTEMS_SMP)
RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, Interrupt_frame) == PER_CPU_INTERRUPT_FRAME_AREA,
  PER_CPU_INTERRUPT_FRAME_AREA
);

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Interrupt_frame ) == CPU_INTERRUPT_FRAME_SIZE,
  CPU_INTERRUPT_FRAME_SIZE
);
#endif

RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, interrupt_stack_low)
    == PER_CPU_INTERRUPT_STACK_LOW,
  PER_CPU_INTERRUPT_STACK_LOW
);

RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, interrupt_stack_high)
    == PER_CPU_INTERRUPT_STACK_HIGH,
  PER_CPU_INTERRUPT_STACK_HIGH
);
