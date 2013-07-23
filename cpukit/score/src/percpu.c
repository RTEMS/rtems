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

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/thread.h>
#include <rtems/score/percpu.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>
#include <string.h>

#if defined(RTEMS_SMP)

  #include <rtems/score/smp.h>
  #include <rtems/bspsmp.h>

  void _SMP_Handler_initialize(void)
  {
    uint32_t max_cpus = rtems_configuration_get_maximum_processors();
    uint32_t cpu;

    /*
     *  Initialize per cpu pointer table
     */
    _Per_CPU_Information_p[0] = &_Per_CPU_Information[0];
    for ( cpu = 1 ; cpu < max_cpus; ++cpu ) {

      Per_CPU_Control *p = &_Per_CPU_Information[cpu];

      _Per_CPU_Information_p[cpu] = p;

#if CPU_ALLOCATE_INTERRUPT_STACK == TRUE
      {
        size_t size = rtems_configuration_get_interrupt_stack_size();
        uintptr_t ptr;

        p->interrupt_stack_low = _Workspace_Allocate_or_fatal_error( size );

        ptr = (uintptr_t) _Addresses_Add_offset( p->interrupt_stack_low, size );
        ptr &= ~(CPU_STACK_ALIGNMENT - 1);
        p->interrupt_stack_high = (void *)ptr;
      }
#endif
    }

    /*
     * Discover and initialize the secondary cores in an SMP system.
     */
    max_cpus = bsp_smp_initialize( max_cpus );

    _SMP_Processor_count = max_cpus;

    for ( cpu = 1 ; cpu < max_cpus; ++cpu ) {
      _Per_CPU_Wait_for_state(
        &_Per_CPU_Information[ cpu ],
        PER_CPU_STATE_READY_TO_BEGIN_MULTITASKING
      );
    }
  }

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
  Per_CPU_Control _Per_CPU_Information[1];
#endif
