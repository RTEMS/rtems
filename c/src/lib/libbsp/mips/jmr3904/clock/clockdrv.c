/*
 *  Instantiate the clock driver shell.
 *
 *  Since there is no clock source on the simulator, all we do is
 *  make sure it will build.
 *
 *  $Id$
 */

#include <rtems.h>
#include <libcpu/tx3904.h>

#define CLOCK_VECTOR TX3904_IRQ_TMR0

#define Clock_driver_support_at_tick()

/* XXX */
#define CLICKS 10000
#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    unsigned32 _clicks = CLICKS; \
    _old = (rtems_isr_entry) set_vector( _new, CLOCK_VECTOR, 1 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_CCDR, 0x3 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_CPRA, _clicks ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_TISR, 0x00 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_ITMR, 0x8001 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_TCR,   0xC0 ); \
    *((volatile unsigned32 *) 0xFFFFC01C) = 0x00000700; \
  } while(0)


#define Clock_driver_support_initialize_hardware()

#define Clock_driver_support_shutdown_hardware()

#include "../../../shared/clockdrv_shell.c"
