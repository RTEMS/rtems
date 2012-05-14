/**
 *  @file
 *  
 *  Instantiate the clock driver shell for the Mongoose-V's on-CPU timer.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp/irq.h>
#include <bsp.h>

#if defined(USE_TIMER2_FOR_CLOCK)
#define CLOCK_BASE   MONGOOSEV_TIMER2_BASE
#define CLOCK_VECTOR MONGOOSEV_IRQ_TIMER2
#else
#define CLOCK_BASE   MONGOOSEV_TIMER1_BASE
#define CLOCK_VECTOR MONGOOSEV_IRQ_TIMER1
#endif

/* reset Timeout (TO) bit */

#define Clock_driver_support_at_tick() \
  do { \
    MONGOOSEV_WRITE_REGISTER( CLOCK_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER, \
         (MONGOOSEV_TIMER_CONTROL_COUNTER_ENABLE | MONGOOSEV_TIMER_CONTROL_INTERRUPT_ENABLE)); \
  } while(0)

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    rtems_interrupt_handler_install( \
      CLOCK_VECTOR, \
      "clock", \
      0, \
      _new, \
      NULL \
    ); \
  } while(0)

#define Clock_driver_support_initialize_hardware() \
   do { \
    uint32_t   _clicks = CPU_CLOCK_RATE_MHZ * rtems_configuration_get_microseconds_per_tick(); \
    MONGOOSEV_WRITE_REGISTER( CLOCK_BASE, MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER, _clicks ); \
    Clock_driver_support_at_tick(); \
   } while(0)

#define Clock_driver_support_shutdown_hardware() \
  MONGOOSEV_WRITE_REGISTER( CLOCK_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER, 0 )

#include "../../../shared/clockdrv_shell.h"
