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
 *  Stuff for Time Test 27
 */

#include <rtems/c4x/c4xio.h>
#define MUST_WAIT_FOR_INTERRUPT 0

/* XXX */
#define Install_tm27_vector( _handler ) \
  set_vector( (_handler), 0x20, 1 )

#define Cause_tm27_intr()  \
  do { \
     __asm__ volatile ( "trapu 0" ); \
  } while (0)

#define Clear_tm27_intr() \
  do { \
     ; \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    c4x_global_interrupts_enable(); \
  } while (0)

#endif
