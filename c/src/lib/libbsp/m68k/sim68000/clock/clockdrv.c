/*
 *  Instantiate the clock driver shell.
 *
 *  $Id$
 */

#include <bsp.h>

#define CLOCK_VECTOR 0

#define Clock_driver_support_install_isr( _new, _old )  \
  do { _old = (rtems_isr_entry) set_vector( _new, CLOCK_VECTOR, 1 ); } while(0)


#define Clock_driver_support_initialize_hardware() \
  } while (0);

#define Clock_driver_support_at_tick() \
  Clock_driver_support_initialize_hardware()

#define Clock_driver_support_shutdown_hardware()

#include "../../../shared/clockdrv_shell.c"
