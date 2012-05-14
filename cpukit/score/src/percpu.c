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
#include <rtems/score/thread.h>
#include <rtems/score/percpu.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>
#include <string.h>

#if defined(RTEMS_SMP)

  #include <rtems/score/smp.h>

  void _SMP_Handler_initialize(void)
  {
    int         cpu;
    size_t      size;
    uintptr_t   ptr;

    /*
     *  Initialize per CPU structures.
     */
    size = (_SMP_Processor_count) * sizeof(Per_CPU_Control);
    memset( _Per_CPU_Information, '\0', size );

    /*
     *  Initialize per cpu pointer table
     */
    size = Configuration.interrupt_stack_size;
    _Per_CPU_Information_p[0] = &_Per_CPU_Information[0];
    for (cpu=1 ; cpu < rtems_configuration_smp_maximum_processors; cpu++ ) {

      Per_CPU_Control *p = &_Per_CPU_Information[cpu];

      _Per_CPU_Information_p[cpu] = p;

      p->interrupt_stack_low = _Workspace_Allocate_or_fatal_error( size );

      ptr = (uintptr_t) _Addresses_Add_offset( p->interrupt_stack_low, size );
      ptr &= ~(CPU_STACK_ALIGNMENT - 1);
      p->interrupt_stack_high = (void *)ptr;
      p->state = RTEMS_BSP_SMP_CPU_INITIAL_STATE;
      RTEMS_COMPILER_MEMORY_BARRIER();
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
