/*
 *  Instantiate the clock driver shell for psim based
 *  on the decrementer register.
 *
 *  $Id$
 */

#include <rtems.h>

/*
 *  If defined, speed up the clock ticks while the idle task is running so
 *  time spent in the idle task is minimized.  This significantly reduces
 *  the wall time required to execute the RTEMS test suites.
 */

#define CLOCK_DRIVER_USE_FAST_IDLE

#define CLOCK_VECTOR PPC_IRQ_DECREMENTER

/*  On psim, each click of the decrementer register corresponds
 *  to 1 instruction.  By setting this to 100, we are indicating
 *  that we are assuming it can execute 100 instructions per
 *  microsecond.  This corresponds to sustaining 1 instruction
 *  per cycle at 100 Mhz.  Whether this is a good guess or not
 *  is anyone's guess.
 */

extern int PSIM_INSTRUCTIONS_PER_MICROSECOND;

unsigned int PPC_DECREMENTER_CLICKS;

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    _old = (rtems_isr_entry) set_vector( _new, CLOCK_VECTOR, 1 ); \
    PPC_DECREMENTER_CLICKS = (unsigned int)&PSIM_INSTRUCTIONS_PER_MICROSECOND; \
    PPC_DECREMENTER_CLICKS *= rtems_configuration_get_microseconds_per_tick(); \
    PPC_DECREMENTER_CLICKS = 1000; \
  } while(0)

#define Clock_driver_support_initialize_hardware() \
  do { \
    unsigned int _clicks = PPC_DECREMENTER_CLICKS; \
    PPC_Set_decrementer( _clicks ); \
  } while (0)

#define Clock_driver_support_at_tick() \
  Clock_driver_support_initialize_hardware()

#define Clock_driver_support_shutdown_hardware()

#include "../../../shared/clockdrv_shell.c"
