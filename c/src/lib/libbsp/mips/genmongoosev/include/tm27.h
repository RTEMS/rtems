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

/*
 *  Define the interrupt mechanism for Time Test 27
 */

#include <bsp/irq.h>

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( handler ) \
   rtems_interrupt_handler_install( \
      MONGOOSEV_IRQ_SOFTWARE_1, "benchmark", 0, \
      (rtems_interrupt_handler)handler, NULL );

#define Cause_tm27_intr()   assertSoftwareInterrupt(0);

#define Clear_tm27_intr() /* empty */
#define Lower_tm27_intr() /* empty */

#endif
