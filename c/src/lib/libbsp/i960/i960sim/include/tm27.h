/*
 *  tm27.h
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Following are for i960CA and are board independent
 *
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) /* set_vector( (handler), 6, 1 ) */

#define Cause_tm27_intr()  /* i960_cause_intr( 0x62 ) */

#define Clear_tm27_intr()  /* i960_clear_intr( 6 ) */

#define Lower_tm27_intr() /* empty */

#endif
