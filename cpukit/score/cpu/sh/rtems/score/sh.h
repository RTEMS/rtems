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
 
#if defined(sh7032)

#define CPU_MODEL_NAME  "SH 7032"

#define SH_HAS_FPU 	0

/*
 * If the following macro is set to 0 there will be no software irq stack
 */
#define SH_HAS_SEPARATE_STACKS 1

#else
 
#error "Unsupported CPU Model"
 
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "Hitachi SH"

#ifndef ASM

/*
 * Mask for disabling interrupts
 */
#define SH_IRQDIS_VALUE 0xf0

#define sh_disable_interrupts( _level ) \
  asm volatile ( \
    "stc sr,%0\n\t" \
    "ldc %1,sr\n\t"\
  : "=r" (_level ) \
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

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 *
 *  Since we don't have a real time clock, this is a very rough
 *  approximation, assuming that each cycle of the delay loop takes
 *  approx. 4 machine cycles.
 *
 *  e.g.: MHZ = 20 =>     5e-8 secs per instruction
 *                 => 4 * 5e-8 secs per delay loop
 */

#define sh_delay( microseconds ) \
{ register unsigned int _delay = (microseconds) * (MHZ / 4 ); \
  asm volatile ( \
"0:	add  #-1,%0\n \
	nop\n \
	cmp/pl %0\n \
	bt 0b\
	nop" \
    :: "r" (_delay) );  \
}

#define CPU_delay( microseconds ) sh_delay( microseconds )

extern unsigned int sh_set_irq_priority( 
  unsigned int irq, 
  unsigned int prio );

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif
