/*  Clock Driver for eZKit533.
 *  
 *  Instantiate the clock driver shell blackfin core timer.
 *
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */ 
 

#include <rtems.h>
#include <bsp.h>
#include <rtems/libio.h>

/*
 *  If defined, speed up the clock ticks while the idle task is running so
 *  time spent in the idle task is minimized.  This significantly reduces
 *  the wall time required to execute the RTEMS test suites.
 */

/* #define CLOCK_DRIVER_USE_FAST_IDLE */

#define CLOCK_VECTOR 6


#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    _old = (rtems_isr_entry) set_vector( _new, CLOCK_VECTOR, 1 ); \
  } while(0)

#define Clock_driver_support_initialize_hardware() \
      *((uint32_t*)TCNTL)     = TAUTORLD|TMREN|TMPWR;                                   \
      *((uint32_t*)TPERIOD) = CCLK / 1000000 * rtems_configuration_get_microseconds_per_tick(); \
      *((uint32_t*)TSCALE)  = 0;                                                        \
  do { \
  } while (0)

#define Clock_driver_support_at_tick()

#define Clock_driver_support_shutdown_hardware()

#include "../../../shared/clockdrv_shell.c"

