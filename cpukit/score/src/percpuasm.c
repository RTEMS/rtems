/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/cpu.h>

#define _RTEMS_PERCPU_DEFINE_OFFSETS
#include <rtems/score/percpu.h>

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

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Per_CPU_control ) == CPU_PER_CPU_CONTROL_SIZE,
  CPU_PER_CPU_CONTROL_SIZE
);

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
  offsetof(Per_CPU_Control, thread_dispatch_disable_level)
    == PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL,
  PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL
);

RTEMS_STATIC_ASSERT(
  offsetof(Per_CPU_Control, dispatch_necessary) == PER_CPU_DISPATCH_NEEDED,
  PER_CPU_DISPATCH_NEEDED
);

#if CPU_ALLOCATE_INTERRUPT_STACK == TRUE \
  || CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE
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
#endif
