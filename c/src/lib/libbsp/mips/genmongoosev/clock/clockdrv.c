/*
 *  Instantiate the clock driver shell.
 *
 *  The tx3904 simulator in gdb counts instructions.
 *
 *  $Id$
 */

#include <rtems.h>
#include <libcpu/mongoose-v.h>
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
  MONGOOSEV_WRITE_REGISTER( \
    CLOCK_BASE, \
    MONGOOSEV_TIMER_CONTROL_REGISTER, \
    (MONGOOSEV_TIMER_CONTROL_COUNTER_ENABLE | \
     MONGOOSEV_TIMER_CONTROL_INTERRUPT_ENABLE) \
  );

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    _old = set_vector( _new, CLOCK_VECTOR, 1 ); \
  } while(0)


extern int ClockRate;

#define CLICKS (unsigned32) &ClockRate
#define Clock_driver_support_initialize_hardware() \
  do { \
    unsigned32 _clicks; \
    _clicks = CLICKS * rtems_configuration_get_microseconds_per_tick(); \
    MONGOOSEV_WRITE_REGISTER( \
      CLOCK_BASE, \
      MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER, \
      _clicks \
    ); \
    Clock_driver_support_at_tick(); \
  } while(0)

#define Clock_driver_support_shutdown_hardware() \
  MONGOOSEV_WRITE_REGISTER( CLOCK_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER, 0 )

#include <clockdrv_shell.c>
