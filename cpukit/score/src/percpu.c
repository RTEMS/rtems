/**
 * @file
 *
 * @brief Allocate and Initialize Per CPU Structures
 * @ingroup PerCPU
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/percpu.h>

#if defined(RTEMS_SMP)
  void _Per_CPU_Change_state(
    Per_CPU_Control *per_cpu,
    Per_CPU_State new_state
  )
  {
    per_cpu->state = new_state;
    _CPU_SMP_Processor_event_broadcast();
  }

  void _Per_CPU_Wait_for_state(
    const Per_CPU_Control *per_cpu,
    Per_CPU_State desired_state
  )
  {
    while ( per_cpu->state != desired_state ) {
      _CPU_SMP_Processor_event_receive();
    }
  }
#else
  /*
   * On single core systems, we can efficiently directly access a single
   * statically allocated per cpu structure.  And the fields are initialized
   * as individual elements just like it has always been done.
   */
  Per_CPU_Control_envelope _Per_CPU_Information[1];
#endif
