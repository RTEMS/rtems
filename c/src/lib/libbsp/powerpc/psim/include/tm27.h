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

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( _handler ) \
  set_vector( (_handler), PPC_IRQ_DECREMENTER, 1 )

#define Cause_tm27_intr()  \
  do { \
    uint32_t   _clicks = 1; \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

#define Clear_tm27_intr() \
  do { \
    uint32_t   _clicks = 0xffffffff; \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    uint32_t   _msr = 0; \
    _ISR_Set_level( 0 ); \
    asm volatile( "mfmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
    _msr |=  0x8002; \
    asm volatile( "mtmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
  } while (0)

#endif
