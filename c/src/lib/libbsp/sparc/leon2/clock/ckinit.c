/*
 *  Clock Tick Device Driver
 *
 *  This routine initializes LEON timer 1 which used for the clock tick.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON BSP
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bspopts.h>

#if SIMSPARC_FAST_IDLE==1
#define CLOCK_DRIVER_USE_FAST_IDLE
#endif

/*
 *  The Real Time Clock Counter Timer uses this trap type.
 */

#define CLOCK_VECTOR LEON_TRAP_TYPE( LEON_INTERRUPT_TIMER1 )

#define Clock_driver_support_at_tick()

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    _old = set_vector( _new, CLOCK_VECTOR, 1 ); \
  } while(0)

extern int CLOCK_SPEED;

#define Clock_driver_support_initialize_hardware() \
  do { \
    LEON_REG.Timer_Reload_1 = rtems_configuration_get_microseconds_per_tick() - 1; \
    \
    LEON_REG.Timer_Control_1 = ( \
      LEON_REG_TIMER_COUNTER_ENABLE_COUNTING |  \
        LEON_REG_TIMER_COUNTER_RELOAD_AT_ZERO | \
        LEON_REG_TIMER_COUNTER_LOAD_COUNTER  \
    ); \
  } while (0)

#define Clock_driver_support_shutdown_hardware() \
  do { \
    LEON_Mask_interrupt( LEON_INTERRUPT_TIMER1 ); \
    LEON_REG.Timer_Control_1 = 0; \
  } while (0)

uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
  uint32_t clicks;
  uint32_t usecs;

  clicks = LEON_REG.Timer_Counter_1;

  if ( LEON_Is_interrupt_pending( LEON_INTERRUPT_TIMER1 ) ) {
    clicks = LEON_REG.Timer_Counter_1;
    usecs = (2*rtems_configuration_get_microseconds_per_tick() - clicks);
  } else {
    usecs = (rtems_configuration_get_microseconds_per_tick() - clicks);
  }
  return usecs * 1000;
}

#define Clock_driver_nanoseconds_since_last_tick bsp_clock_nanoseconds_since_last_tick

#include "../../../shared/clockdrv_shell.h"
