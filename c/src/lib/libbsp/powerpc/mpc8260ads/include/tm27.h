/*
 *  tm27.h
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/*
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) \
   do { \
	static rtems_irq_connect_data scIrqData = { \
	  PPC_IRQ_SCALL, \
	  (rtems_irq_hdl) handler, \
	  NULL, \
	  NULL, \
	  NULL  \
	}; \
	BSP_install_rtems_irq_handler (&scIrqData); \
   } while(0)

#define Cause_tm27_intr() asm volatile ("sc")

#define Clear_tm27_intr()

#define Lower_tm27_intr()

#endif
