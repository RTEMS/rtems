/*
 *  Clock Tick Device Driver
 *
 *  This routine initializes the Real Time Clock Counter Timer which is
 *  part of the MEC on the ERC32 CPU.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#include <bsp.h>
#include <bspopts.h>

#if SIMSPARC_FAST_IDLE==1
#define CLOCK_DRIVER_USE_FAST_IDLE
#endif

/*
 *  The Real Time Clock Counter Timer uses this trap type.
 */

#define CLOCK_VECTOR ERC32_TRAP_TYPE( ERC32_INTERRUPT_REAL_TIME_CLOCK )

#define Clock_driver_support_at_tick()

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    _old = set_vector( _new, CLOCK_VECTOR, 1 ); \
  } while(0)

extern int CLOCK_SPEED;

uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
  uint32_t clicks;
  uint32_t usecs;

  clicks = ERC32_MEC.Real_Time_Clock_Counter;

  if ( ERC32_Is_interrupt_pending( ERC32_INTERRUPT_REAL_TIME_CLOCK ) ) {
    clicks = ERC32_MEC.Real_Time_Clock_Counter;
    usecs = (2*rtems_configuration_get_microseconds_per_tick() - clicks);
  } else {
    usecs = (rtems_configuration_get_microseconds_per_tick() - clicks);
  }
  return usecs * 1000;
}

#define Clock_driver_nanoseconds_since_last_tick \
  bsp_clock_nanoseconds_since_last_tick

#define Clock_driver_support_initialize_hardware() \
  do { \
    /* approximately 1 us per countdown */ \
    ERC32_MEC.Real_Time_Clock_Scalar  = CLOCK_SPEED - 1; \
    ERC32_MEC.Real_Time_Clock_Counter = \
      rtems_configuration_get_microseconds_per_tick(); \
    \
    ERC32_MEC_Set_Real_Time_Clock_Timer_Control( \
      ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING | \
    	ERC32_MEC_TIMER_COUNTER_LOAD_SCALER | \
	    ERC32_MEC_TIMER_COUNTER_LOAD_COUNTER \
    ); \
    \
    ERC32_MEC_Set_Real_Time_Clock_Timer_Control( \
      ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING | \
	    ERC32_MEC_TIMER_COUNTER_RELOAD_AT_ZERO \
    ); \
  } while (0)

#define Clock_driver_support_shutdown_hardware() \
  do { \
    ERC32_Mask_interrupt( ERC32_INTERRUPT_REAL_TIME_CLOCK ); \
     \
    ERC32_MEC_Set_Real_Time_Clock_Timer_Control( \
      ERC32_MEC_TIMER_COUNTER_DISABLE_COUNTING \
    ); \
  } while (0)

#include "../../../shared/clockdrv_shell.h"

