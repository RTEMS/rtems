/*  i960.h
 *
 *  This include file contains information pertaining to the Intel
 *  i960 processor family.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __i960_h
#define __i960_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the Intel i960
 *  family.  It does this by setting variables to indicate
 *  which implementation dependent features are present
 *  in a particular member of the family.
 *
 *  NOTE: For now i960 support is for models without an FPU.
 *        The stubs for FP routines are in  place so only need to be filled in.
 *
 *  NOTE: RTEMS defines a canonical name for each cpu model.
 */

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "Intel i960"

/*
 *  This should work since most i960 models do not have FPUs. The logic is:
 *
 *    + If the user specifically asks for soft-float, give it to them
 *      regardless of hardware availability.
 *    + If the CPU has hardware FPU, then use it.
 *    + Otherwise, we have to use soft float.
 */

#if defined(_SOFT_FLOAT)
#define I960_HAS_FPU 0
#elif defined(_i960_KB__) || defined(_i960_SB__) || defined(_i960_SB__) || \
      defined(_i960_JF__) || defined(_i960_MC__) || defined(_i960_CC__)
#define I960_HAS_FPU 1
#else
#define I960_HAS_FPU 0
#endif

/*
 *  Some of the CPU models may have better performance with 
 *  alignment of 8 or 16 but we don't know what model we are
 *  being compiled for based solely on the information provided
 *  when multilibbing.
 */

#define I960_CPU_ALIGNMENT 4

/*
 *  This is not the perfect CPU model name but it is adequate and
 *  reflects what we know from multilib.
 */

#if I960_HAS_FPU
#define CPU_MODEL_NAME "w/FPU"
#else
#define CPU_MODEL_NAME "w/soft-float"
#endif
#ifndef ASM


/*
 *  Miscellaneous Support Routines
 */

#define i960_reload_ctl_group( group ) \
 { register int _cmd = ((group)|0x400) ; \
   asm volatile( "sysctl %0,%0,%0" : "=d" (_cmd) : "0" (_cmd) ); \
 }

#define i960_atomic_modify( mask, addr, prev ) \
 { register unsigned int  _mask = (mask); \
   register unsigned int *_addr = (unsigned int *)(addr); \
   asm volatile( "atmod  %0,%1,%1" \
                  : "=d" (_addr), "=d" (_mask) \
                  : "0"  (_addr), "1"  (_mask) ); \
   (prev) = _mask; \
 }

#define atomic_modify( _mask, _address, _previous ) \
  i960_atomic_modify( _mask, _address, _previous )

#define i960_enable_tracing() \
 { register unsigned int _pc = 0x1; \
   asm volatile( "modpc 0,%0,%0" : "=d" (_pc) : "0" (_pc) ); \
 }

/*
 *  Interrupt Level Routines
 */

#define i960_disable_interrupts( oldlevel ) \
  { (oldlevel) = 0x1f0000; \
    asm volatile ( "modpc   0,%1,%1" \
                       : "=d" ((oldlevel)) \
                       : "0"  ((oldlevel)) ); \
  }

#define i960_enable_interrupts( oldlevel ) \
  { unsigned int _mask = 0x1f0000; \
    asm volatile ( "modpc   0,%0,%1" \
                       : "=d" (_mask), "=d" ((oldlevel)) \
                       : "0"  (_mask), "1"  ((oldlevel)) ); \
  }

#define i960_flash_interrupts( oldlevel ) \
  { unsigned int _mask = 0x1f0000; \
    asm volatile ( "modpc   0,%0,%1 ; \
                    mov     %0,%1 ; \
                    modpc   0,%0,%1"  \
                       : "=d" (_mask), "=d" ((oldlevel)) \
                       : "0"  (_mask), "1"  ((oldlevel)) ); \
  }

#define i960_get_interrupt_level( _level ) \
  { \
    i960_disable_interrupts( _level ); \
    i960_enable_interrupts( _level ); \
    (_level) = ((_level) & 0x1f0000) >> 16; \
  } while ( 0 )

#define i960_cause_intr( intr ) \
 { register int _intr = (intr); \
   asm volatile( "sysctl %0,%0,%0" : "=d" (_intr) : "0" (_intr) ); \
 }

/*
 *  Interrupt Masking Routines
 */

static inline unsigned int i960_get_fp()
{ register unsigned int _fp=0;
  asm volatile( "mov fp,%0" : "=d" (_fp) : "0" (_fp) );
  return ( _fp );
}

/*
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 *
 *  This version is based on code presented in Vol. 4, No. 4 of
 *  Insight 960.  It is certainly something you wouldn't think
 *  of on your own.
 */

static inline unsigned int CPU_swap_u32(
  unsigned int value
)
{
  register unsigned int to_swap = value;
  register unsigned int temp    = 0xFF00FF00;
  register unsigned int swapped = 0;

                                            /*  to_swap      swapped  */
  asm volatile ( "rotate  16,%0,%2 ;"       /* 0x12345678  0x56781234 */
                 "modify  %1,%0,%2 ;"       /* 0x12345678  0x12785634 */
                 "rotate  8,%2,%2"          /* 0x12345678  0x78563412 */
                 : "=r" (to_swap), "=r" (temp), "=r" (swapped)
                 : "0" (to_swap), "1" (temp), "2" (swapped)
               );
  return( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

#endif
/* end of include file */
