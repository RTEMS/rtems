/*
 *  tm27.h
 *
 * COPYRIGHT (c) 2010 by ECE Northeastern University.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/*
 *  Define the interrupt mechanism for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector(handler) \
{ \
  set_vector( handler, 0x06, 1 ); \
}

#define Cause_tm27_intr() asm volatile("raise 0x06;" : :);

#define Clear_tm27_intr() /* empty */

#define Lower_tm27_intr() /* empty */

#endif
