/*  sh.h
 *
 *  This include file contains information pertaining to the Hitachi SH
 *  processor.
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * 
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef _sh_h
#define _sh_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "SH" family.
 *  
 *  It does  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member of the family.
 */

/*
 *  Figure out all CPU Model Feature Flags based upon compiler 
 *  predefines. 
 */

#if defined(__SH3E__) || defined(__SH4__) || defined(__SH4_SINGLE_ONLY__)

/* 
 * Define this if you want to use XD-registers.
 * Then this registers will be saved/restored on context switch.
 * ! They will not be saved/restored on interrupts!
 */
#define SH4_USE_X_REGISTERS	0

#if defined(__LITTLE_ENDIAN__)
#define SH_HAS_FPU 1
#else
/* FIXME: Context_Control_fp does not support big endian */
#warning FPU not supported
#define SH_HAS_FPU 0
#endif

#elif defined(__sh1__) || defined(__sh2__) || defined(__sh3__)
#define SH_HAS_FPU 0
#else
#warning Cannot detect FPU support, assuming no FPU
#define SH_HAS_FPU 0
#endif

/* this should not be here */
#ifndef CPU_MODEL_NAME
#define CPU_MODEL_NAME  "SH-Multilib"
#endif

/*
 * If the following macro is set to 0 there will be no software irq stack
 */

#ifndef SH_HAS_SEPARATE_STACKS
#define SH_HAS_SEPARATE_STACKS 1
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "Hitachi SH"

#ifndef ASM

#if defined(__sh1__) || defined(__sh2__)

/*
 * Mask for disabling interrupts
 */
#define SH_IRQDIS_VALUE 0xf0

#define sh_disable_interrupts( _level ) \
  asm volatile ( \
    "stc sr,%0\n\t" \
    "ldc %1,sr\n\t"\
  : "=&r" (_level ) \
  : "r" (SH_IRQDIS_VALUE) ); 

#define sh_enable_interrupts( _level ) \
  asm volatile( "ldc %0,sr\n\t" \
    "nop\n\t" \
    :: "r" (_level) );

/*
 *  This temporarily restores the interrupt to _level before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _level is not
 *  modified.
 */
     
#define sh_flash_interrupts( _level ) \
  asm volatile( \
    "ldc %1,sr\n\t" \
    "nop\n\t" \
    "ldc %0,sr\n\t" \
    "nop\n\t" \
    : : "r" (SH_IRQDIS_VALUE), "r" (_level) );

#else

#define SH_IRQDIS_MASK 0xf0

#define sh_disable_interrupts( _level ) \
  asm volatile ( \
    "stc sr,%0\n\t" \
    "mov %0,r5\n\t" \
    "or %1,r5\n\t" \
    "ldc r5,sr\n\t"\
  : "=&r" (_level ) \
  : "r" (SH_IRQDIS_MASK) \
  : "r5" ); 

#define sh_enable_interrupts( _level ) \
  asm volatile( "ldc %0,sr\n\t" \
    "nop\n\t" \
    :: "r" (_level) );

/*
 *  This temporarily restores the interrupt to _level before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _level is not
 *  modified.
 */
     
#define sh_flash_interrupts( _level ) \
  asm volatile( \
    "stc sr,r5\n\t" \
    "ldc %1,sr\n\t" \
    "nop\n\t" \
    "or %0,r5\n\t" \
    "ldc r5,sr\n\t" \
    "nop\n\t" \
    : : "r" (SH_IRQDIS_MASK), "r" (_level) : "r5");

#endif

#define sh_get_interrupt_level( _level ) \
{ \
  register unsigned32 _tmpsr ; \
  \
  asm volatile( "stc sr, %0" : "=r" (_tmpsr) ); \
  _level = (_tmpsr & 0xf0) >> 4 ; \
}

#define sh_set_interrupt_level( _newlevel ) \
{ \
  register unsigned32 _tmpsr; \
  \
  asm volatile ( "stc sr, %0" : "=r" (_tmpsr) ); \
  _tmpsr = ( _tmpsr & ~0xf0 ) | ((_newlevel) << 4) ; \
  asm  volatile( "ldc %0,sr" :: "r" (_tmpsr) ); \
}

/*
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 */
 
static inline unsigned int sh_swap_u32(
  unsigned int value
)
{
  register unsigned int swapped;
  
  asm volatile ( 
    "swap.b %1,%0; "
    "swap.w %0,%0; "
    "swap.b %0,%0" 
    : "=r" (swapped) 
    : "r"  (value) );

  return( swapped );
}

static inline unsigned int sh_swap_u16(
  unsigned int value
)
{
  register unsigned int swapped ;

  asm volatile ( "swap.b %1,%0" : "=r" (swapped) : "r"  (value) );

  return( swapped );
}

#define CPU_swap_u32( value ) sh_swap_u32( value )
#define CPU_swap_u16( value ) sh_swap_u16( value )

extern unsigned int sh_set_irq_priority( 
  unsigned int irq, 
  unsigned int prio );

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif
