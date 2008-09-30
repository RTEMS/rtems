/*
 *  Instantiate the clock driver shell.
 *
 *  Since there is no clock source on the simulator, we fake
 *  it with a special IDLE task.
 *
 *  $Id$
 */

#include <rtems.h>

#define CLOCK_VECTOR 0

#define Clock_driver_support_at_tick()

#define Clock_driver_support_install_isr( _new, _old ) \
  do { _old = 0; } while(0)

volatile bool clock_driver_enabled;

#define Clock_driver_support_initialize_hardware() \
  do { \
    clock_driver_enabled = true; \
  } while (0)

#define Clock_driver_support_shutdown_hardware() \
  do { \
    clock_driver_enabled = false; \
  } while (0)

#include "clockdrv_shell.h"

/*
 *  Since there is no interrupt on this simulator, let's just
 *  fake time passing.  This will not let preemption from an
 *  interrupt work but is enough for many tests.
 */
Thread clock_driver_sim_idle_body(
  uintptr_t   ignored
)
{
  for( ; ; ) {
    if ( clock_driver_enabled )
      rtems_clock_tick();
  }
  return 0;   /* to avoid warning */
}
