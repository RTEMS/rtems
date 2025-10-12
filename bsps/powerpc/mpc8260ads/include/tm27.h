/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * @file
 * @ingroup powerpc_mpc8260ads
 * @brief Implementations for interrupt mechanisms for Time Test 27
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <rtems/powerpc/powerpc.h>

/*
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) \
   do { \
	static rtems_irq_connect_data scIrqData = { \
	  .name = PPC_IRQ_SCALL, \
	  .hdl = handler, \
	  .handle = NULL, \
	  .on = NULL, \
	  .off = NULL, \
	  .isOn = NULL \
	}; \
	BSP_install_rtems_irq_handler (&scIrqData); \
   } while(0)

#define Cause_tm27_intr() __asm__ volatile ("sc")

#define Clear_tm27_intr() /* empty */

#define Lower_tm27_intr() /* empty */

#endif
