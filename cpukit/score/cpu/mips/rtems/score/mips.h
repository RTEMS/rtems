/*  mips.h
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
/* @(#)mips64orion.h       08/29/96     1.3 */

#ifndef _INCLUDE_MIPS_h
#define _INCLUDE_MIPS_h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASM
#include <idtcpu.h>
#endif

/*
 *  SR bits that enable/disable interrupts
 *
 *  NOTE: XXX what about SR_ERL?
 */

#if __mips == 3
#ifdef ASM
#define SR_INTERRUPT_ENABLE_BITS 0x01
#else
#define SR_INTERRUPT_ENABLE_BITS SR_IE
#endif

#elif __mips == 1
#define SR_INTERRUPT_ENABLE_BITS SR_IEC

#else
#error "mips interrupt enable bits: unknown architecture level!"
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "no cpu"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */
 
#if defined(__mips_soft_float)
#define MIPS_HAS_FPU 0
#else
#define MIPS_HAS_FPU 1
#endif 

#if (__mips == 1)
#define CPU_MODEL_NAME  "ISA Level 1 or 2"
#elif (__mips == 3)
#if defined(__mips64)
#define CPU_MODEL_NAME  "ISA Level 4"
#else
#define CPU_MODEL_NAME  "ISA Level 3"
#endif
#else
#error "Unknown MIPS ISA level"
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "MIPS"

/*
 *  Some macros to access registers
 */

#define mips_get_sr( _x ) \
  do { \
    asm volatile( "mfc0 %0, $12; nop" : "=r" (_x) : ); \
  } while (0)

#define mips_set_sr( _x ) \
  do { \
    register unsigned int __x = (_x); \
    asm volatile( "mtc0 %0, $12; nop" : : "r" (__x) ); \
  } while (0)

/*
 *  Manipulate interrupt mask 
 *
 *  mips_unmask_interrupt( _mask) 
 *    enables interrupts - mask is positioned so it only needs to be or'ed
 *    into the status reg. This also does some other things !!!! Caution
 *    should be used if invoking this while in the middle of a debugging
 *    session where the client may have nested interrupts.
 *
 *  mips_mask_interrupt( _mask )
 *    disable the interrupt - mask is the complement of the bits to be
 *    cleared - i.e. to clear ext int 5 the mask would be - 0xffff7fff
 *
 *
 *  NOTE: mips_mask_interrupt() used to be disable_int().
 *        mips_unmask_interrupt() used to be enable_int().
 *
 */

#define mips_enable_in_interrupt_mask( _mask ) \
  do { \
    unsigned int _sr; \
    mips_get_sr( _sr ); \
    _sr |= (_mask); \
    mips_set_sr( _sr ); \
  } while (0)

#define mips_disable_in_interrupt_mask( _mask ) \
  do { \
    unsigned int _sr; \
    mips_get_sr( _sr ); \
    _sr &= ~(_mask); \
    mips_set_sr( _sr ); \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_MIPS_h */
/* end of include file */
