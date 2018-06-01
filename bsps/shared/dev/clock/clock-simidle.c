/*
 *  Instantiate the clock driver shell.
 *
 *  Since there is no clock source on the simulator, we fake
 *  it with a special IDLE task.
 */

#include <rtems.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threaddispatch.h>

#define CLOCK_VECTOR 0

#define Clock_driver_support_initialize_hardware() \
  do { } while (0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "clockimpl.h"

/*
 * If this is defined, then the BSP has defined a delay of some sort so
 * time passage appears somewhat correctly. Otherwise, it runs extremely
 * fast with no delays.
 */
#ifndef BSP_CLOCK_DRIVER_DELAY
#define BSP_CLOCK_DRIVER_DELAY()
#endif

/*
 *  Since there is no interrupt on this simulator, let's just
 *  fake time passing.  This will not let preemption from an
 *  interrupt work but is enough for many tests.
 */
void *clock_driver_sim_idle_body(
  uintptr_t   ignored
)
{
  for( ; ; ) {
    Per_CPU_Control *cpu = _Thread_Dispatch_disable();
    _ISR_Nest_level++;
    rtems_clock_tick();
    _ISR_Nest_level--;
    _Thread_Dispatch_enable( cpu );
    BSP_CLOCK_DRIVER_DELAY();
  }
  return 0;   /* to avoid warning */
}
