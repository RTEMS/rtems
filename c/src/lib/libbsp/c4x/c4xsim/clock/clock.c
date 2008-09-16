/*  ckinit.c
 *
 *  This file provides a template for the clock device driver initialization.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <rtems/tic4x/c4xio.h>


#define CLOCK_VECTOR    9

#define Clock_driver_support_at_tick()

#define Clock_driver_support_install_isr( _new, _old ) \
  do { _old = 0; } while(0)

void Clock_driver_support_initialize_hardware()
{
  extern int _ClockFrequency;
  uint32_t Clock_counter_register_value;
  float tmp;
  int tmpi;

  tmpi = ((int) &_ClockFrequency) * 1000000;  /* ClockFrequency is in Mhz */
  tmp = (float) tmpi / 2.0;
  tmp = ((float) rtems_configuration_get_microseconds_per_tick() / 1000000.0) * (tmp);

  Clock_counter_register_value = (unsigned int) tmp;
#if 0
  Clock_counter_register_value =
      (uint32_t) ((float) rtems_configuration_get_microseconds_per_tick() /
       ((float)_ClockFrequency / 2.0)));
#endif
  c4x_timer_stop( C4X_TIMER_0 );
  c4x_timer_set_counter( C4X_TIMER_0, 0 );
  c4x_timer_set_period( C4X_TIMER_0, Clock_counter_register_value );
  c4x_timer_start( C4X_TIMER_0 );

#if defined(_C4x)
  c4x_set_iee( c4x_get_iie() | 0x1 );  /* should be ETINT0 */
#else
  c3x_set_ie( c3x_get_ie() | 0x100 );
#endif

  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );
}

#define Clock_driver_support_shutdown_hardware()

#include "../../../shared/clockdrv_shell.c"
