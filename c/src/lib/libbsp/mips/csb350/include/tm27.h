/**
 *  @file
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <bsp/irq.h>

/*
 *  Define the interrupt mechanism for Time Test 27
 */

int assert_sw_irw(uint32_t irqnum);
int negate_sw_irw(uint32_t irqnum);

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) \
   rtems_interrupt_handler_install( \
      AU1X00_IRQ_SW0, "benchmark", 0, (rtems_interrupt_handler)handler, NULL );

#define Cause_tm27_intr() \
  do { \
     assert_sw_irq(0); \
  } while(0)

#define Clear_tm27_intr() \
  do { \
     negate_sw_irq(0); \
  } while(0)

#define Lower_tm27_intr() \
  do { \
     continue;\
  } while(0)

#endif
