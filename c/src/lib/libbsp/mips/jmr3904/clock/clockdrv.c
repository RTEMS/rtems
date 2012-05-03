/**
 *  @file
 *  
 *  Instantiate the clock driver shell.
 *
 *  The TX3904 simulator in gdb counts instructions.
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

#define CLOCK_DRIVER_USE_FAST_IDLE

#define CLOCK_VECTOR TX3904_IRQ_TMR0

/*
 *  5000 clicks per tick ISR is HIGHLY arbitrary
 */

#define CLICKS 5000

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
    uint32_t   _clicks = CLICKS; \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_CCDR, 0x3 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_CPRA, _clicks ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_TISR, 0x00 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_ITMR, 0x8001 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_TCR,   0xC0 ); \
    *((volatile uint32_t*) 0xFFFFC01C) = 0x00000700; \
  } while(0)

#define Clock_driver_support_at_tick()

#define Clock_driver_support_shutdown_hardware()

#include "../../../shared/clockdrv_shell.h"
