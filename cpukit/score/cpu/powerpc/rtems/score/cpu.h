/**
 * @file rtems/score/cpu.h
 */

/*
 * $Id$
 */
 
#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#include <rtems/score/powerpc.h>              /* pick up machine definitions */
#ifndef ASM
#include <rtems/score/types.h>
#endif

/* conditional compilation parameters */

/*
 *  Should the calls to _Thread_Enable_dispatch be inlined?
 *
 *  If TRUE, then they are inlined.
 *  If FALSE, then a subroutine call is made.
 *
 *  Basically this is an example of the classic trade-off of size
 *  versus speed.  Inlining the call (TRUE) typically increases the
 *  size of RTEMS while speeding up the enabling of dispatching.
 *  [NOTE: In general, the _Thread_Dispatch_disable_level will
 *  only be 0 or 1 unless you are in an interrupt handler and that
 *  interrupt handler invokes the executive.]  When not inlined
 *  something calls _Thread_Enable_dispatch which in turns calls
 *  _Thread_Dispatch.  If the enable dispatch is inlined, then
 *  one subroutine call is avoided entirely.]
 */

#define CPU_INLINE_ENABLE_DISPATCH       FALSE

/*
 *  Should the body of the search loops in _Thread_queue_Enqueue_priority
 *  be unrolled one time?  In unrolled each iteration of the loop examines
 *  two "nodes" on the chain being searched.  Otherwise, only one node
 *  is examined per iteration.
 *
 *  If TRUE, then the loops are unrolled.
 *  If FALSE, then the loops are not unrolled.
 *
 *  The primary factor in making this decision is the cost of disabling
 *  and enabling interrupts (_ISR_Flash) versus the cost of rest of the
 *  body of the loop.  On some CPUs, the flash is more expensive than
 *  one iteration of the loop body.  In this case, it might be desirable
 *  to unroll the loop.  It is important to note that on some CPUs, this
 *  code is the longest interrupt disable period in RTEMS.  So it is
 *  necessary to strike a balance when setting this parameter.
 */

#define CPU_UNROLL_ENQUEUE_PRIORITY      FALSE

#ifdef _OLD_EXCEPTIONS
#include <rtems/old-exceptions/cpu.h>
#else
#include <rtems/new-exceptions/cpu.h>
#endif

#ifndef ASM
/*  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 *
 *  This version will work on any processor, but if there is a better
 *  way for your CPU PLEASE use it.  The most common way to do this is to:
 *
 *     swap least significant two bytes with 16-bit rotate
 *     swap upper and lower 16-bits
 *     swap most significant two bytes with 16-bit rotate
 *
 *  Some CPUs have special instructions which swap a 32-bit quantity in
 *  a single instruction (e.g. i486).  It is probably best to avoid
 *  an "endian swapping control bit" in the CPU.  One good reason is
 *  that interrupts would probably have to be disabled to insure that
 *  an interrupt does not try to access the same "chunk" with the wrong
 *  endian.  Another good reason is that on some CPUs, the endian bit
 *  endianness for ALL fetches -- both code and data -- so the code
 *  will be fetched incorrectly.
 */
 
static inline uint32_t CPU_swap_u32(
  uint32_t value
)
{
  uint32_t   swapped;
 
  asm volatile("rlwimi %0,%1,8,24,31;"
	       "rlwimi %0,%1,24,16,23;"
	       "rlwimi %0,%1,8,8,15;"
	       "rlwimi %0,%1,24,0,7;" :
	       "=&r" ((swapped)) : "r" ((value)));

  return( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

#endif /* ASM */

#ifndef ASM
/*
 *  Macros to access PowerPC specific additions to the CPU Table
 */

#define rtems_cpu_configuration_get_clicks_per_usec() \
   (_CPU_Table.clicks_per_usec)

#define rtems_cpu_configuration_get_exceptions_in_ram() \
   (_CPU_Table.exceptions_in_RAM)

#endif /* ASM */

#ifndef /* ASM */
/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define CPU_Get_timebase_low( _value ) \
    asm volatile( "mftb  %0" : "=r" (_value) )

#define rtems_bsp_delay( _microseconds ) \
  do { \
    uint32_t   start, ticks, now; \
    CPU_Get_timebase_low( start ) ; \
    ticks = (_microseconds) * rtems_cpu_configuration_get_clicks_per_usec(); \
    do \
      CPU_Get_timebase_low( now ) ; \
    while (now - start < ticks); \
  } while (0)

#define rtems_bsp_delay_in_bus_cycles( _cycles ) \
  do { \
    uint32_t   start, now; \
    CPU_Get_timebase_low( start ); \
    do \
      CPU_Get_timebase_low( now ); \
    while (now - start < (_cycles)); \
  } while (0)

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPU_H */

