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
 */

/* use a VMEbus interrupt */

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( handler ) \
  { \
    set_vector( (handler), 0x50, 1 );  \
    (*(volatile uint32_t*)0x0d800024) = 0x50; /* set IVECT reg */ \
    (*(volatile uint8_t*)0x0d00000c) = 0x40; /* set VIE reg */ \
  }

#define Cause_tm27_intr()  \
  (*(volatile uint8_t*)0x0d000003) = 0x0f  /* set VINT */

#define Clear_tm27_intr()  /* no operation necessary */

#define Lower_tm27_intr() /* empty */

#endif
